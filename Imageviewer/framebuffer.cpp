#include "framebuffer.h"
#include <limits>

Framebuffer::Framebuffer(int width, int height)
{
    reset(width, height);
}

void Framebuffer::reset(int width, int height) {
    this->width = width;
    this->height = height;

    q = std::make_shared<QImage>(width, height, QImage::Format_RGB32);
    zBuffer.clear();
    zBuffer.resize(width);
    for(auto& i : zBuffer) {
        i.resize(height);
    }
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
    q->fill(qRgb(0,0,0));

    for(auto& i : zBuffer) {
        for(auto& wrap : i) {
            wrap._a = -std::numeric_limits<double>::max();
        }
    }
}

int Framebuffer::getWidth() const {
    return this->width;
}

int Framebuffer::getHeight() const {
    return this->height;
}

const QImage& Framebuffer::getImage() const {
    return *q;
}

bool Framebuffer::tryZBufferWrite(const Vec3D& pos) {
    const double& x = pos.x;
    const double& y = pos.y;

    if(x < 0 || x > width-1 || y < 0 || y > height-1)
        return false;

    atomic<double>& current = zBuffer[x][y]._a;
    double currentVal = current.load();
    while(pos.z > currentVal) {
        if(current.compare_exchange_strong(currentVal, pos.z))
            return true;
        else {
            currentVal = current.load();
        }
    }
    return false;
}

void Framebuffer::setPixel(const Vec3D& pos, QRgb index_or_rgb) {
    const double& x = pos.x;
    const double& y = pos.y;

    if(x < 0 || x > width-1 || y < 0 || y > height-1)
        return;

    q->setPixel(x, this->height-1-y, index_or_rgb);
}

