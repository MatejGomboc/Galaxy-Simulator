/*
	Copyright (C) 2019 Matej Gomboc https://github.com/MatejGomboc/Galaxy-Simulator

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Affero General Public License as published
	by the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version. This program is distributed in the
	hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
	See the GNU Affero General Public License for more details. You should
	have received a copy of the GNU Affero General Public License along with
	this program. If not, see <https://www.gnu.org/licenses/>.
*/


#include "utils.h"
#include <stdexcept>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif


float Utils::wrap(float value, const float min, const float max)
{
	if (min > max) throw std::exception("MIN > MAX in wrap function.");
	if (min == max) return min;

	const float difference = max - min;

	while (value < min)
	{
		value += difference;
	}

	while (value > max)
	{
		value -= difference;
	}

	return value;
}


float Utils::clamp(float value, const float min, const float max)
{
	if (min > max) throw std::exception("MIN > MAX in clamp function.");
	if (min == max) return min;

	if (value > max) return max;
	else if (value < min) return min;
	else return value;
}


float Utils::deg(const float value_rad)
{
	return(value_rad * 180.0f / (float)M_PI);
}


float Utils::rad(const float value_deg)
{
	return(value_deg * (float)M_PI / 180.0f);
}
