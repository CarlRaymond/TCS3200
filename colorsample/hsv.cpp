#include "hsv.h";
#include "math.h";

HSV::HSV(int h, unsigned int s, unsigned int v) {
	this->h = h;
	this->s = s;
	this->v = v;
}

HSV::HSV() : HSV(0, 0, 0) {
}

void HSV::fromScaledRGB(unsigned int scale, unsigned int r, unsigned int g, unsigned int b, HSV &out) {

	// scale is the maximum range of r, g, b.

	// Find min and max of r,g,b
	unsigned int min = r;
	unsigned int max = r;
	if (g > max) { max = g; }
	if (g < min) { min = g; }
	if (b > max) { max = b; }
	if (b < min) { min = b; }

	unsigned int delta = max - min;

	int h = 0;
	int s = 0;
	int v = 0;

	v = (250L * max) / scale ;

	if (max == 0) {
		s = 0;
		h = -1;
	}
	else {
		s = (250L * delta) / max;
		if (r == max) {
			// Yellow to magenta. H = -60 to 60
			h = 60 * (g - b) / delta;
		}
		else if (g == max) {
			// Cyan to yellow. H = 60 to 180
			h = 120 + (60 * (b - r) / delta);
		}
		else {
			// Magenta to cyan. H = 180 to 300
			h = 240 + (60 * (r - g) / delta);
		}

		if (h < 0) {
			h += 360;
		}
	}

	out.h = h;
	out.s = s;
	out.v = v;
	return;
	
}