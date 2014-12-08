#include "triangledrawer.h"
#include <algorithm>
#include <QRgb>
#include <functional>
#include <cmath>
#include <thread>
using std::max;
using std::min;
using std::thread;

struct VertexInfo {
    Vec3D pos;
    Vec3D norm;
};
inline bool xMinCompVi(const VertexInfo& v1, const VertexInfo& v2) {
    return xMinComp(v1.pos, v2.pos);
}
inline bool yMinCompVi(const VertexInfo& v1, const VertexInfo& v2) {
    return yMinComp(v1.pos, v2.pos);
}
inline bool zMinCompVi(const VertexInfo& v1, const VertexInfo& v2) {
    return zMinComp(v1.pos, v2.pos);
}
inline VertexInfo interpolate(const VertexInfo& vi1, const VertexInfo& vi2, double alpha) {
    return {    Vec3D::interpolate(vi1.pos, vi2.pos, alpha),
                Vec3D::interpolate(vi1.norm, vi2.norm, alpha)};
}

TriangleDrawer::TriangleDrawer()
{
}

QRgb perFragmentCalc(const VertexInfo& vi, const Vec3D& lightDir) {
    const Vec3D ambientColor(100, 100, 255);
    const Vec3D diffuseColor(255, 255, 255);
    double ambAlpha = 0.1;

    //Vec3D lightDir          = (lightPos - vi.pos).getNormalized();
    double diffuseFactor    = max(0.0, vi.norm.dotProduct(lightDir));

    Vec3D resultColor = (ambAlpha*ambientColor) + ((1-ambAlpha)*diffuseColor) * diffuseFactor;
    return qRgb(resultColor.x, resultColor.y, resultColor.z);
}


typedef std::function<void(Framebuffer&,const VertexInfo&,const VertexInfo&, const Vec3D&)> drawFuncType;
void drawHorizontalLine(Framebuffer& img, const VertexInfo& vi1, const VertexInfo& vi2, const Vec3D& lightDir) {
    pair<VertexInfo,VertexInfo> range = std::minmax(vi1, vi2, xMinCompVi);
    range.first.pos.round();
    range.second.pos.round();

    if(range.first.pos.y < 0 || range.first.pos.y >= img.getHeight())
        return;

    double rangeWidth = range.second.pos.x-range.first.pos.x;
    
    if(rangeWidth == 0) {
        if(img.tryZBufferWrite(vi1.pos)) {
            QRgb color = perFragmentCalc(vi1, lightDir);
            img.setPixel(vi1.pos, color);
        }
        return;
    }
    
    for(int x = max(range.first.pos.x, 0.0); x <= min(range.second.pos.x, (double)(img.getWidth()-1)); x++) {
        double alpha = ((double)x-range.first.pos.x)/rangeWidth;
        auto pos = Vec3D::interpolate(range.first.pos, range.second.pos, alpha);
        pos.round();

        if(img.tryZBufferWrite(pos)) {
            auto norm = Vec3D::interpolate(range.first.norm, range.second.norm, alpha);
            norm.normalize();
            QRgb color = perFragmentCalc({pos,norm}, lightDir);
            img.setPixel(pos, color);
        }
    }
}

void drawVerticallLine(Framebuffer& img, const VertexInfo& vi1, const VertexInfo& vi2, const Vec3D& lightDir) {
    pair<VertexInfo,VertexInfo> range = std::minmax(vi1, vi2, yMinCompVi);
    range.first.pos.round();
    range.second.pos.round();

    if(range.first.pos.x < 0 || range.first.pos.x >= img.getWidth())
        return;

    double rangeWidth = range.second.pos.y-range.first.pos.y;
    
    if(rangeWidth == 0) {
        if(img.tryZBufferWrite(range.first.pos)) {
            QRgb color = perFragmentCalc(range.first, lightDir);
            img.setPixel(range.first.pos, color);
        }
        return;
    }
    
    for(int y = max(range.first.pos.y, 0.0); y <= min(range.second.pos.y, (double)(img.getHeight()-1)); y++) {
        double alpha = ((double)y-range.first.pos.y)/rangeWidth;
        auto pos = Vec3D::interpolate(range.first.pos, range.second.pos, alpha);
        pos.round();

        if(img.tryZBufferWrite(pos)) {
            auto norm = Vec3D::interpolate(range.first.norm, range.second.norm, alpha);
            norm.normalize();
            QRgb color = perFragmentCalc({pos,norm}, lightDir);
            img.setPixel(pos, color);
        }
    }
}

