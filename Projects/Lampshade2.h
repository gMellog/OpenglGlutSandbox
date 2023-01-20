///////////////////////////////////////////////////////////////////////////////////////          
// This program draws lampshade2
//
// Interaction:
// Press x, X, y, Y, z, Z to turn the lampshade.
/////////////////////////////////////////////////////////////////////////////////////// 

#define _USE_MATH_DEFINES 

#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

namespace Lampshade2
{
	static float Xangle = 40.0, Yangle = 0.0, Zangle = 0.0;
	static float R0 = 20.f;
	static float R1 = 12.5f;
	static int N = 15;

	// Initialization routine.
	void setup(void)
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);
	}

	// Drawing routine.
	void drawScene(void)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glLoadIdentity();

		glTranslatef(0.0, 0.0, -40.0);

		glRotatef(Zangle, 0.0, 0.0, 1.0);
		glRotatef(Yangle, 0.0, 1.0, 0.0);
		glRotatef(Xangle, 1.0, 0.0, 0.0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor3f(0.0, 0.0, 0.0);

		glBegin(GL_TRIANGLE_STRIP);

		float t;
		for (int i = 0; i <= N; ++i)
		{
			t = 2 * M_PI * i / N;

			glVertex3f(R0 * cos(t), 0.f, R0 * sin(t));
			glVertex3f(R1 * cos(t), 10.f, R1 * sin(t));
		}

		glEnd();

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
		std::cout << "Interaction:\n";
		std::cout << "Press x, X, y, Y, z, Z to turn the lampshade" << std::endl;
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
		glutCreateWindow("Lampshade2");
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

