#pragma once

#include <fstream>
#include <vector>

#include "geometry.h"
#include "image.h"

using namespace std;

class Model {
    private:
        vector<Vec3f> verts;
        vector<vector<int>> faces;
    public:
        Model(string filename);
        Model(string filename, Image &image, Color color, bool fill = true, bool wireframe = false, string algorithm = "bresenham");
        ~Model();

        bool open(string filename);

        int nverts();
        int nfaces();

        Vec3f vert(int i);
        vector<int> face(int i);

        bool draw(Image &image, Color color, bool fill = true, bool wireframe = false, string algorithm = "bresenham");
};