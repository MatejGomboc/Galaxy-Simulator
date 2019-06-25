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

#include "camera.h"
#include "utils.h"
#include <cmath>
#include <GL/freeglut.h>


Camera::Camera() :
m_phi(0.0f),
m_theta(0.0f),
m_zoom(2.0f),
m_uptodate(false)
{
}


void Camera::updatePosition()
{
	if (!m_uptodate)
	{
		m_position[0] = m_zoom * cos(m_phi) * cos(m_theta);
		m_position[1] = m_zoom * sin(m_phi) * cos(m_theta);
		m_position[2] = m_zoom * sin(m_theta);

		m_up[0] = -cos(m_phi) * sin(m_theta);
		m_up[1] = -sin(m_phi) * sin(m_theta);
		m_up[2] = cos(m_theta);

		m_uptodate = true;
	}
}


void Camera::set_phi(float phi)
{
	m_phi = Utils::rad(Utils::wrap(phi, -180.0f, 180.0f));
	m_uptodate = false;
}


float Camera::get_phi() const
{
	return Utils::deg(m_phi);
}


void Camera::set_theta(float theta)
{
	m_theta = Utils::rad(Utils::wrap(theta, -180.0f, 180.0f));
	m_uptodate = false;
}


float Camera::get_theta() const
{
	return Utils::deg(m_theta);
}


void Camera::set_zoom(float zoom)
{
	m_zoom = Utils::clamp(zoom, 1.5f, 10.0f);
	m_uptodate = false;
}


float Camera::get_zoom() const
{
	return m_zoom;
}


void Camera::view_transform()
{
	updatePosition();
	gluLookAt
	(
		m_position[0], m_position[1], m_position[2],
		0.0f, 0.0f, 0.0f,
		m_up[0], m_up[1], m_up[2]
	);
}


void Camera::projection_transform(int w, int h)
{
	gluPerspective(60.0f, static_cast<float>(w) / static_cast<float>(h), 0.01f, 100.0f);
}
