class HSV {

public:
	// Hue: 0 - 360, or -1 for black.
	int h;

	// Saturation: 0 - 100
  unsigned int s;

  // Value: 0 - 100
  unsigned int v;

  HSV(int h, unsigned int s, unsigned int v);
  HSV();
  static void fromScaledRGB(unsigned int scale, unsigned int r, unsigned int g, unsigned int b, HSV &out);
};
