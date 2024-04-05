///////////////////////////////////////////////////////////////////////////////////////          
//Simple start up program
/////////////////////////////////////////////////////////////////////////////////////// 

#define _USE_MATH_DEFINES 

#include <cmath>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

namespace SierpinskiGasket
{
	
	struct Point
	{
		float X;
		float Y;
	};

	void setup(void)
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glColor3f(0.f, 0.f, 0.f);
		glPointSize(4.0);
	}
	
	int random(int m)
	{
		return rand() % m;
	}

	void drawDot(const Point& point)
	{
		glBegin(GL_POINTS);
			glVertex3f(point.X, point.Y, 0.f);
		glEnd();
	}

	void drawScene(void)
	{
		std::cout << "drawScene() called\n";
		glClear(GL_COLOR_BUFFER_BIT);
		
		Point points[]{ {10.f, 10.f}, {300.f, 300.f}, {200.f, 300.f} };
		int i = random(3);
		Point point = points[i];
		
		drawDot(point);
		
		for(int j = 0; j < 1000; ++j)
		{
			i = random(3);
			point.X = (point.X + points[i].X) / 2;
			point.Y = (point.Y + points[i].Y) / 2;
			drawDot(point);
		}
		
		std::cout << "After drawScene()\n";
		glFlush();
	}

	void resize(int w, int h)
	{
		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0.0, 640.0, 0.0, 480.0);
	}

	void keyInput(unsigned char key, int x, int y)
	{
		switch (key)
		{
			case 27:
				exit(0);
				break;
			case ' ':
				glutPostRedisplay();
				break;
		}
	}

	void printInteraction(void)
	{
		std::cout << "This is a program which draws Sierpinski gasket\n";
		std::cout << "But you can quit with using of ESC!\n";
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
