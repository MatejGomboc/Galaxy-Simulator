/*
	Copyright (C) 2019 Matej Gomboc https://github.com/MatejGomboc/Galaxy-Simulator

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Affero General Public License as published
	by the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Affero General Public License for more details.
	You should have received a copy of the GNU Affero General Public License
	along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "stars.h"
#include <random>
#include <stdexcept>

#include <iostream>


Stars::Stars(GLulong num) :
	m_initialised(false),
	m_num(num),
	m_vbo(0),
	m_vel(nullptr),
	m_ocl_context(nullptr),
	m_ocl_cmd_queue(nullptr),
	m_ocl_kernel(nullptr)
{
}


Stars::~Stars()
{
	if (m_initialised)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glDeleteBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		m_vbo = 0;

		clReleaseKernel(m_ocl_kernel);
		m_ocl_kernel = nullptr;
		clReleaseContext(m_ocl_context);
		m_ocl_context = nullptr;
		clReleaseCommandQueue(m_ocl_cmd_queue);
		m_ocl_cmd_queue = nullptr;

		m_initialised = false;
	}
}


void Stars::init()
{
	if (!m_initialised)
	{
		std::random_device gen;
		std::uniform_real_distribution<GLfloat> distrib(-1.0f, 1.0f);

		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, m_num * 3 * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);
		
		Vector3D* pos = reinterpret_cast<Vector3D*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

		for (GLsizei i = 0; i < m_num; i++)
		{
			pos[i].x = distrib(gen);
			pos[i].y = distrib(gen);
			pos[i].z = distrib(gen);
		}

		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		m_vel = std::make_unique<Vector3D[]>(m_num);

		for (GLsizei i = 0; i < m_num; i++)
		{
			m_vel[i].x = distrib(gen);
			m_vel[i].y = distrib(gen);
			m_vel[i].z = distrib(gen);
		}

		std::cout << glGetString(GL_RENDERER) << std::endl;

		/*cl_context_properties ocl_properties[] =
		{
			CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
			CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
			0
		};*/

		//cl_device_id ocl_device = nullptr;
		//clGetGLContextInfoKHR(ocl_properties, CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR, sizeof(cl_device_id), &ocl_device, nullptr);
		//m_ocl_context = clCreateContext(nullptr, 1, &ocl_device, nullptr, nullptr, nullptr);

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
