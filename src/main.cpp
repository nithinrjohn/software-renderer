#include <iostream>

#include "geometry.h"

#include "image.h"
#include "model.h"

using namespace std;

const Color black = {0, 0, 0};
const Color lightBlue = {66, 135, 245};
const Color lightred = {245, 66, 135};
const Color lightgreen = {135, 245, 66};

const int WIDTH = 512;
const int HEIGHT = 512;

int main() {

	Image image(WIDTH, HEIGHT, black);

	vector<Vec2i> triangle1 = { {10, 70}, {50, 160}, {70, 80} };
	vector<Vec2i> triangle2 = { {180, 50}, {150, 1}, {70, 180} };
	vector<Vec2i> triangle3 = { {180, 150}, {120, 160}, {130, 180} };

	image.drawTriangle(triangle1, lightBlue);
	image.drawTriangle(triangle2, lightred);
	image.drawTriangle(triangle3, lightgreen);

	Model model("../resources/models/suzanne.obj", image, lightBlue);

	cout << "nverts: " << model.nverts() << endl;
	cout << "nfaces: " << model.nfaces() << endl;

	image.write("render.bmp");

	return 0;
}