void internalBresenham(Framebuffer& img, Vec3D v1, const Vec3D& v2, bool steep) {
    QRgb color = qRgb(254, 254, 254);

    double dX = v2.x - v1.x;
    double dY = v2.y - v1.y;

    int yStep    = (dY >= 0) ? 1 : -1;
    double slope = fabs(dY/dX);
    double error = 0;

    int x = v1.x;
    int y = v1.y;
    int maxX = v2.x;

    if(steep) {
        v1.x = y;
        v1.y = x;
    }

    img.setPixel(v1, color);
    for(x = x+1; x <= maxX; x++) {
        error += slope;
        if(error >= 0.5) {
            y += yStep;
            error--;
        }
        v1.x = steep ? y : x;
        v1.y = steep ? x : y;
        img.setPixel(v1, color);
    }
}

void bresenham(Framebuffer& img, Vec3D v1, Vec3D v2) {
    v1.round();
    v2.round();

    bool steep = (fabs(v2.y-v1.y) > fabs(v2.x-v1.x));
    if(steep) {
        v1 = Vec3D(v1.y, v1.x);
        v2 = Vec3D(v2.y, v2.x);
    }
    if(v2.x < v1.x)
        internalBresenham(img, v2, v1, steep);
    else
        internalBresenham(img, v1, v2, steep);
}

Vec3D ndcToPix(const Framebuffer& img, const Vec3D& vec) {
    return {   floor((1 + vec.x)/2 * (img.getWidth()-1)),
               floor((1 + vec.y)/2 * (img.getHeight()-1)),
               floor(vec.z)};
}

void TriangleDrawer::drawWireframe(Framebuffer &target, const TriangleDB &tdb, const Matrix4x4& pMatrix, const Matrix4x4& mvMatrix) {
    Matrix4x4 pmvMatrix = pMatrix * mvMatrix;

    for(size_t i = 0; i < tdb.triangleIndexes.size()/3; i++) {
        vector<Vec3D> triVert = {ndcToPix(target, pmvMatrix * tdb.vertices[tdb.triangleIndexes[i*3 + 0]]),
                                 ndcToPix(target, pmvMatrix * tdb.vertices[tdb.triangleIndexes[i*3 + 1]]),
                                 ndcToPix(target, pmvMatrix * tdb.vertices[tdb.triangleIndexes[i*3 + 2]])};

        bresenham(target, triVert[0], triVert[1]);
        bresenham(target, triVert[0], triVert[2]);
        bresenham(target, triVert[1], triVert[2]);
    }
}

#define SINGLE_THREADED 0
void parallelFor(size_t totalLoad, std::function<void(int)> func) {
#if SINGLE_THREADED
    for(size_t i = 0; i < totalLoad; i++)
        func(i);
#else
    int numthreads = std::thread::hardware_concurrency();
    vector<std::thread> threads;

    auto threadOp = [&](int threadId) {
        int load   = totalLoad/numthreads;
        int offset = load*threadId;
        if(threadId == numthreads-1)
            load = totalLoad-offset;

        for(int i = 0; i < load; i++)
            func(offset + i);
    };
    for (int threadId = 0; threadId < numthreads; threadId++) {
        threads.push_back(thread(threadOp, threadId));
    }
    //threadOp(0);
    for(thread& t : threads)
        t.join();
#endif
}

