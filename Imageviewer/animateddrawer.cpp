#include "animateddrawer.h"
#include <QtWidgets>

Matrix4x4 autoProjMatrix(const Framebuffer &target, const TriangleDB &tdb) {
    double aspectRatio = ((double)target.getWidth())/target.getHeight();

    auto xDims = tdb.minMaxX();
    auto yDims = tdb.minMaxY();
    double xrWidth = xDims.second - xDims.first;
    double yrWidth = yDims.second - yDims.first;
    double dataAspectRatio = xrWidth/yrWidth;

    const double testMulti = 0.8;

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

AnimatedDrawer::AnimatedDrawer(Framebuffer &fb, ImageViewer &viewer, shared_ptr<TriangleDB> tdb)
    :QObject(0), fb(fb), viewer(viewer), tdb(tdb) {

    translMat  = Matrix4x4::translationMatrix(tdb->getCenter());
    projMatrix = autoProjMatrix(fb, *tdb);
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

    drawer.draw(fb, *tdb, projMatrix, mvMatrix);
    //drawer.drawWireframe(fb, *tdb, projMatrix, mvMatrix);
    
    QImage img(fb.getImage(), fb.getWidth(), fb.getHeight(), QImage::Format_RGB888);
    viewer.loadPixmap(QPixmap::fromImage(img));
}

void AnimatedDrawer::loop() {
    while (!_canceled) {
        update();
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }
}

void AnimatedDrawer::start() {
    if(animationTimer)
        return;

    _canceled = false;
    
    loopThread = std::make_shared<std::thread>(&AnimatedDrawer::loop, this);
    //animationTimer = new QTimer(this);
    //connect(animationTimer, SIGNAL(timeout()), this, SLOT(update()));
    //animationTimer->start(1);
}

void AnimatedDrawer::stop() {
    _canceled = true;
    if (loopThread) {
        loopThread->join();
        loopThread.reset();
    }
    
    if(animationTimer) {
        animationTimer->stop();
        animationTimer->deleteLater();
    }
}
