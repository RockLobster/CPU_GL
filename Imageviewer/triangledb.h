#ifndef TRIANGLEDB_H
#define TRIANGLEDB_H

#include "../../MasterThesis/source/Server/vec3d.h"
#include <vector>
#include <string>
#include <utility>
using std::pair;
using std::string;
using std::vector;

typedef pair<double, double> minMaxType;

class TriangleDB
{
    void generateNormals();

public:
    vector<Vec3D> vertices;
    vector<Vec3D> normals;
    vector<uint32_t> triangleIndexes;

    TriangleDB();
    TriangleDB(string filename);

    minMaxType minMaxX() const;
    minMaxType minMaxY() const;
    minMaxType minMaxZ() const;
    Vec3D getCenter() const;
};

#endif // TRIANGLEDB_H
