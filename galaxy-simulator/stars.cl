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


unsigned long long indx(unsigned long long i, unsigned long long j)
{
	return (j * (j - 1) / 2 + i);
}


kernel void propagate(global float4* pos, global float4* vel, global float* acc_matr)
{
	const float step = 0.02f;
	const float mass = 0.5f;

	unsigned long long i = get_global_id(0);

	pos[i] = pos[i] + (float4)(step * vel[i].xyz, 0.0f);

	if (pos[i].x > 1.0f)
	{
		pos[i] = (float4)(2.0f - pos[i].x, pos[i].yz, 1.0f);
		vel[i] = (float4)(-vel[i].x, vel[i].yz, 0.0f);
	}

	if (pos[i].x < -1.0f)
	{
		pos[i] = (float4)(-2.0f - pos[i].x, pos[i].yz, 1.0f);
		vel[i] = (float4)(-vel[i].x, vel[i].yz, 0.0f);
	}

	if (pos[i].y > 1.0f)
	{
		pos[i] = (float4)(pos[i].x, 2.0f - pos[i].y, pos[i].z, 1.0f);
		vel[i] = (float4)(pos[i].x, -vel[i].y, vel[i].z, 0.0f);
	}

	if (pos[i].y < -1.0f)
	{
		pos[i] = (float4)(pos[i].x, -2.0f - pos[i].y, pos[i].z, 1.0f);
		vel[i] = (float4)(pos[i].x, -vel[i].y, vel[i].z, 0.0f);
	}

	if (pos[i].z > 1.0f)
	{
		pos[i] = (float4)(pos[i].xy, 2.0f - pos[i].z, 1.0f);
		vel[i] = (float4)(pos[i].xy, -vel[i].z, 0.0f);
	}

	if (pos[i].z < -1.0f)
	{
		pos[i] = (float4)(pos[i].xy, -2.0f - pos[i].z, 1.0f);
		vel[i] = (float4)(pos[i].xy, -vel[i].z, 0.0f);
	}

	barrier(CLK_GLOBAL_MEM_FENCE);

	for (unsigned long long j = i + 1; j < get_global_size(0); j++)
	{
		float r = distance(pos[i], pos[j]);

		if (r * r > mass)
		{
			acc_matr[indx(i, j)] = mass / r / r / r;
		}
		else
		{
			acc_matr[indx(i, j)] = 0.0f;
		}
	}

	barrier(CLK_GLOBAL_MEM_FENCE);

	float4 acc = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	for (unsigned long long j = 0; j < i; j++)
	{
		if (acc_matr[indx(j, i)] > 0.0f) acc += acc_matr[indx(j, i)] * (pos[i] - pos[j]);
	}
	for (unsigned long long j = i + 1; j < get_global_size(0); j++)
	{
		if (acc_matr[indx(i, j)] > 0.0f) acc += acc_matr[indx(i, j)] * (pos[i] - pos[j]);
	}

	vel[i] = vel[i] + step * acc;

	vel[i] = (float4)(clamp(vel[i].x, -1.0f, 1.0f), clamp(vel[i].y, -1.0f, 1.0f), clamp(vel[i].z, -1.0f, 1.0f), 0.0f);
}
