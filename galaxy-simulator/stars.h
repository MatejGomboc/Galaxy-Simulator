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

#ifndef STARS_H
#define STARS_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <CL/opencl.h>
#include <memory>

class Stars
{
private:
	struct Vector4D
	{
		GLfloat x;
		GLfloat y;
		GLfloat z;
		GLfloat w;
	};

	const GLsizei m_num;

	bool m_initialised;
	GLuint m_vbo;
	std::unique_ptr<Vector4D[]> m_vel;
	cl_context m_ocl_context;
	cl_command_queue m_ocl_cmd_queue;
	cl_kernel m_ocl_kernel;
	cl_mem m_ocl_buffer_pos;
	cl_mem m_ocl_buffer_vel;
	cl_mem m_ocl_buffer_acc;

	void release();

public:
	Stars(GLulong num);
	~Stars();
	void init();
	void calculate();
	void draw();
};

#endif
