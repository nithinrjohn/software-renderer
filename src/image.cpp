#include "image.h"

using namespace std;

Image::Image(string filename) {
    this->open(filename);
}

Image::Image(int width, int height) {
    this->width = width;
    this->height = height;

    this->infoHeader.width = width;
	this->infoHeader.height = height;

    unsigned char padding[3] = {0, 0, 0};
	int paddingSize = (4 - (width * 3) % 4) % 4;
	int stride = (width * 3) + paddingSize;

	this->fileHeader.fileSize = this->fileHeader.dataOffset + (stride * height);

    this->pixels = vector<vector<Color>>(height, vector<Color>(width));
}

Image::Image(int width, int height, Color color) {
    this->width = width;
    this->height = height;

    this->infoHeader.width = width;
	this->infoHeader.height = height;

    unsigned char padding[3] = {0, 0, 0};
	int paddingSize = (4 - (width * 3) % 4) % 4;
	int stride = (width * 3) + paddingSize;

	this->fileHeader.fileSize = this->fileHeader.dataOffset + (stride * height);

    this->pixels = vector<vector<Color>>(height, vector<Color>(width, color));
}

Image::~Image() {}

bool Image::open(string filename) {
    ifstream file;
    file.open(filename, ios::binary);
    if(!file.is_open()) {
        return false;
    }

    file.read((char *) &this->fileHeader, 14);
    file.read((char *) &this->infoHeader, 40);

    this->width = this->infoHeader.width;
    this->height = this->infoHeader.height;

    unsigned char padding[3] = {0, 0, 0};
    int paddingSize = (4 - (this->width * 3) % 4) % 4;
    int stride = (this->width * 3) + paddingSize;

    this->pixels = vector<vector<Color>>(this->height, vector<Color>(this->width));

    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            //converts from BGR to RGB due to little endian
            file.read((char *) &this->pixels[y][x].blue, 1);
            file.read((char *) &this->pixels[y][x].green, 1);
            file.read((char *) &this->pixels[y][x].red, 1);
        }
        file.read((char *) padding, paddingSize);
    }

    file.close();

    return true;
}

bool Image::write(string filename) {
    ofstream file;
    file.open(filename, ios::binary);
    if(!file.is_open()) {
        return false;
    }

    file.write((char *) &this->fileHeader, 14);
    file.write((char *) &this->infoHeader, 40);

    unsigned char padding[3] = {0, 0, 0};
    int paddingSize = (4 - (this->width * 3) % 4) % 4;
    int stride = (this->width * 3) + paddingSize;

    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            //converts from RGB to BGR due to little endian
            file.write((char *) &this->pixels[y][x].blue, 1);
            file.write((char *) &this->pixels[y][x].green, 1);
            file.write((char *) &this->pixels[y][x].red, 1);
        }
        file.write((char *) padding, paddingSize);
    }

    file.close();

    return true;
}

int Image::getWidth() {
	return this->width;
}

int Image::getHeight() {
	return this->height;
}

void Image::clear(Color color) {
	unsigned char padding[3] = {0, 0, 0};
	int paddingSize = (4 - (this->width * 3) % 4) % 4;
	int stride = (this->width * 3) + paddingSize;

	this->fileHeader.fileSize = this->fileHeader.dataOffset + (stride * this->height);

	this->pixels = vector<vector<Color>>(this->height, vector<Color>(this->width, color));
}

void Image::set(int y, int x, Color color) {
    this->pixels[y][x] = color;
}

void Image::drawLineLow(int x0, int y0, int x1, int y1, Color color) {
	int dx = x1 - x0;
	int dy = y1 - y0;

	int yi = 1;
	if(dy < 0) {
		yi = -1;
		dy = -dy;
	}

	int D = 2*dy - dx;
	int y = y0;

	for(int x = x0; x <= x1; x++) {
		set(y, x, color);
		if(D > 0) {
			y += yi;
			D = D + (2 * (dy - dx));
		}
		else {
			D = D + 2 * dy;
		}
	}
}

