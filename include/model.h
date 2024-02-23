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

        vector<Vec2f> vertTextures;
        vector<vector<int>> textureCoords;
    public:
        Model(string filename);
        Model(string filename, Image &image, Color color, Image& texture, bool fill = true, bool wireframe = false, string algorithm = "bresenham");
        ~Model();

        bool open(string filename);

        int nverts();
        int nfaces();

        Vec3f vert(int i);
        vector<int> face(int i);

        Vec2f vertTexture(int i);
        vector<int> textureCoord(int i);

        bool draw(Image &image, Color color, Image& texture, bool fill = true, bool wireframe = false, string algorithm = "bresenham");
};