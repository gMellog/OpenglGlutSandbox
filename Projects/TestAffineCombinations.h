///////////////////////////////////////////////////////////////////////////////////////          
//Affine Combinations test
/////////////////////////////////////////////////////////////////////////////////////// 

#define _USE_MATH_DEFINES 

#include <cmath>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

namespace AffineCombinations
{
	const int screenWidth = 640;
	const int screenHeight = 480;

	void setup(void)
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glColor3f(0.f, 0.f, 0.f);
		glPointSize(4.0);
	}

	double f(double x)
	{
		return std::exp(-x) * std::cos(2 * M_PI * x);
	}
	
	void drawScene(void)
	{
		glClear(GL_COLOR_BUFFER_BIT);


		GLdouble A, B, C, D, x;
		A = screenWidth / 4.0;
		B = 0.0;
		C = screenHeight / 2.0;
		D = C;

		glBegin(GL_POINTS);
			for(x = 0.0; x < 4.0; x += 0.05)
				glVertex2d(A * x + B, C * f(x) + D);
		glEnd();

		glFlush();
	}

	void resize(int w, int h)
	{
		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0.0, screenWidth, 0.0, screenHeight);
	}

	void keyInput(unsigned char key, int x, int y)
	{
		switch (key)
		{
		case 27:
			exit(0);
			break;
		}
	}

	void printInteraction(void)
	{
		std::cout << "There is no interaction, just check out things programm\n";
		std::cout << "But you can quit with using of ESC!\n";
	}

	int main(int argc, char** argv)
	{
		printInteraction();
		glutInit(&argc, argv);

		glutInitContextVersion(4, 3);
		glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
		glutInitWindowSize(screenWidth, screenHeight);
		glutInitWindowPosition(100, 100);
		glutCreateWindow("Check out things");
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
