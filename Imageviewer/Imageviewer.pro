QT += widgets
qtHaveModule(printsupport): QT += printsupport

CONFIG += c++11

HEADERS       = imageviewer.h \
    ../../MasterThesis/source/Server/matrix4x4.h \
    ../../MasterThesis/source/Server/vec3d.h \
    framebuffer.h \
    triangledb.h \
    triangledrawer.h \
    animateddrawer.h
SOURCES       = imageviewer.cpp \
                main.cpp \
    ../../MasterThesis/source/Server/matrix4x4.cpp \
    ../../MasterThesis/source/Server/vec3d.cpp \
    framebuffer.cpp \
    triangledb.cpp \
    triangledrawer.cpp\
    animateddrawer.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/imageviewer
INSTALLS += target


wince*: {
   DEPLOYMENT_PLUGIN += qjpeg qgif
}
