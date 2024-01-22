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

Model::Model(string filename, Image &image, Color color, string algorithm) {
    open(filename);
    draw(image, color, algorithm);
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
        else if(type == "f") {
            vector<int> f;
            char slash;
            int vertex, textureCoord, normal;
            while(ss >> vertex >> slash >> textureCoord >> slash >> normal) {
                f.push_back(vertex - 1); // -1 because obj files start at 1
            }
            
            // for(int i = 0; i < f.size(); i++) {
            //     cout << f[i] + 1 << " ";
            // }
            // cout << endl;

            this->faces.push_back(f);
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

bool Model::draw(Image &image, Color color, string algorithm) {
    int width = image.getWidth();
    int height = image.getHeight();
    
    for(int i = 0; i < this->nfaces(); i++) {
        vector<int> face = this->face(i);

        if(face.size() != 3) {
            cout << "Error: Face " << i << " does not have 3 vertices." << endl;
            cout << "Face " << i << " has " << face.size() << " vertices." << endl;
            return false;
        }
        for(int j = 0; j < 3; j++) {
            if(face[j] >= this->nverts()) {
                cout << "Error: Vertex index " << face[j] << " is out of bounds." << endl;
                return false;
            }
            Vec3f v0 = this->vert(face[j]); //triangulate mesh or this becomes segfault
            Vec3f v1 = this->vert(face[(j + 1) % 3]);

            Vec2i pixel0 = WorldToScreenSpace(v0, width, height);
            Vec2i pixel1 = WorldToScreenSpace(v1, width, height);
            
            image.drawLine(pixel0.x, pixel0.y, pixel1.x, pixel1.y, color, algorithm);
        }
        //cout << "Completed face " << i + 1 << endl;
    }

    return true;
}