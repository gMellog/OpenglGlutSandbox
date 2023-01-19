#pragma once
///////////////////////////////////////////////////////         
// childrenSide.h
//
// This program draws 3D children slide
// 
///////////////////////////////////////////////////////        

#define _USE_MATH_DEFINES 

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

namespace ChildrenSlide
{
	static auto N = 10;

	// Drawing routine.
	void drawScene(void)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(0.0, 0.0, 0.0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBegin(GL_TRIANGLE_STRIP);

		glVertex3f(-20.f, -10.f, -30.f);
		glVertex3f(-15.f, -10.f, -37.5f);
		glVertex3f(-20.f, 7.f, -30.f);
		glVertex3f(-15.f, 10.f, -37.5f);

		const auto curvedAngle = M_PI / 3;
		const auto start = M_PI + (M_PI_2 - curvedAngle);
		const auto xPath = 25.f;
		const auto R = xPath / sin(curvedAngle);
		const auto X0 = -20.f - R * cos(start);
		const auto X1 = -15.f - R * cos(start);
		const auto Y0 =  7.f  - R * sin(start);
		const auto Y1 =  10.f - R * sin(start);

		float t;
		
		for (int i = 1; i <= N; ++i)
		{
			t = (start + (curvedAngle) * i / N);

			glVertex3f(X0 + R * cos(t), Y0 + R * sin(t), -30.f);
			glVertex3f(X1 + R * cos(t), Y1 + R * sin(t), -37.5f);
		}
		
		glVertex3f(X0 + R * cos(t), -10.f, -30.f);
		glVertex3f(X1 + R * cos(t), -10.f, -37.5f);

		glEnd();

		glFlush();
	}

	// Initialization routine.
	void setup(void)
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);
	}

	// OpenGL window reshape routine.
	void resize(int w, int h)
	{
		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	// Keyboard input processing routine.
	void keyInput(unsigned char key, int x, int y)
	{
		switch (key)
		{
		case 27:
			exit(0);
			break;

		default:
			break;
		}
	}

	// Main routine.
	int main(int argc, char** argv)
	{
		glutInit(&argc, argv);

		glutInitContextVersion(4, 3);
		glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
		glutInitWindowSize(500, 500);
		glutInitWindowPosition(500, 200);
		glutCreateWindow("childrenSlide");
		glutDisplayFunc(drawScene);
		glutReshapeFunc(resize);
		glutKeyboardFunc(keyInput);

		glewExperimental = GL_TRUE;
		glewInit();

		setup();

		glutMainLoop();

		return 0;
	}
}
