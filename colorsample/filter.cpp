// Represents a first-order filter, with coefficients supplied in the constructor.

#include "filter.h";

Filter::Filter (FilterType type, float gain, float y0_coeff) {
		this->type = type;
		this->gain = gain;
		this->gain_recip = 1.0 / gain;
		this->y0_coeff = y0_coeff;
	};

int Filter::next(int sample) {

		x0 = x1;
		x1 = sample * gain_recip;

		y0 = y1;

		switch (this->type) {

			case LOWPASS:
				y1 = x0 + x1 + (int)(y0 * y0_coeff);
				break;

			case HIGHPASS:
				y1 = x0 - x1 + (int)(y0 * y0_coeff);
				break;
		}

		return y1;
	};
