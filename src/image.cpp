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
	this->flip(false, true);

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

void Image::flip(bool x, bool y) {
	if(x) {
		for(int y = 0; y < this->height / 2; y++) {
			for(int x = 0; x < this->width; x++) {
				Color temp = this->pixels[y][x];
				this->pixels[y][x] = this->pixels[this->height - y - 1][x];
				this->pixels[this->height - y - 1][x] = temp;
			}
		}
	}
	if(y) {
		for(int y = 0; y < this->height; y++) {
			for(int x = 0; x < this->width / 2; x++) {
				Color temp = this->pixels[y][x];
				this->pixels[y][x] = this->pixels[y][this->width - x - 1];
				this->pixels[y][this->width - x - 1] = temp;
			}
		}
	}
}

void Image::drawLineLow(vector<Vec2i> points, Color color) {
	int x0 = points[0].x;
	int y0 = points[0].y;
	int x1 = points[1].x;
	int y1 = points[1].y;

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

void Image::drawLineHigh(vector<Vec2i> points, Color color) {
	int x0 = points[0].x;
	int y0 = points[0].y;
	int x1 = points[1].x;
	int y1 = points[1].y;

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

void Image::drawLineBresenham(vector<Vec2i> points, Color color) {
	int x0 = points[0].x;
	int y0 = points[0].y;
	int x1 = points[1].x;
	int y1 = points[1].y;

	int dx = x1 - x0;
	int dy = y1 - y0;

	if(abs(dx) > abs(dy)) {
		//slope is less than 1
		if(x0 > x1) {
			drawLineLow(points, color);
		}
		else {
			drawLineLow(points, color);
		}
	}
	else {
		//slope is greater than 1
		if(y0 > y1) {
			drawLineHigh(points, color);
		}
		else {
			drawLineHigh(points, color);
		}
	}
}

void Image::drawLineSimple(vector<Vec2i> points, Color color) {
	int x0 = points[0].x;
	int y0 = points[0].y;
	int x1 = points[1].x;
	int y1 = points[1].y;

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
			set(y, x, color);
		}
	} 
	else {
		//slope is greater than 1
		float m = (float) dx / dy;
		float b = x0 - m * y0;

		for(int y = y0; y <= y1; y++) {
			int x = m * y + b;
			set(y, x, color);
		}
	}
}

void Image::drawLine(vector<Vec2i> points, Color color, string algorithm) {
	if(algorithm == "bresenham") {
		drawLineBresenham(points, color);
	}
	else if(algorithm == "simple") {
		drawLineSimple(points, color);
	}
}

vector<Vec2i> Image::boundingBox(Color color, vector<Vec3f> tri) {
	int minX = min(tri[0].x, min(tri[1].x, tri[2].x));
	int minY = min(tri[0].y, min(tri[1].y, tri[2].y));
	int maxX = max(tri[0].x, max(tri[1].x, tri[2].x));
	int maxY = max(tri[0].y, max(tri[1].y, tri[2].y));

	vector<Vec2i> box = { {minX, minY}, {maxX, minY}, {maxX, maxY}, {minX, maxY} };

	return box;
}

void Image::rasterizeTriangle(Color color, vector<Vec3f> tri, vector<vector<float>>& zbuffer) {
	vector<Vec2i> box = boundingBox(color, tri);

	Vec3f p;
	for(p.y = box[0].y; p.y <= box[3].y; p.y++) {
		for(p.x = box[0].x; p.x <= box[1].x; p.x++) {
			Vec3f barycentric = barycentricCoords(tri, p);
			if(barycentric.x < 0 || barycentric.y < 0 || barycentric.z < 0) continue;
			p.z = 0;

			p.z = tri[0].z * barycentric.x + tri[1].z * barycentric.y + tri[2].z * barycentric.z;

			if(p.z < zbuffer[p.x][p.y]) {
				zbuffer[p.x][p.y] = p.z;
				//Color zColor = {static_cast<unsigned char>(p.z * 255), static_cast<unsigned char>(p.z * 255), static_cast<unsigned char>(p.z * 255)};
				set(p.y, p.x, color);
			}
		}
	}
}

void Image::drawTriangle(vector<Vec3f> vertices, Color color, vector<vector<float>>& zbuffer, bool fill, bool wireframe) {
	if(wireframe) {
		vector<Vec2i> line1({ {static_cast<int>(vertices[0].x), static_cast<int>(vertices[0].y)}, {static_cast<int>(vertices[1].x), static_cast<int>(vertices[1].y)} });
		vector<Vec2i> line2({ {static_cast<int>(vertices[1].x), static_cast<int>(vertices[1].y)}, {static_cast<int>(vertices[2].x), static_cast<int>(vertices[2].y)} });
		vector<Vec2i> line3({ {static_cast<int>(vertices[2].x), static_cast<int>(vertices[2].y)}, {static_cast<int>(vertices[0].x), static_cast<int>(vertices[0].y)} });

		drawLine(line1, color);
		drawLine(line2, color);
		drawLine(line3, color);
	}
	if(fill) {
		rasterizeTriangle(color, vertices, zbuffer);
	}
}