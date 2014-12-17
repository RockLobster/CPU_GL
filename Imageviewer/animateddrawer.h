#ifndef ANIMATEDDRAWER_H
#define ANIMATEDDRAWER_H

#include "triangledb.h"
#include "framebuffer.h"
#include "triangledrawer.h"
#include "imageviewer.h"
#include <time.h>
#include <thread>
#include <QObject>

class AnimatedDrawer : public QObject
{
    Q_OBJECT

    Framebuffer& fb;
    ImageViewer& viewer;
    shared_ptr<TriangleDB> tdb;
    TriangleDrawer drawer;

    time_t refSeconds;

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
