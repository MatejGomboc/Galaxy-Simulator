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


float clamp(float value, const float min, const float max)
{
	if (value > max) return max;
	else if (value < min) return min;
	else return value;
}


kernel void propagate(global read_only float* old_pos, global read_only float* old_vel,
	global write_only float* pos, global write_only float* vel,
	const float min_pos, const float max_pos)
{
	long long i = get_global_id(0);

	old_pos[i] = pos[i];
	old_vel[i] = vel[i];

	barrier(CLK_GLOBAL_MEM_FENCE);

	pos[i] = clamp(vel[i] + old_pos[i], -1.0f, 1.0f);
}