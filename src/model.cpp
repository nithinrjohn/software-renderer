#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include "model.h"

using namespace std;

Model::Model(string filename) {
    open(filename);
}

Model::Model(string filename, Image &image, Color color, Image& texture, bool fill, bool wireframe, string algorithm) {
    open(filename);
    draw(image, color, texture, fill, wireframe, algorithm);
}

Model::~Model() {}

bool Model::open(string filename) {
    ifstream file;
    file.open(filename);

    if(!file.is_open()) {
        return false;
    }

    string line;
    while(!file.eof()) {
        getline(file, line);
        stringstream ss(line);
        string type;
        ss >> type;

        if(type == "v") {
            Vec3f v;
            ss >> v.x >> v.y >> v.z;
            this->verts.push_back(v);
        }
        else if (type == "vt")
        {
            Vec2f vt;
            ss >> vt.x >> vt.y;
            this->vertTextures.push_back(vt);
        }
        else if(type == "f") {
            vector<int> f;
            vector<int> t;
            char slash;
            int vertex, textureCoord, normal;
            while(ss >> vertex >> slash >> textureCoord >> slash >> normal) {
                f.push_back(vertex - 1); // -1 because obj files start at 1
                t.push_back(textureCoord - 1);
            }
            
            // for(int i = 0; i < f.size(); i++) {
            //     cout << f[i] + 1 << " ";
            // }
            // cout << endl;

            this->faces.push_back(f);
            this->textureCoords.push_back(t);

            // cout << ss.str() << endl;
            // cout << "textureCoords: " << t[0] << " " << t[1] << " " << t[2] << endl;
        }
    }

    file.close();

    return true;
}

int Model::nverts() {
    return this->verts.size();
}

int Model::nfaces() {
    return this->faces.size();
}

Vec3f Model::vert(int i) {
    return this->verts[i];
}

vector<int> Model::face(int i) {
    return this->faces[i];
}

Vec2f Model::vertTexture(int i) {
    return this->vertTextures[i];
}

vector<int> Model::textureCoord(int i) {
    return this->textureCoords[i];
}

bool Model::draw(Image &image, Color color, Image& texture, bool fill, bool wireframe, string algorithm) {
    int width = image.getWidth();
    int height = image.getHeight();

    vector<vector<float>> zbuffer(width, vector<float>(height, numeric_limits<float>::max()));
    
    for(int i = 0; i < this->nfaces(); i++) {
        vector<int> face = this->face(i);
        vector<int> textureCoord = this->textureCoord(i);

        //cout << "Face " << i << ": " << face[0] << " " << face[1] << " " << face[2] << endl;
        //cout << "TextureCoord " << i << ": " << textureCoord[0] << " " << textureCoord[1] << " " << textureCoord[2] << endl;

        if(face.size() != 3) {
            cout << "Error: Face " << i << " does not have 3 vertices." << endl;
            cout << "Face " << i << " has " << face.size() << " vertices." << endl;
            return false;
        }
        Vec3f v0 = this->vert(face[0]); //triangulate mesh or this becomes segfault
        Vec3f v1 = this->vert(face[1]);
        Vec3f v2 = this->vert(face[2]);

        Vec3f pixel0 = WorldToScreenSpace(v0, width, height);
        Vec3f pixel1 = WorldToScreenSpace(v1, width, height);
        Vec3f pixel2 = WorldToScreenSpace(v2, width, height);

        vector<Vec3f> tri = {pixel0, pixel1, pixel2};

        Vec2f vt0 = this->vertTexture(textureCoord[0]); //obtain vertex textures
        Vec2f vt1 = this->vertTexture(textureCoord[1]);
        Vec2f vt2 = this->vertTexture(textureCoord[2]);

        vector<Vec2f> triTexture = {vt0, vt1, vt2};

        //cout << "TriTexture: " << triTexture[0].x << " " << triTexture[0].y << endl;

        //randomize color
        //Color randColor = {rand() % 255, rand() % 255, rand() % 255};
        //color = randColor;

        Vec3f light = {0, 0, -1};

        Vec3f normal =  cross(v1 - v0, v2 - v0);
        normal = normal / sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);

        float intensity =  dot(normal, light);

        if(intensity > 0) {
             image.drawTriangle(tri, intensity, zbuffer, triTexture, &texture, fill, wireframe);
        }

        //image.drawTriangle(tri, lightColor, zbuffer, false, true);


        // for(int j = 0; j < 3; j++) {
        //     if(face[j] >= this->nverts()) {
        //         cout << "Error: Vertex index " << face[j] << " is out of bounds." << endl;
        //         return false;
        //     }
        //     Vec3f v0 = this->vert(face[j]); //triangulate mesh or this becomes segfault
        //     Vec3f v1 = this->vert(face[(j + 1) % 3]);

        //     Vec2i pixel0 = WorldToScreenSpace(v0, width, height);
        //     Vec2i pixel1 = WorldToScreenSpace(v1, width, height);
            
        //     image.drawLine(pixel0.x, pixel0.y, pixel1.x, pixel1.y, color, algorithm);
        // }
        //cout << "Completed face " << i + 1 << endl;
    }

    return true;
}