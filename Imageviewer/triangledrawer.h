#ifndef TRIANGLEDRAWER_H
#define TRIANGLEDRAWER_H

#include <stdint.h>
#include <vector>
#include "framebuffer.h"
#include "triangledb.h"
#include "../../MasterThesis/source/Server/matrix4x4.h"
using std::vector;

class TriangleDrawer
{

public:
    TriangleDrawer();

    static void drawWireframe(Framebuffer& target, const TriangleDB& tdb, const Matrix4x4& pMatrix, const Matrix4x4& mvMatrix);
    static void draw(Framebuffer& target, const TriangleDB& tdb, const Matrix4x4& pMatrix, const Matrix4x4& mvMatrix);
};

#endif // TRIANGLEDRAWER_H
