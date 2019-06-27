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

#include <iostream>
#include <cstdlib>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "camera.h"
#include "coordinate_axes.h"
#include "stars.h"


Camera camera;
Coordinate_axes coordinate_axes;
Stars stars(1000);


void init(void)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);

	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);

	stars.init();
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.view_transform();

	coordinate_axes.draw();
	stars.draw();

	glFlush();
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	camera.projection_transform(w, h);
	glutPostRedisplay();
}

void keyboard(unsigned char c, int x, int y)
{
	bool changed = false;

	switch (c)
	{
	case '6':
		camera.set_phi(camera.get_phi() + 10.0f); // R
		changed = true;
		break;

	case '4':
		camera.set_phi(camera.get_phi() - 10.0f); // L
		changed = true;
		break;

	case '2':
		camera.set_theta(camera.get_theta() - 10.0f); // D
		changed = true;
		break;

	case '8':
		camera.set_theta(camera.get_theta() + 10.0f); // U
		changed = true;
		break;

	case '-':
		camera.set_zoom(camera.get_zoom() * 1.25f); // Out
		changed = true;
		break;

	case '+':
		camera.set_zoom(camera.get_zoom() * 0.75f); // In
		changed = true;
		break;
	}

	if (changed)
		glutPostRedisplay();
}

void timer(int value)
{
	stars.calculate();
	glutPostRedisplay();
	glutTimerFunc(100, timer, 0);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutCreateWindow(argv[0]);

	glewInit();

	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(100, timer, 0);

	glutMainLoop();

	return EXIT_SUCCESS;
}
