#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <QImage>
#include <memory>
#include <vector>
#include <atomic>
#include "../../MasterThesis/source/Server/vec3d.h"
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

  atomwrapper &operator=(const atomwrapper &other)
  {
    _a.store(other._a.load());
  }
};

class Framebuffer
{
    shared_ptr<QImage> q;
    //vector<vector<double>> zBuffer;
    vector<vector<atomwrapper<double>>> zBuffer;
    int width;
    int height;

    void reset(int width, int height);

public:
    Framebuffer(int width, int height);
    void clear();

    void setDimensions(int width, int heigth);
    int getWidth() const;
    int getHeight() const;

    const QImage& getImage() const;
    bool tryZBufferWrite(const Vec3D& pos);
    void setPixel(const Vec3D& pos, QRgb index_or_rgb);
};

#endif // FRAMEBUFFER_H
