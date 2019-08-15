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
#include "stars_ocl.h"
#include <random>
#include <stdexcept>
#include <string>

#ifdef DEBUG
#include <fstream>
#endif


Stars::Stars(GLulong num) :
	m_initialised(false),
	m_num((num < 2) ? 2 : num),
	m_vbo(0),
	m_vel(std::make_unique<Vector4D[]>(m_num)),
	m_ocl_context(nullptr),
	m_ocl_cmd_queue(nullptr),
	m_ocl_kernel(nullptr),
	m_ocl_buffer_pos(nullptr),
	m_ocl_buffer_vel(nullptr),
	m_ocl_buffer_acc(nullptr)
{
}


void Stars::release()
{
	if (m_initialised)
	{
		if (m_ocl_cmd_queue != nullptr)
		{
			clFinish(m_ocl_cmd_queue);
			clReleaseCommandQueue(m_ocl_cmd_queue);
			m_ocl_cmd_queue = nullptr;
		}

		if (m_ocl_kernel != nullptr)
		{
			clReleaseKernel(m_ocl_kernel);
			m_ocl_kernel = nullptr;
		}

		if (m_ocl_context != nullptr)
		{
			clReleaseContext(m_ocl_context);
			m_ocl_context = nullptr;
		}

		if (m_ocl_buffer_pos != nullptr)
		{
			clReleaseMemObject(m_ocl_buffer_pos);
			m_ocl_buffer_pos = nullptr;
		}

		if (m_ocl_buffer_vel != nullptr)
		{
			clReleaseMemObject(m_ocl_buffer_vel);
			m_ocl_buffer_vel = nullptr;
		}

		if (m_ocl_buffer_acc != nullptr)
		{
			clReleaseMemObject(m_ocl_buffer_acc);
			m_ocl_buffer_acc = nullptr;
		}

		if (m_vbo != 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			glDeleteBuffers(1, &m_vbo);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			m_vbo = 0;
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
		std::uniform_real_distribution<GLfloat> distrib_pos(-0.1f, 0.1f);

		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, m_num * sizeof(Vector4D), nullptr, GL_STATIC_DRAW);
		
		auto pos = reinterpret_cast<Vector4D*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

		for (GLsizei i = 0; i < m_num; i++)
		{
			pos[i].x = distrib_pos(gen);
			pos[i].y = distrib_pos(gen);
			pos[i].z = distrib_pos(gen);
			pos[i].w = 1.0f;
		}

		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		for (GLsizei i = 0; i < m_num; i++)
		{
			m_vel[i].x = 0.0f;
			m_vel[i].y = 0.0f;
			m_vel[i].z = 0.0f;
			m_vel[i].w = 0.0f;
		}

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
				if (ocl_num_devices != 1)
				{
					clReleaseContext(m_ocl_context);
					continue;
				}

				cl_device_id ocl_device;
				clGetContextInfo(m_ocl_context, CL_CONTEXT_DEVICES, sizeof(cl_device_id), &ocl_device, nullptr);

				m_ocl_cmd_queue = clCreateCommandQueue(m_ocl_context, ocl_device, 0, &ocl_err);
				if (ocl_err != CL_SUCCESS)
				{
					clReleaseContext(m_ocl_context);
					continue;
				}

				cl_program ocl_program = clCreateProgramWithSource(m_ocl_context, 1, &ocl_src_stars, nullptr, &ocl_err);
				if (ocl_err != CL_SUCCESS)
				{
					clReleaseContext(m_ocl_context);
					clReleaseCommandQueue(m_ocl_cmd_queue);
					continue;
				}

				ocl_err = clBuildProgram(ocl_program, 1, &ocl_device, "", nullptr, nullptr);

#ifdef DEBUG
				size_t log_str_size;
				clGetProgramBuildInfo(ocl_program, ocl_device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &log_str_size);

				auto log_str = std::make_unique<char[]>(log_str_size);
				clGetProgramBuildInfo(ocl_program, ocl_device, CL_PROGRAM_BUILD_LOG, log_str_size, log_str.get(), nullptr);

				std::ofstream log_file("ocl_build_log_" + std::to_string(i) + ".txt");
				log_file << log_str.get();
				log_file.close();
#endif

				if (ocl_err != CL_SUCCESS)
				{
					clReleaseContext(m_ocl_context);
					clReleaseCommandQueue(m_ocl_cmd_queue);
					clReleaseProgram(ocl_program);
					continue;
				}

				m_ocl_kernel = clCreateKernel(ocl_program, "propagate", &ocl_err);
				clReleaseProgram(ocl_program);

				if (ocl_err != CL_SUCCESS)
				{
					clReleaseContext(m_ocl_context);
					clReleaseCommandQueue(m_ocl_cmd_queue);
					continue;
				}

				m_ocl_buffer_pos = clCreateFromGLBuffer(m_ocl_context, CL_MEM_READ_WRITE, m_vbo, &ocl_err);
				if (ocl_err != CL_SUCCESS)
				{
					clReleaseContext(m_ocl_context);
					clReleaseCommandQueue(m_ocl_cmd_queue);
					clReleaseKernel(m_ocl_kernel);
					continue;
				}

				m_ocl_buffer_vel = clCreateBuffer(m_ocl_context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
					m_num * sizeof(Vector4D), m_vel.get(), &ocl_err);

				if (ocl_err != CL_SUCCESS)
				{
					clReleaseContext(m_ocl_context);
					clReleaseCommandQueue(m_ocl_cmd_queue);
					clReleaseKernel(m_ocl_kernel);
					clReleaseMemObject(m_ocl_buffer_pos);
					continue;
				}

				m_ocl_buffer_acc = clCreateBuffer(m_ocl_context, CL_MEM_READ_WRITE,
					(m_num * (m_num - 1) / 2) * sizeof(cl_float), nullptr, &ocl_err);

				if (ocl_err != CL_SUCCESS)
				{
					clReleaseContext(m_ocl_context);
					clReleaseCommandQueue(m_ocl_cmd_queue);
					clReleaseKernel(m_ocl_kernel);
					clReleaseMemObject(m_ocl_buffer_pos);
					clReleaseMemObject(m_ocl_buffer_vel);
					continue;
				}

				ocl_err = clSetKernelArg(m_ocl_kernel, 0, sizeof(cl_mem), &m_ocl_buffer_pos);
				if (ocl_err != CL_SUCCESS)
				{
					clReleaseContext(m_ocl_context);
					clReleaseCommandQueue(m_ocl_cmd_queue);
					clReleaseKernel(m_ocl_kernel);
					clReleaseMemObject(m_ocl_buffer_pos);
					clReleaseMemObject(m_ocl_buffer_vel);
					clReleaseMemObject(m_ocl_buffer_acc);
					continue;
				}

				ocl_err = clSetKernelArg(m_ocl_kernel, 1, sizeof(cl_mem), &m_ocl_buffer_vel);
				if (ocl_err != CL_SUCCESS)
				{
					clReleaseContext(m_ocl_context);
					clReleaseCommandQueue(m_ocl_cmd_queue);
					clReleaseKernel(m_ocl_kernel);
					clReleaseMemObject(m_ocl_buffer_pos);
					clReleaseMemObject(m_ocl_buffer_vel);
					clReleaseMemObject(m_ocl_buffer_acc);
					continue;
				}

				ocl_err = clSetKernelArg(m_ocl_kernel, 2, sizeof(cl_mem), &m_ocl_buffer_acc);
				if (ocl_err != CL_SUCCESS)
				{
					clReleaseContext(m_ocl_context);
					clReleaseCommandQueue(m_ocl_cmd_queue);
					clReleaseKernel(m_ocl_kernel);
					clReleaseMemObject(m_ocl_buffer_pos);
					clReleaseMemObject(m_ocl_buffer_vel);
					clReleaseMemObject(m_ocl_buffer_acc);
					continue;
				}
				
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
		glFinish();

		cl_int ocl_err = clEnqueueAcquireGLObjects(m_ocl_cmd_queue, 1, &m_ocl_buffer_pos, 0, nullptr, nullptr);
		if (ocl_err != CL_SUCCESS)
		{
			release();
			throw std::exception("OpenCL cannot acquire OpenGL buffer.");
		}

		const size_t ocl_global_work_size = m_num;
		ocl_err = clEnqueueNDRangeKernel(m_ocl_cmd_queue, m_ocl_kernel, 1, nullptr, &ocl_global_work_size, nullptr, 0, nullptr, nullptr);
		if (ocl_err != CL_SUCCESS)
		{
			release();
			throw std::exception("OpenCL cannot run kernel.");
		}

		ocl_err = clEnqueueReleaseGLObjects(m_ocl_cmd_queue, 1, &m_ocl_buffer_pos, 0, nullptr, nullptr);
		if (ocl_err != CL_SUCCESS)
		{
			release();
			throw std::exception("OpenCL cannot release OpenGL buffer.");
		}

		ocl_err = clFinish(m_ocl_cmd_queue);
		if (ocl_err != CL_SUCCESS)
		{
			release();
			throw std::exception("OpenCL cannot finish.");
		}
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
		glVertexPointer(4, GL_FLOAT, 0, nullptr);
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
