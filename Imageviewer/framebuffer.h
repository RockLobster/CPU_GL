#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <QImage>
#include <memory>
#include <vector>
#include <atomic>
#include "vec3d.h"
using std::shared_ptr;
using std::vector;
using std::atomic;

template <typename T>
struct atomwrapper
{
    std::atomic<T> _a;
    
    atomwrapper()
    :_a()
    {}
    
    atomwrapper(const std::atomic<T> &a)
    :_a(a.load())
    {}
    
    atomwrapper(const atomwrapper &other)
    :_a(other._a.load())
    {}
    
    void operator=(const T& val) {
        _a.store(val);
    }
    
    atomwrapper &operator=(const atomwrapper &other)
    {
        _a.store(other._a.load());
    }
};

class Framebuffer
{
    //shared_ptr<QImage> q;
    vector<uint8_t> color;
    //uint8_t color[
    
    //vector<vector<double>> zBuffer;
    bool useZBuffering;
    vector<atomwrapper<double>> zBuffer;
    uint16_t width;
    uint16_t height;

    bool tryZBufferWrite(const vec_t& x, const vec_t& y, const vec_t& z);
    
    void reset(int width, int height);
    size_t zIndexForPos(const vec_t& x, const vec_t& y);
    size_t cIndexForPos(const vec_t& x, const vec_t& y);

public:
    Framebuffer(int width, int height, bool useZBuffer);
    void clear();

    void setDimensions(int width, int heigth);
    uint16_t getWidth() const;
    uint16_t getHeight() const;
    
    const uint8_t* getImage() const;
    
    void setPixel(const vec_t& x, const vec_t& y, const vec_t& z, const uint8_t (&c)[3]);
    void setPixel(const Vec3D& pos, const uint8_t (&c)[3]);
};

#endif // FRAMEBUFFER_H
