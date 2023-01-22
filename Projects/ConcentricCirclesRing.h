/////////////////////////////////////////////////////////////////////         
//
// This program draws a circle with glDrawArrays()
//
// Interaction:
// Press +/- to increase/decrease the number of vertices of the loop. 
//
///////////////////////////////////////////////////////////////////// 

#define _USE_MATH_DEFINES 

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <vector>
#include <random>

namespace ConcentricCirclesRing
{
	static const float R = 40.0; 
	static const float X = 50.0; 
	static const float Y = 50.0;
	static int N = 10; 
	static const float offset = 0.05f;
	static const int concentricCircles = 1.f / offset;
	static unsigned int circle;
	static const float lineWidth = 3.f;

	struct RGB
	{
		float R;
		float G;
		float B;
	};

	std::vector<RGB> circlesColors;

	void initColors()
	{
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_real_distribution<float> dist(0.f,1.f);

		for (int i = 0; i < concentricCircles; ++i)
		{
			RGB color;
			color.R = dist(rng);
			color.G = dist(rng);
			color.B = dist(rng);

			circlesColors.push_back(color);
		}
	}

	void initCircles(bool initial = true)
	{
		if (initial)
			circle = glGenLists(1);

		glNewList(circle, GL_COMPILE);
		glBegin(GL_LINE_STRIP);
		float t;
		for (int i = 0; i <= N; ++i)
		{
			t = 2 * M_PI * i / N;
			glVertex3f(X + R * cos(t), Y - R * sin(t), 0.0);
		}
		glEnd();
		glEndList();
	}

	void reinitilizeCircles()
	{
		initCircles(false);
	}

	void setup()
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);

		initColors();
		initCircles();
	}
	
	void drawRingWithColor(float XScale, float YScale, RGB color)
	{
		glColor3f(color.R, color.G, color.B);
		glPushMatrix();
		glTranslatef(X * (1.f - XScale), Y * (1.f - YScale), 0.f);
		glScalef(XScale, YScale, 1.f);
		glCallList(circle);
		glPopMatrix();
	}

	void drawScene(void)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glLineWidth(lineWidth);

		for (int i = 0; i < concentricCircles; ++i)
		{
			const auto color = circlesColors[i];
			drawRingWithColor(1.f - 0.05f * i, 1.f - 0.05f * i, color);
		}

		glFlush();
	}

	void resize(int w, int h)
	{
		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0, 100.0, 0.0, 100.0, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	void keyInput(unsigned char key, int x, int y)
	{
		switch (key)
		{
		case 27:
			exit(0);
			break;
		case '+':
			N++;
			reinitilizeCircles();
			glutPostRedisplay();
			break;
		case '-':
			if (N > 3)
			{
				N--;
				reinitilizeCircles();
				glutPostRedisplay();
			}
			break;
		default:
			break;
		}
	}

	void printInteraction(void)
	{
		std::cout << "Interaction:" << std::endl;
		std::cout << "Press +/- to increase/decrease the number of vertices on the circle." << std::endl;
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
		glutCreateWindow("concentricCirclesRing");
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