#include "framebuffer.h"
#include <limits>

Framebuffer::Framebuffer(int width, int height, bool useZBuffer)
:useZBuffering(useZBuffer)
{
    reset(width, height);
}

void Framebuffer::reset(int width, int height) {
    this->width = width;
    this->height = height;

    color.resize(height*width*3);
    zBuffer.resize(height*width);

#if USE_ATOMS
#else
    vector<mutex> locksNew(height*width);
    locks.swap(locksNew);
#endif

    clear();
}

void Framebuffer::setDimensions(int width, int height) {
    if(width != this->width || height != this->height) {
        reset(width, height);
    }
    else
        clear();
}

void Framebuffer::clear() {
    std::fill(color.begin(), color.end(), 0);

    if (useZBuffering)
        std::fill(zBuffer.begin(), zBuffer.end(), -std::numeric_limits<double>::max());
}

uint16_t Framebuffer::getWidth() const {
    return this->width;
}

uint16_t Framebuffer::getHeight() const {
    return this->height;
}

const uint8_t* Framebuffer::getImage() const {
    if (color.size() > 0) {
        return &color[0];
    }
    
    return nullptr;
}
#if USE_ATOMS
bool Framebuffer::tryZBufferWrite(const vec_t& x, const vec_t& y, const vec_t& z) {
    if (!useZBuffering)
        return true;

    atomic<double>& current = zBuffer[zIndexForPos(x, y)]._a;
    double currentVal = current.load();
    while(z > currentVal) {
        if(current.compare_exchange_strong(currentVal, z))
            return true;
        else
            currentVal = current.load();
    }
    return false;
}
#endif

size_t Framebuffer::zIndexForPos(const vec_t& x, const vec_t& y) {
    return y*width + x;
}

size_t Framebuffer::cIndexForPos(const vec_t& x, const vec_t& y) {
    return y*width*3 + x*3;
}

void Framebuffer::setPixel(const vec_t& x, const vec_t& y, const vec_t& z, const uint8_t (&c)[3]) {
    if(x < 0 || x > width-1 || y < 0 || y > height-1)
        return;
    
#if USE_ATOMS
    if (tryZBufferWrite(x, y, z)) {
        size_t index = cIndexForPos(x, y);
        color[index + 0] = c[0];
        color[index + 1] = c[1];
        color[index + 2] = c[2];
    }
#else
    size_t index = zIndexForPos(x, y);

    std::lock_guard<mutex> lock(locks[index]);

    bool writeColor = false;
    if(!useZBuffering)
        writeColor = true;
    else if(zBuffer[index] < z) {
        zBuffer[index] = z;
        writeColor = true;
    }

    if(writeColor) {
        size_t index = cIndexForPos(x, y);
        color[index + 0] = c[0];
        color[index + 1] = c[1];
        color[index + 2] = c[2];
    }
#endif
}

void Framebuffer::setPixel(const Vec3D& pos, const uint8_t (&c)[3]) {
    return setPixel(pos.x, pos.y, pos.z, c);
}

