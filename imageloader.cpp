#include <assert.h>
#include <vector>
#include <windows.h>
#include "imageloader.h"
#include <cstdio>
#pragma warning(disable:4996)

using namespace std;

void printLog(const char *msg, ...) {
	va_list argp;
	va_start(argp, msg);
	FILE* fp = fopen("logout.txt", "a+");
	vfprintf(fp, msg, argp);
	fprintf(fp, "\n");
	fclose(fp);
	va_end(argp);
}

Image::Image(char* ps, int w, int h) : pixels(ps), width(w), height(h) {

}

Image::~Image() {
	delete[] pixels;
}

vector<char> ToPixels(HBITMAP BitmapHandle, int& width, int& height) {
	BITMAP Bmp = { 0 };
	BITMAPINFO Info = { 0 };
	vector<char> Pixels = vector<char>();
	const int newpixel = 24;

	HDC DC = CreateCompatibleDC(NULL);
	std::memset(&Info, 0, sizeof(BITMAPINFO)); //not necessary really..
	HBITMAP OldBitmap = (HBITMAP)SelectObject(DC, BitmapHandle);
	GetObject(BitmapHandle, sizeof(Bmp), &Bmp);

	Info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	Info.bmiHeader.biWidth = width = Bmp.bmWidth;
	Info.bmiHeader.biHeight = height = Bmp.bmHeight;
	Info.bmiHeader.biPlanes = 1;
	Info.bmiHeader.biBitCount = newpixel;
	Info.bmiHeader.biCompression = BI_RGB;
	Info.bmiHeader.biSizeImage = ((width * newpixel + 31) / 32) * 4 * height;


	Pixels.resize(Info.bmiHeader.biSizeImage);
	GetDIBits(DC, BitmapHandle, 0, height, &Pixels[0], &Info, DIB_RGB_COLORS);
	SelectObject(DC, OldBitmap);

	height = std::abs(height);
	DeleteDC(DC);
	return Pixels;
}

Image* loadBMP(int resid) {
	HBITMAP hbmp0 = LoadBitmap(GetModuleHandle(0), MAKEINTRESOURCE(resid));
	int width = 0, height = 0;
	vector<char> pixels = ToPixels(hbmp0, width, height); 
	printLog("hbmp0_null=%d,width=%d,height=%d,pixels_size=%d",(hbmp0==NULL),width,height,pixels.size());
	//Read the data
	int bytesPerRow = ((width * 3 + 3) / 4) * 4 - (width * 3 % 4);
	int size = bytesPerRow * height;
	//Get the data into the right format
	char* pixels2 = new char[width * height * 3];
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++) {
			for(int c = 0; c < 3; c++) {
				pixels2[3 * (width * y + x) + c] =
					pixels[bytesPerRow * y + 3 * x + (2 - c)];
			}
		}
	}
	return new Image(pixels2, width, height);
}