#define RANGE_CHECK 0
#define BACK_FACE_CULLING 1
void TriangleDrawer::draw(Framebuffer &target, const TriangleDB &tdb, const Matrix4x4& pMatrix, const Matrix4x4& mvMatrix) {
    Vec3D lightDir = (Vec3D(0,0,0) - tdb.getCenter()).getNormalized();

    Matrix4x4 pmvMatrix = pMatrix * mvMatrix;
    //Matrix4x4 normalMat = Matrix4x4::identityMatrix();
    Matrix4x4 normalMat = mvMatrix;//.normalMatrix();

    vector<Vec3D> vertices(tdb.vertices.size());
    vector<Vec3D> normals(tdb.normals.size());

    parallelFor(tdb.vertices.size(), [&](int index){
        vertices[index] = pmvMatrix * tdb.vertices[index];
        normals[index]  = normalMat * Vec4D(tdb.normals[index], 0);
    });

    parallelFor(tdb.triangleIndexes.size()/3, [&](int i){
        uint32_t index1 = tdb.triangleIndexes[i*3 + 0];
        uint32_t index2 = tdb.triangleIndexes[i*3 + 1];
        uint32_t index3 = tdb.triangleIndexes[i*3 + 2];

        vector<VertexInfo> triVert = {{ndcToPix(target, vertices[index1]), normals[index1]},
                                      {ndcToPix(target, vertices[index2]), normals[index2]},
                                      {ndcToPix(target, vertices[index3]), normals[index3]}};



#if BACK_FACE_CULLING
        Vec3D test(0,0,1);
        if(triVert[0].norm.dotProduct(test) < 0
                && triVert[1].norm.dotProduct(test) < 0
                && triVert[2].norm.dotProduct(test) < 0)
            return;
#endif

        int firstRange, secondRange;
        drawFuncType drawingFunc;
#if RANGE_CHECK
        std::pair<int, int> minmaxX = std::minmax({triVert[0].pos.x, triVert[1].pos.x, triVert[2].pos.x});
        std::pair<int, int> minmaxY = std::minmax({triVert[0].pos.y, triVert[1].pos.y, triVert[2].pos.y});
        int xRange = 1 + minmaxX.second - minmaxX.first;
        int yRange = 1 + minmaxY.second - minmaxY.first;

        if(xRange >= yRange) {
#else
        if(false) {
#endif
            std::sort(triVert.begin(), triVert.end(), yMinCompVi);
            firstRange  = triVert[1].pos.y - triVert[0].pos.y;
            secondRange = triVert[2].pos.y - triVert[1].pos.y;
            drawingFunc = drawHorizontalLine;
        }
        else {
            std::sort(triVert.begin(), triVert.end(), xMinCompVi);
            firstRange  = triVert[1].pos.x - triVert[0].pos.x;
            secondRange = triVert[2].pos.x - triVert[1].pos.x;
            drawingFunc = drawVerticallLine;
        }

        drawingFunc(target, triVert[0], triVert[0], lightDir);
        for(int offset = 1; offset <= firstRange; offset++) {
            double alpha1 = ((double)offset)/firstRange;
            double alpha2 = ((double)offset)/(firstRange+secondRange);
            auto vi1 = interpolate(triVert[0], triVert[1], alpha1);
            auto vi2 = interpolate(triVert[0], triVert[2], alpha2);
            drawingFunc(target, vi1, vi2, lightDir);
        }
        for(int offset = 1; offset <= secondRange; offset++) {
            double alpha1 = ((double)offset)/secondRange;
            double alpha2 = ((double)offset+firstRange)/(firstRange+secondRange);
            auto vi1 = interpolate(triVert[1], triVert[2], alpha1);
            auto vi2 = interpolate(triVert[0], triVert[2], alpha2);
            drawingFunc(target, vi1, vi2, lightDir);
        }
    });
}
