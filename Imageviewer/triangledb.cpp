#include "triangledb.h"
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
using std::ifstream;
using std::string;
using std::istringstream;
using std::getline;
/*
double randomNDC() {
    double r = (double)rand();
    r /= RAND_MAX;
    return r;
}*/

TriangleDB::TriangleDB()
{
    vertices.push_back(Vec3D(-0.5, -0.5, 0));
    vertices.push_back(Vec3D(0, 0, 0));
    vertices.push_back(Vec3D(0, 1, 0));
    vertices.push_back(Vec3D(0.7, 0, 0));
    vertices.push_back(Vec3D(0, 1, 0));
    vertices.push_back(Vec3D(0.5, -0.5, 0));

    /*triangleIndexes = {0, 1, 2,
                       1, 4, 3};*/
    triangleIndexes = {0, 3, 2};
    /*
    moveDir.reserve(vertices.size());
    for(Vec3D& vec : vertices) {
        moveDir.push_back(Vec3D());
    }*/
}

TriangleDB::TriangleDB(string filename) {
    ifstream infile(filename);

    string line;
    while (getline(infile, line)) {
        istringstream iss(line);

        string descriptor;
        if(!(iss >> descriptor))
            abort();

        if(descriptor == "#") {
            size_t vertC, faceC;
            string s1, s2;
            if(!(iss >> vertC >> s1 >> faceC >> s2))
                abort();

            vertices.reserve(vertC);
            triangleIndexes.reserve(faceC*3);
        }
        else if(descriptor == "v") {
            Vec3D pos;
            if(!(iss >> pos.x >> pos.y >> pos.z))
                abort();

            vertices.push_back(pos);
        }
        else if(descriptor == "f") {
            uint32_t v1, v2, v3;
            if(!(iss >> v1 >> v2 >> v3))
                abort();

            triangleIndexes.push_back(v1-1);
            triangleIndexes.push_back(v2-1);
            triangleIndexes.push_back(v3-1);
        }
    }

    generateNormals();
}

Vec3D normalForTriangle(const Vec3D& v1, const Vec3D& v2, const Vec3D& v3) {
    return (v2-v1).crossProduct(v3-v1).getNormalized();
}

void TriangleDB::generateNormals() {
    normals.clear();
    normals.resize(vertices.size());

    for(size_t i = 0; i < triangleIndexes.size()/3; i++) {
        uint32_t index1 = triangleIndexes[i*3 + 0];
        uint32_t index2 = triangleIndexes[i*3 + 1];
        uint32_t index3 = triangleIndexes[i*3 + 2];

        Vec3D normal = normalForTriangle(vertices[index1], vertices[index2], vertices[index3]);

        normals[index1] += normal;
        normals[index2] += normal;
        normals[index3] += normal;
    }

    for(Vec3D& norm : normals)
        norm.normalize();
}

Vec3D TriangleDB::getCenter() const {
    Vec3D center;

    for(const Vec3D& vec : vertices)
        center += vec;

    if(vertices.size() > 0)
        center /= vertices.size();

    return center;
}

minMaxType TriangleDB::minMaxX() const {
    auto range = std::minmax_element(vertices.begin(), vertices.end(), xMinComp);
    return minMaxType(range.first->x, range.second->x);
}

minMaxType TriangleDB::minMaxY() const {
    auto range = std::minmax_element(vertices.begin(), vertices.end(), yMinComp);
    return minMaxType(range.first->y, range.second->y);
}

minMaxType TriangleDB::minMaxZ() const {
    auto range = std::minmax_element(vertices.begin(), vertices.end(), zMinComp);
    return minMaxType(range.first->z, range.second->z);
}
