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
	const float step = 0.01f;
	const float mass = 0.00009f;

	unsigned long long i = get_global_id(0);

	pos[i] = pos[i] + (float4)(step * vel[i].xyz, 0.0f);

	if (length(pos[i].xyz) > 1.0f)
	{
		float3 pos_norm = normalize(pos[i].xyz);
		pos[i] = (float4)(2.0f * pos_norm - pos[i].xyz, 1.0f);
		vel[i] = (float4)(vel[i].xyz - dot(pos_norm, vel[i].xyz) * pos_norm, 0.0f);
	}

	barrier(CLK_GLOBAL_MEM_FENCE);

	for (unsigned long long j = i + 1; j < get_global_size(0); j++)
	{
		float r = distance(pos[i], pos[j]);

		if (r > 0.05f)
		{
			acc_matr[indx(i, j)] = mass / r / r / r;
		}
		else
		{
			acc_matr[indx(i, j)] = -0.5f;
		}
	}

	barrier(CLK_GLOBAL_MEM_FENCE);

	float4 acc = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	for (unsigned long long j = 0; j < i; j++)
	{
		acc += acc_matr[indx(j, i)] * (pos[j] - pos[i]);
	}
	for (unsigned long long j = i + 1; j < get_global_size(0); j++)
	{
		acc += acc_matr[indx(i, j)] * (pos[j] - pos[i]);
	}

	vel[i] = vel[i] + step * acc;

	if (length(vel[i]) > 1.0f)
		vel[i] = normalize(vel[i]);
}
