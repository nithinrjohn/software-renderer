#include <iostream>

#include "geometry.h"

#include "image.h"
#include "model.h"

#include "colors.h"

using namespace std;

const int WIDTH = 800;
const int HEIGHT = 800;

int main() {

	Image image(WIDTH, HEIGHT, BackgroundColor2);

	Model model("../resources/models/link.obj", image, ZeldaGreen, true, false);

	cout << "nverts: " << model.nverts() << endl;
	cout << "nfaces: " << model.nfaces() << endl;

	image.write("render.bmp");

	return 0;
}