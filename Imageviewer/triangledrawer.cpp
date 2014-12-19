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
bool TriangleDrawer::operator()( const vindex_t & i1, const vindex_t & i2 ) {
    return _rendVs[i1].x < _rendVs[i2].x;
}

inline VertexInfo interpolate(VertexInfo vi1, const VertexInfo& vi2, float alpha) {
    vi1.pos = Vec3D::interpolate(vi1.pos, vi2.pos, alpha);
    vi1.norm = Vec3D::interpolate(vi1.norm, vi2.norm, alpha);
    return vi1;
}

inline VertexInfo interpolate(VertexInfo vi1, const Vec3D& otherPos, const Vec3D& otherN, const double& alpha) {
    vi1.pos.interpolate(otherPos, alpha);
    vi1.norm.interpolate(otherN, alpha);
    return vi1;
}

TriangleDrawer::TriangleDrawer()
{
}

void perFragmentCalc(const VertexInfo& vi, const Vec3D& lightDir, uint8_t (&c)[3]) {
    const Vec3D ambientColor(100, 100, 255);
    const Vec3D diffuseColor(255, 255, 255);
    const vec_t ambAlpha = 0.1;

    //Vec3D lightDir          = (lightPos - vi.pos).getNormalized();
    vec_t diffuseFactor = max(0.0f, vi.norm.dotProduct(lightDir));

    Vec3D resultColor = (ambAlpha*ambientColor) + ((1.0-ambAlpha)*diffuseColor) * diffuseFactor;
    
    c[0] = resultColor.x;
    c[1] = resultColor.y;
    c[2] = resultColor.z;
}


typedef std::function<void(Framebuffer&,const VertexInfo&,const VertexInfo&, const Vec3D&)> drawFuncType;
void drawHorizontalLine(Framebuffer& img, const VertexInfo& vi1, const VertexInfo& vi2, const Vec3D& lightDir) {
    pair<VertexInfo,VertexInfo> range = std::minmax(vi1, vi2, xMinCompVi);
    range.first.pos.round();
    range.second.pos.round();

    if(range.first.pos.y < 0 || range.first.pos.y >= img.getHeight())
        return;

    float rangeWidth = range.second.pos.x-range.first.pos.x;
    
    if(rangeWidth == 0) {
        uint8_t color[3];
        perFragmentCalc(vi1, lightDir, color);
        img.setPixel(vi1.pos, color);
        return;
    }
    
    for(int x = max(range.first.pos.x, 0.0f); x <= min(range.second.pos.x, (vec_t)(img.getWidth()-1)); x++) {
        float alpha = ((float)x-range.first.pos.x)/rangeWidth;
        auto pos = Vec3D::interpolate(range.first.pos, range.second.pos, alpha);
        pos.round();
        
        auto norm = Vec3D::interpolate(range.first.norm, range.second.norm, alpha);
        norm.normalize();
        
        uint8_t color[3];
        perFragmentCalc({pos,norm}, lightDir, color);
        img.setPixel(pos, color);
    }
}

void drawVerticallLine(Framebuffer& img, const VertexInfo& vi1, const VertexInfo& vi2, const Vec3D& lightDir) {
    pair<VertexInfo,VertexInfo> range = std::minmax(vi1, vi2, yMinCompVi);
    range.first.pos.round();
    range.second.pos.round();

    if(range.first.pos.x < 0 || range.first.pos.x >= img.getWidth())
        return;

    float rangeWidth = range.second.pos.y-range.first.pos.y;
    
    if(rangeWidth == 0) {
        uint8_t color[3];
        perFragmentCalc(range.first, lightDir, color);
        img.setPixel(range.first.pos, color);
        return;
    }
    
    for(int y = max(range.first.pos.y, 0.0f); y <= min(range.second.pos.y, (vec_t)(img.getHeight()-1)); y++) {
        float alpha = ((float)y-range.first.pos.y)/rangeWidth;
        auto pos = Vec3D::interpolate(range.first.pos, range.second.pos, alpha);
        pos.round();

        auto norm = Vec3D::interpolate(range.first.norm, range.second.norm, alpha);
        norm.normalize();
        
        uint8_t color[3];
        perFragmentCalc({pos,norm}, lightDir, color);
        img.setPixel(pos, color);
    }
}

void internalBresenham(Framebuffer& img, vec_t x1, vec_t y1, const vec_t& x2, const vec_t& y2, bool steep) {
    uint8_t color[3] = {254, 254, 254};
    
    float dX = x2 - x1;
    float dY = y2 - y1;

    int yStep    = (dY >= 0) ? 1 : -1;
    float slope = fabs(dY/dX);
    float error = 0;

    if (steep)
        img.setPixel(y1, x1, 0, color);
    else
        img.setPixel(x1, y1, 0, color);
    
    for(x1 = x1+1; x1 <= x2; x1++) {
        error += slope;
        if(error >= 0.5) {
            y1 += yStep;
            error--;
        }
        
        if (steep)
            img.setPixel(y1, x1, 0, color);
        else
            img.setPixel(x1, y1, 0, color);
    }
}

void bresenham(Framebuffer& img, const Vec3D& v1, const Vec3D& v2) {
    //v1.round();
    //v2.round();

    bool steep = (fabs(v2.y-v1.y) > fabs(v2.x-v1.x));
    
    if(v2.x < v1.x) {
        if (steep)
            internalBresenham(img, v2.y, v2.x, v1.y, v1.x, steep);
        else
            internalBresenham(img, v2.x, v2.y, v1.x, v1.y, steep);
    }
    else {
        if (steep)
            internalBresenham(img, v1.y, v1.x, v2.y, v2.x, steep);
        else
            internalBresenham(img, v1.x, v1.y, v2.x, v2.y, steep);
    }
}

