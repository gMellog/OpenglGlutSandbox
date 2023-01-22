#pragma once
///////////////////////////////////////////////////////         
// This program draws 3D cone with user 
// defined amount of sides.
// 
// Sides could be set by the user with terminal where 
// also used some technics of input handling 
///////////////////////////////////////////////////////        

#define _USE_MATH_DEFINES 

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

namespace Cone
{
	static int N;

	void drawScene(void)
	{
		const float X = 0.f;
		const float Y = 10.f;
		const float Z = -20.f;

		float R = 20.0;
		float t;

		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(0.0, 0.0, 0.0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBegin(GL_TRIANGLE_FAN);

		glVertex3f(X, Y, Z);
		for (int i = 0; i <= N; ++i)
		{
			t = (2 * M_PI * i / N);

			glVertex3f(X + R * cos(t), -20.f, -50.f + R * sin(t));
		}

		glEnd();

		glFlush();
	}

	void setup(void)
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);
	}

	void resize(int w, int h)
	{
		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	void promptSides()
	{
		int sides = 1;

		while (1)
		{
			std::cout << "\nWrite amount of sides: ";
			std::cin >> sides;

			if (std::cin.fail())
			{
				std::cout << "Write a number not something else\n";

				if (!std::cin.eof())
				{
					std::cin.clear();

					while (!isspace(std::cin.get()))
						continue;
				}
				else
				{
					// user sent to us CRTL+Z, CTR+C, etc command
					std::cin.clear();
				}

				continue;
			}
			else if (sides < 3)
			{
				std::cout << "Wrong amount sides, try again\n";
				continue;
			}

			N = sides;
			break;
		}

		glutPostRedisplay();
	}

	void keyInput(unsigned char key, int x, int y)
	{
		switch (key)
		{
		case 27:
			exit(0);
			break;
		case '`':
			promptSides();
			glutPostRedisplay();
			break;
		default:
			break;
		}
	}

	void printInteraction()
	{
		std::cout <<
			"Interaction:\n"
			"1. Press ` (it's left up button close to ESC)\n"
			"2. Follow to prompted instructions\n"
			"3. Finally you'll get cone respective your response\n";
	}

	int main(int argc, char** argv)
	{
		printInteraction();
		glutInit(&argc, argv);

		glutInitContextVersion(4, 3);
		glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
		glutInitWindowSize(500, 500);
		glutInitWindowPosition(100, 100);
		glutCreateWindow("cone");
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
