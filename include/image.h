#pragma once

#include <iostream>
#include <cstdint>
#include <fstream>
#include <vector>

#include "geometry.h"

using namespace std;

#pragma pack(push, 1)
struct BitmapFileHeader {
	char signature[2] = {'B', 'M'};
	uint32_t fileSize;
	uint32_t reserved = 0;
	uint32_t dataOffset = 54;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct BitmapInfoHeader {
	uint32_t headerSize = 40;
	int32_t width;
	int32_t height;
	uint16_t planes = 1;
	uint16_t bitsPerPixel = 24;
	uint32_t compression = 0;
	uint32_t dataSize = 0;
	int32_t horizontalResolution = 3780; //3780 ppm = 96 dpi
	int32_t verticalResolution = 3780; //3780 ppm = 96 dpi
	uint32_t colorsInColorTable = 0;
	uint32_t importantColors = 0;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Color {
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;

	Color operator*(const float& f) const {
		return Color{(uint8_t) (red * f), (uint8_t) (green * f), (uint8_t) (blue * f)};
	}
};
#pragma pack(pop)

class Image {
    private:
        int width;
        int height;

    	BitmapFileHeader fileHeader;
	    BitmapInfoHeader infoHeader;
        vector<vector<Color>> pixels;

		void drawLineLow(vector<Vec2i> points, Color color);
		void drawLineHigh(vector<Vec2i> points, Color color);
		void drawLineBresenham(vector<Vec2i> points, Color color);
		void drawLineSimple(vector<Vec2i> points, Color color);

		vector<Vec2i> boundingBox(Color color, vector<Vec3f> tri);
		void rasterizeTriangle(Color color, vector<Vec3f> tri, vector<vector<float>>& zbuffer);
    public:
        Image(string filename);
        Image(int width, int height);
		Image(int width, int height, Color color);
        ~Image();
		
		bool open(string filename);
        bool write(string filename);

		int getWidth();
		int getHeight();

		void clear(Color color);
		void set(int y, int x, Color color);
		void flip(bool x, bool y);

		void drawLine(vector<Vec2i> points, Color color, string algorithm="bresenham");
		void drawTriangle(vector<Vec3f>, Color color, vector<vector<float>>& zbuffer, bool fill = true, bool wireframe=false);
};