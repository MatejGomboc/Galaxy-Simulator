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


Stars::Stars(GLulong num) :
	m_num(num),
	m_vbo(0),
	m_vel(nullptr)
{
}


Stars::~Stars()
{
	if (m_vbo)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glDeleteBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		m_vbo = 0;
	}
}


void Stars::init()
{
	if (!m_vbo)
	{
		std::random_device gen; // use a hardware entropy source if available, otherwise use PRNG
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
	}
}


void Stars::calculate()
{
	if (m_vbo)
	{
		// TODO !!
	}
}


void Stars::draw()
{
	if (m_vbo)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glVertexPointer(3, GL_FLOAT, 0, nullptr);
		glEnableClientState(GL_VERTEX_ARRAY);
		glColor3f(1.0f, 1.0f, 0.0f);
		glDrawArrays(GL_POINTS, 0, m_num);
		glDisableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}