void Image::drawLineHigh(int x0, int y0, int x1, int y1, Color color) {
	int dx = x1 - x0;
	int dy = y1 - y0;

	int xi = 1;
	if(dx < 0) {
		xi = -1;
		dx = -dx;
	}

	int D = 2*dx - dy;
	int x = x0;

	for(int y = y0; y <= y1; y++) {
		set(y, x, color);
		if(D > 0) {
			x += xi;
			D = D + (2 * (dx - dy));
		}
		else {
			D = D + 2 * dx;
		}
	}
}

void Image::drawLineBresenham(int x0, int y0, int x1, int y1, Color color) {
	int dx = x1 - x0;
	int dy = y1 - y0;

	if(abs(dx) > abs(dy)) {
		//slope is less than 1
		if(x0 > x1) {
			drawLineLow(x1, y1, x0, y0, color);
		}
		else {
			drawLineLow(x0, y0, x1, y1, color);
		}
	}
	else {
		//slope is greater than 1
		if(y0 > y1) {
			drawLineHigh(x1, y1, x0, y0, color);
		}
		else {
			drawLineHigh(x0, y0, x1, y1, color);
		}
	}
}

void Image::drawLineSimple(int x0, int y0, int x1, int y1, Color color) {
	int dx = x1 - x0;
	int dy = y1 - y0;

	if(dy == 0) {
		// Horizontal line
		for(int x = x0; x <= x1; x++) {
			set(y0, x, color);
		}
	}
	else if(abs(dx) > abs(dy)) {
		//slope is less than 1
		float m = (float) dy / dx;
		float b = y0 - m * x0;

		for(int x = x0; x <= x1; x++) {
			int y = m * x + b;
			// cout << "x: " << x << " y: " << y << endl;
			// if(x < 0 || x > width)
			// {
			// 	cout << "x out of bounds" << endl;
			// 	cout << "slope: less than one" << m << endl;
			// }
			// if(y < 0 || y > height)
			// {
			// 	cout << "y out of bounds" << endl;
			// 	cout << "slope: less than one" << m << endl;
			// }
			set(y, x, color);
		}
	} 
	else {
		//slope is greater than 1
		float m = (float) dx / dy;
		float b = x0 - m * y0;

		for(int y = y0; y <= y1; y++) {
			int x = m * y + b;
			// cout << "x: " << x << " y: " << y << endl;
			// if(x < 0 || x > width)
			// {
			// 	cout << "x out of bounds" << endl;
			// 	cout << "slope: greater than one" << m << endl;
			// }
			// if(y < 0 || y > height)
			// {
			// 	cout << "y out of bounds" << endl;
			// 	cout << "slope: greater than one" << m << endl;
			// }
			set(y, x, color);
		}
	}
}

void Image::drawLine(int x0, int y0, int x1, int y1, Color color, string algorithm) {
	if(algorithm == "bresenham") {
		drawLineBresenham(x0, y0, x1, y1, color);
	}
	else if(algorithm == "simple") {
		drawLineSimple(x0, y0, x1, y1, color);
	}
}

void Image::drawTriangle(Vec2i v0, Vec2i v1, Vec2i v2, Color color) {
	drawLine(v0.x, v0.y, v1.x, v1.y, color);
	drawLine(v1.x, v1.y, v2.x, v2.y, color);
	drawLine(v2.x, v2.y, v0.x, v0.y, color);
}

void Image::drawTriangle(vector<Vec2i> vertices, Color color) {
	drawLine(vertices[0].x, vertices[0].y, vertices[1].x, vertices[1].y, color);
	drawLine(vertices[1].x, vertices[1].y, vertices[2].x, vertices[2].y, color);
	drawLine(vertices[2].x, vertices[2].y, vertices[0].x, vertices[0].y, color);
}