Vec3D ndcToPix(const Framebuffer& img, const Vec3D& vec) {
    return {   floorf((1 + vec.x)/2 * (img.getWidth()-1)),
               floorf((1 + vec.y)/2 * (img.getHeight()-1)),
               floorf(vec.z)};
}

void parallelFor(size_t totalLoad, std::function<void(size_t)> func) {
    size_t numthreads = std::thread::hardware_concurrency();
    vector<std::thread> threads;
    
    auto threadOp = [&](size_t threadId) {
        size_t load   = totalLoad/numthreads;
        size_t offset = load*threadId;
        if(threadId == numthreads-1)
            load = totalLoad-offset;
        
        for(size_t i = 0; i < load; i++)
            func(offset + i);
    };
    for (size_t threadId = 1; threadId < numthreads; threadId++) {
        threads.push_back(thread(threadOp, threadId));
    }
    threadOp(0);
    for(thread& t : threads)
        t.join();
}

void TriangleDrawer::drawWireframe(Framebuffer &target, const TriangleDB &tdb, const Matrix4x4& pMatrix, const Matrix4x4& mvMatrix) {
    Matrix4x4 pmvMatrix = pMatrix * mvMatrix;
    
    _rendWVs.resize(tdb.vertices.size());
    
    //for (size_t index = 0; index < tdb.vertices.size(); index++) {
    parallelFor(tdb.vertices.size(), [&](size_t index){
        _rendWVs[index] = ndcToPix(target, pmvMatrix * tdb.vertices[index]);
    });
    //}
    
    parallelFor(tdb.triangleIndexes.size()/3, [&](size_t i){
    //for(size_t i = 0; i < tdb.triangleIndexes.size()/3; i++) {
        const vindex_t& index1 = tdb.triangleIndexes[i*3 + 0];
        const vindex_t& index2 = tdb.triangleIndexes[i*3 + 1];
        const vindex_t& index3 = tdb.triangleIndexes[i*3 + 2];
        
        bresenham(target, _rendWVs[index1], _rendWVs[index2]);
        bresenham(target, _rendWVs[index1], _rendWVs[index3]);
        bresenham(target, _rendWVs[index2], _rendWVs[index3]);
    //}
    });
}

#define RANGE_CHECK 0
#define BACK_FACE_CULLING 1
void TriangleDrawer::draw(Framebuffer &target, const TriangleDB &tdb, const Matrix4x4& pMatrix, const Matrix4x4& mvMatrix) {
    Vec3D lightDir = (Vec3D(0,0,0) - tdb.getCenter()).getNormalized();

    Matrix4x4 pmvMatrix = pMatrix * mvMatrix;
    //Matrix4x4 normalMat = Matrix4x4::identityMatrix();
    Matrix4x4 normalMat = mvMatrix;//.normalMatrix();

    _rendVs.resize(tdb.vertices.size());
    _rendNs.resize(tdb.normals.size());
    
    //for (size_t index = 0; index < tdb.vertices.size(); index++) {
    parallelFor(tdb.vertices.size(), [&](size_t index){
        _rendVs[index] = ndcToPix(target, pmvMatrix * tdb.vertices[index]);
        _rendNs[index] = normalMat * Vec4D(tdb.normals[index], 0);
    });
    //}

    parallelFor(tdb.triangleIndexes.size()/3, [&](size_t i) {
    //for (size_t i = 0; i < tdb.triangleIndexes.size()/3; i++) {
        const uint32_t& index1 = tdb.triangleIndexes[i*3 + 0];
        const uint32_t& index2 = tdb.triangleIndexes[i*3 + 1];
        const uint32_t& index3 = tdb.triangleIndexes[i*3 + 2];

        vector<VertexInfo> triVert = {
            {_rendVs[index1], _rendNs[index1]},
            {_rendVs[index2], _rendNs[index2]},
            {_rendVs[index3], _rendNs[index3]}};

#if BACK_FACE_CULLING
        const Vec3D test(0,0,1);
        if(triVert[0].norm.dotProduct(test) < 0
           && triVert[1].norm.dotProduct(test) < 0
           && triVert[2].norm.dotProduct(test) < 0) {
            return;
        }
#endif

        drawFuncType drawingFunc;
        int firstRange, secondRange;
        float alpha1, alpha2;
        VertexInfo vi1, vi2;
        int offset;
        
#if RANGE_CHECK
        
        std::pair<int, int> minmaxX = std::minmax({triVert[0].pos.x, triVert[1].pos.x, triVert[2].pos.x});
        std::pair<int, int> minmaxY = std::minmax({triVert[0].pos.y, triVert[1].pos.y, triVert[2].pos.y});
        int xRange = 1 + minmaxX.second - minmaxX.first;
        int yRange = 1 + minmaxY.second - minmaxY.first;
        
        if(xRange >= yRange) {
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
#else
        if(false) {
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
#endif

        drawingFunc(target, triVert[0], triVert[0], lightDir);
        for(offset = 1; offset <= firstRange; offset++) {
            alpha1 = ((float)offset)/firstRange;
            alpha2 = ((float)offset)/(firstRange+secondRange);
            vi1 = interpolate(triVert[0], triVert[1], alpha1);
            vi2 = interpolate(triVert[0], triVert[2], alpha2);
            drawingFunc(target, vi1, vi2, lightDir);
        }
        for(offset = 1; offset <= secondRange; offset++) {
            alpha1 = ((float)offset)/secondRange;
            alpha2 = ((float)offset+firstRange)/(firstRange+secondRange);
            vi1 = interpolate(triVert[1], triVert[2], alpha1);
            vi2 = interpolate(triVert[0], triVert[2], alpha2);
            drawingFunc(target, vi1, vi2, lightDir);
        }

    //}
    });
}
