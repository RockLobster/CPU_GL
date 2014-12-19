/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwidget.h"

#include <QMouseEvent>
#include <iostream>
#include <math.h>

inline Matrix4x4 autoProjMatrix(const Framebuffer &target, const TriangleDB &tdb) {
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

MainWidget::MainWidget(QWidget *parent, shared_ptr<Framebuffer> fb, shared_ptr<TriangleDB> tdb) :
    QOpenGLWidget(parent),
    fb(fb),
    tdb(tdb)
{
    translMat  = Matrix4x4::translationMatrix(tdb->getCenter());
    projMatrix = autoProjMatrix(*fb, *tdb);
}

MainWidget::~MainWidget()
{
    // Make sure the context is current when deleting the texture
    // and the buffers.
    makeCurrent();
    doneCurrent();
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void MainWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    m_xRot = angle;
}

void MainWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    m_yRot = angle;
}

void MainWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    m_zRot = angle;
}

void MainWidget::mousePressEvent(QMouseEvent *event)
{
    // Save mouse press position
    m_lastPos = event->pos();
}

void MainWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - m_lastPos.x();
    int dy = event->y() - m_lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(m_xRot + 8 * dy);
        setYRotation(m_yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(m_xRot + 8 * dy);
        setZRotation(m_zRot + 8 * dx);
    }
    m_lastPos = event->pos();
    update();
}

QSize MainWidget::minimumSizeHint() const
{
    return QSize(400, 400);
}

QSize MainWidget::sizeHint() const
{
    return QSize(fb->getWidth(), fb->getHeight());
}

void MainWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glClearColor(0, 0, 0, 1);
}

void MainWidget::resizeGL(int w, int h)
{
    printf("resize!\n");
    fb->setDimensions (w, h);
    projMatrix = autoProjMatrix(*fb, *tdb);
    update();
}

void MainWidget::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    fb->clear();

    Matrix4x4 rotMatrix;
    rotMatrix.rotate((m_xRot / 16.0f), {1, 0, 0});
    rotMatrix.rotate((m_yRot / 16.0f), {0, 1, 0});
    rotMatrix.rotate(m_zRot / 16.0f, {0, 0, 1});
    Matrix4x4 mvMatrix = translMat * rotMatrix * translMat.inverted();

    drawer.draw(*fb, *tdb, projMatrix, mvMatrix);
    //drawer.drawWireframe(*fb, *tdb, projMatrix, mvMatrix);

    glDrawPixels(fb->getWidth(), fb->getHeight(), GL_RGB, GL_UNSIGNED_BYTE, fb->getImage());
}
