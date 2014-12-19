#ifndef ANIMATEDDRAWER_H
#define ANIMATEDDRAWER_H

#include "triangledb.h"
#include "framebuffer.h"
#include "triangledrawer.h"
#include "imageviewer.h"
#include <time.h>
#include <thread>
#include <QObject>
#include <chrono>
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;

class AnimatedDrawer : public QObject
{
    Q_OBJECT

    Framebuffer& fb;
    ImageViewer& viewer;
    shared_ptr<TriangleDB> tdb;
    TriangleDrawer drawer;

    high_resolution_clock::time_point refSeconds;

    Matrix4x4 translMat;
    Matrix4x4 projMatrix;
    QTimer* animationTimer;
    
    bool _canceled;
    void loop();
    shared_ptr<std::thread> loopThread;

public:
    AnimatedDrawer(Framebuffer& fb, ImageViewer& viewer, shared_ptr<TriangleDB> tdb);
    ~AnimatedDrawer();

    void start();
    void stop();

public slots:
    void update();
    
};

#endif // ANIMATEDDRAWER_H
