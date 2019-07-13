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

#ifndef CAMERA_H
#define CAMERA_H

class Camera
{
private:
	bool m_uptodate;
	float m_phi;
	float m_theta;
	float m_zoom;
	float m_position[3];
	float m_up[3];
	void updatePosition();

public:
	Camera();
	void set_phi(float phi);
	float get_phi() const;
	void set_theta(float theta);
	float get_theta() const;
	void set_zoom(float zoom);
	float get_zoom() const;
	void view_transform();
	static void projection_transform(int w, int h);
};

#endif
