#ifndef MATH_HH
#define MATH_HH

#include <complex> 
#include <SFML/System.hpp>

namespace math {

#define PI 3.1415926535897932384626433832795


float hanning_filter(float val);


template <typename ValueType>
float magnitude(std::complex<ValueType> const& value, bool logarithm = false) {
	if (!logarithm) {
		return sqrt(pow(value.real(), 2) + pow(value.imag(), 2));
	} else {
		return log(sqrt(pow(value.real(), 2) + pow(value.imag(), 2))) * 10;
	}
}


template <typename ValueType>
float magnitude(sf::Vector2<ValueType> const& vec) {
	return sqrt(pow(vec.x, 2) + pow(vec.y, 2));
}


} // namespace math

#endif
