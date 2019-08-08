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

float wrap(float value, const float min, const float max)
{
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

kernel void propagate(global float4* pos, global float4* vel, global float4* old_pos, global float4* old_vel)
{
	unsigned long long i = get_global_id(0);

	old_pos[i] = pos[i];
	old_vel[i] = vel[i];

	barrier(CLK_GLOBAL_MEM_FENCE);

	float pos_x = wrap(old_vel[i].x + old_pos[i].x, -1.0f, 1.0f);
	float pos_y = wrap(old_vel[i].y + old_pos[i].y, -1.0f, 1.0f);
	float pos_z = wrap(old_vel[i].z + old_pos[i].z, -1.0f, 1.0f);

	pos[i] = (float4)(pos_x, pos_y, pos_z, 1.0f);
}
