#include "animateddrawer.h"
#include <QtWidgets>

Matrix4x4 autoProjMatrix(const Framebuffer &target, const TriangleDB &tdb) {
    double aspectRatio = ((double)target.getWidth())/target.getHeight();

    auto xDims = tdb.minMaxX();
    auto yDims = tdb.minMaxY();
    double xrWidth = xDims.second - xDims.first;
    double yrWidth = yDims.second - yDims.first;
    double dataAspectRatio = xrWidth/yrWidth;

    const double testMulti = 1.2;

    double nw, nh;
    if(aspectRatio/dataAspectRatio > 1.0) {
        nh = yrWidth / testMulti;
        nw = nh*aspectRatio;
    }
    else {
        nw = xrWidth / testMulti;
        nh = nw/aspectRatio;
    }

    return Matrix4x4::orthoProjectionMatrix(nw, nh);
}

AnimatedDrawer::AnimatedDrawer(Framebuffer &fb, ImageViewer &viewer, const TriangleDB &tdb)
    :QObject(0), fb(fb), viewer(viewer), tdb(tdb) {

    translMat  = Matrix4x4::translationMatrix(tdb.getCenter());
    projMatrix = autoProjMatrix(fb, tdb);
    refSeconds = time(NULL);
    update();
}

AnimatedDrawer::~AnimatedDrawer() {
    stop();
}

void AnimatedDrawer::update() {

    fb.clear();
    double seconds = difftime(time(NULL), refSeconds);

    Matrix4x4 mvMatrix = translMat * Matrix4x4::yRotMatrix(10*seconds) * translMat.inverted();

    TriangleDrawer::draw(fb, tdb, projMatrix, mvMatrix);
    //TriangleDrawer::drawWireframe(fb, tdb, projMatrix, mvMatrix);
    viewer.loadPixmap(QPixmap::fromImage(fb.getImage()));
}

void AnimatedDrawer::start() {
    if(animationTimer)
        return;

    animationTimer = new QTimer(this);
    connect(animationTimer, SIGNAL(timeout()), this, SLOT(update()));
    animationTimer->start(33);
}

void AnimatedDrawer::stop() {
    if(animationTimer) {
        animationTimer->stop();
        animationTimer->deleteLater();
    }
}
