///////////////////////////////////////////////////////////////////////////////////////          
//
// This program approximates a sphere with triangle strips.
//
// Interaction:
// Press P/p to increase/decrease the number of longitudinal slices.
// Press Q/q to increase/decrease the number of latitudinal slices.
// Press x, X, y, Y, z, Z to turn the hemisphere.
//
/////////////////////////////////////////////////////////////////////////////////////// 

#define _USE_MATH_DEFINES 

#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

namespace Sphere
{
	static float R = 5.0;
	static int p = 6; // Number of longitudinal slices.
	static int q = 4; // Number of latitudinal slices.
	static float Xangle = 0.0, Yangle = 0.0, Zangle = 0.0;

	// Initialization routine.
	void setup(void)
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);
	}

	// Drawing routine.
	void drawScene(void)
	{
		int  i, j;

		glClear(GL_COLOR_BUFFER_BIT);

		glLoadIdentity();

		glTranslatef(0.0, 0.0, -10.0);

		glRotatef(Zangle, 0.0, 0.0, 1.0);
		glRotatef(Yangle, 0.0, 1.0, 0.0);
		glRotatef(Xangle, 1.0, 0.0, 0.0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor3f(0.0, 0.0, 0.0);

		for (j = -q; j < q; j++)
		{
			// One latitudinal triangle strip.
			glBegin(GL_TRIANGLE_STRIP);
			for (i = 0; i <= p; i++)
			{
				auto fi = M_PI_2 * (j + 1) / q;
				const auto th = 2.0 * i / p  * M_PI;

				glVertex3f(R * cos(fi) * cos(th), R * sin(fi), -R * cos(fi) * sin(th));

				fi = M_PI_2 * j / q;

				glVertex3f(R * cos(fi) * cos(th), R * sin(fi), -R * cos(fi) * sin(th));

			}
			glEnd();
		}

		glFlush();
	}

	// OpenGL window reshape routine.
	void resize(int w, int h)
	{
		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);
		glMatrixMode(GL_MODELVIEW);
	}

	// Keyboard input processing routine.
	void keyInput(unsigned char key, int x, int y)
	{
		switch (key)
		{
		case 27:
			exit(0);
			break;
		case 'P':
			p += 1;
			glutPostRedisplay();
			break;
		case 'p':
			if (p > 3) p -= 1;
			glutPostRedisplay();
			break;
		case 'Q':
			q += 1;
			glutPostRedisplay();
			break;
		case 'q':
			if (q > 3) q -= 1;
			glutPostRedisplay();
			break;
		case 'x':
			Xangle += 5.0;
			if (Xangle > 360.0) Xangle -= 360.0;
			glutPostRedisplay();
			break;
		case 'X':
			Xangle -= 5.0;
			if (Xangle < 0.0) Xangle += 360.0;
			glutPostRedisplay();
			break;
		case 'y':
			Yangle += 5.0;
			if (Yangle > 360.0) Yangle -= 360.0;
			glutPostRedisplay();
			break;
		case 'Y':
			Yangle -= 5.0;
			if (Yangle < 0.0) Yangle += 360.0;
			glutPostRedisplay();
			break;
		case 'z':
			Zangle += 5.0;
			if (Zangle > 360.0) Zangle -= 360.0;
			glutPostRedisplay();
			break;
		case 'Z':
			Zangle -= 5.0;
			if (Zangle < 0.0) Zangle += 360.0;
			glutPostRedisplay();
			break;
		default:
			break;
		}
	}

	// Routine to output interaction instructions to the C++ window.
	void printInteraction(void)
	{
		std::cout << "Interaction:" << std::endl;
		std::cout << "Press P/p to increase/decrease the number of longitudinal slices." << std::endl
			<< "Press Q/q to increase/decrease the number of latitudinal slices." << std::endl
			<< "Press x, X, y, Y, z, Z to turn the hemisphere." << std::endl;
	}

	// Main routine.
	int main(int argc, char** argv)
	{
		printInteraction();
		glutInit(&argc, argv);

		glutInitContextVersion(4, 3);
		glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
		glutInitWindowSize(500, 500);
		glutInitWindowPosition(100, 100);
		glutCreateWindow("Sphere.cpp");
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