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


#include "stars.h"
#include <random>
#include <stdexcept>


Stars::Stars(GLulong num) :
	m_initialised(false),
	m_num(num),
	m_vbo(0),
	m_vel(std::make_unique<Vector3D[]>(m_num)),
	m_old_pos(std::make_unique<Vector3D[]>(m_num)),
	m_old_vel(std::make_unique<Vector3D[]>(m_num)),
	m_ocl_context(nullptr),
	m_ocl_cmd_queue(nullptr),
	m_ocl_kernel(nullptr)
{
}


void Stars::release()
{
	if (m_initialised)
	{
		if (m_vbo != 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			glDeleteBuffers(1, &m_vbo);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			m_vbo = 0;
		}

		if (m_ocl_kernel != nullptr)
		{
			clReleaseKernel(m_ocl_kernel);
			m_ocl_kernel = nullptr;
		}

		if (m_ocl_context != nullptr)
		{
			// TODO !! Why causes memory access violation ??
			//clReleaseContext(m_ocl_context);
			m_ocl_context = nullptr;
		}

		if (m_ocl_cmd_queue != nullptr)
		{
			// TODO !! Why causes memory access violation ??
			//clReleaseCommandQueue(m_ocl_cmd_queue);
			m_ocl_cmd_queue = nullptr;
		}

		m_initialised = false;
	}
}


Stars::~Stars()
{
	release();
}


void Stars::init()
{
	if (!m_initialised)
	{
		std::random_device gen;
		std::uniform_real_distribution<GLfloat> distrib(-1.0f, 1.0f);

		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, m_num * sizeof(Vector3D), nullptr, GL_STATIC_DRAW);
		
		auto pos = reinterpret_cast<Vector3D*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

		for (GLsizei i = 0; i < m_num; i++)
		{
			pos[i].x = distrib(gen);
			pos[i].y = distrib(gen);
			pos[i].z = distrib(gen);
		}

		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		for (GLsizei i = 0; i < m_num; i++)
		{
			m_vel[i].x = distrib(gen);
			m_vel[i].y = distrib(gen);
			m_vel[i].z = distrib(gen);
		}

		//////////////////////////////////////////////////////

		cl_uint ocl_num_platforms;
		clGetPlatformIDs(0, nullptr, &ocl_num_platforms);
		if (ocl_num_platforms == 0)
		{
			release();
			throw std::exception("No OpenCL platforms found.");
		}

		auto ocl_platforms = std::make_unique<cl_platform_id[]>(ocl_num_platforms);
		clGetPlatformIDs(ocl_num_platforms, ocl_platforms.get(), nullptr);

		for (cl_uint i = 0; i < ocl_num_platforms + 1; i++)
		{
			if (i == ocl_num_platforms)
			{
				release();
				throw std::exception("Cannot initialise OpenCL.");
			}
			else
			{
				cl_context_properties ocl_context_properties[] =
				{
					CL_GL_CONTEXT_KHR, reinterpret_cast<cl_context_properties>(wglGetCurrentContext()),
					CL_WGL_HDC_KHR, reinterpret_cast<cl_context_properties>(wglGetCurrentDC()),
					CL_CONTEXT_PLATFORM, reinterpret_cast<cl_context_properties>(ocl_platforms[i]),
					0
				};

				cl_int ocl_err;
				m_ocl_context = clCreateContextFromType(ocl_context_properties, CL_DEVICE_TYPE_GPU, nullptr, nullptr, &ocl_err);
				if (ocl_err != CL_SUCCESS) continue;

				size_t ocl_devices_size;
				clGetContextInfo(m_ocl_context, CL_CONTEXT_DEVICES, 0, nullptr, &ocl_devices_size);
				size_t ocl_num_devices = ocl_devices_size / sizeof(cl_device_id);
				if (ocl_num_devices != 1) continue;

				cl_device_id ocl_device;
				clGetContextInfo(m_ocl_context, CL_CONTEXT_DEVICES, sizeof(cl_device_id), &ocl_device, nullptr);

				m_ocl_cmd_queue = clCreateCommandQueue(m_ocl_context, ocl_device, 0, &ocl_err);
				if (ocl_err != CL_SUCCESS) continue;

				break;
			}
		}

		m_initialised = true;
	}
	else
	{
		throw std::exception("Already initialised.");
	}
}


void Stars::calculate()
{
	if (m_initialised)
	{
		// TODO !!
	}
	else
	{
		throw std::exception("Not initialised.");
	}
}


void Stars::draw()
{
	if (m_initialised)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glVertexPointer(3, GL_FLOAT, 0, nullptr);
		glEnableClientState(GL_VERTEX_ARRAY);
		glColor3f(1.0f, 1.0f, 0.0f);
		glDrawArrays(GL_POINTS, 0, m_num);
		glDisableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else
	{
		throw std::exception("Not initialised.");
	}
}
