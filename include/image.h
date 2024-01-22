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
};
#pragma pack(pop)

class Image {
    private:
        int width;
        int height;

    	BitmapFileHeader fileHeader;
	    BitmapInfoHeader infoHeader;
        vector<vector<Color>> pixels;

		void drawLineLow(int x0, int y0, int x1, int y1, Color color);;
		void drawLineHigh(int x0, int y0, int x1, int y1, Color color);
		void drawLineBresenham(int x0, int y0, int x1, int y1, Color color);
		void drawLineSimple(int x0, int y0, int x1, int y1, Color color);
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
		void set(int x, int y, Color color);
		void drawLine(int x0, int y0, int x1, int y1, Color color, string algorithm="bresenham");
		void drawTriangle(Vec2i v0, Vec2i v1, Vec2i v2, Color color);
		void drawTriangle(vector<Vec2i>, Color color);
};