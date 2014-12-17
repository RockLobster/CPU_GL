#ifndef TRIANGLEDRAWER_H
#define TRIANGLEDRAWER_H

#include <stdint.h>
#include <vector>
#include "framebuffer.h"
#include "triangledb.h"
#include "matrix4x4.h"
using std::vector;

class TriangleDrawer
{
    vector<Vec3D> _rendWVs;
    vector<Vec3D> _rendVs;
    vector<Vec3D> _rendNs;
    bool operator()( const vindex_t& i1, const vindex_t& i2 );
    
public:
    TriangleDrawer();

    void drawWireframe(Framebuffer& target, const TriangleDB& tdb, const Matrix4x4& pMatrix, const Matrix4x4& mvMatrix);
    void draw(Framebuffer& target, const TriangleDB& tdb, const Matrix4x4& pMatrix, const Matrix4x4& mvMatrix);
};

#endif // TRIANGLEDRAWER_H
