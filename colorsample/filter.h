
#ifndef FILTER_H
#define FILTER_H

enum FilterType { LOWPASS, HIGHPASS };

class Filter {
private:
	float gain;
	float gain_recip;

	float y0_coeff;

	int x0 = 0;
	int x1 = 0;
	int y0 = 0;
	int y1 = 0;

	FilterType type;

public:	
	Filter(FilterType type, float gain, float y0_coeff);
	int next(int sample);
};

#endif