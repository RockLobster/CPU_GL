#ifndef ANIMATEDDRAWER_H
#define ANIMATEDDRAWER_H

#include "triangledb.h"
#include "framebuffer.h"
#include "triangledrawer.h"
#include "imageviewer.h"
#include <time.h>
#include <QObject>

class AnimatedDrawer : public QObject
{
    Q_OBJECT

    Framebuffer& fb;
    ImageViewer& viewer;
    const TriangleDB& tdb;

    time_t refSeconds;

    Matrix4x4 translMat;
    Matrix4x4 projMatrix;
    QTimer* animationTimer;

public:
    AnimatedDrawer(Framebuffer& fb, ImageViewer& viewer, const TriangleDB& tdb);
    ~AnimatedDrawer();

    void start();
    void stop();

public slots:
    void update();
};

#endif // ANIMATEDDRAWER_H
