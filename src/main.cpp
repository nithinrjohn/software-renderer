#include <iostream>

#include "geometry.h"

#include "image.h"
#include "model.h"

#include "colors.h"

using namespace std;

const int WIDTH = 800;
const int HEIGHT = 800;

string name = "";

int main() {

	cout << "Enter the name of the model: ";
	cin >> name;

	Image texture("../resources/textures/" + name + ".bmp");

	Image image(WIDTH, HEIGHT, BackgroundColor1);

	Model model("../resources/models/" + name + ".obj", image, black, texture);

	cout << "nverts: " << model.nverts() << endl;
	cout << "nfaces: " << model.nfaces() << endl;

	image.write("render.bmp");

	return 0;
}