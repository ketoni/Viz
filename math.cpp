
#include "math.hpp"


float math::hanning_filter(float val) {
	float a = 0.54, b = 0.46;
	if (val == 1.0) {
		return a;
	}
	return a - b * cos((2*PI)/(val-1));
}

