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

namespace Circle
{
	static float R = 40.0; // Radius of circle.
	static float X = 50.0; // X-coordinate of center of circle.
	static float Y = 50.0; // Y-coordinate of center of circle.
	static int N = 3; // Number of vertices on circle.

	static std::vector<float> vertices;

	void clearVertices()
	{
		vertices.erase(std::begin(vertices), std::end(vertices));
	}

	void fillVertices()
	{
		vertices.reserve((N + 1) * 3);

		float t;
		for (int i = 0; i <= N; ++i)
		{
			t = 2 * M_PI * i / N;
			vertices.push_back(X + R * cos(t));
			vertices.push_back(Y - R * sin(t));
			vertices.push_back(0.0);
		}

		vertices.shrink_to_fit();
	}

	// Drawing routine.
	void drawScene(void)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glColor3f(0.f, 0.f, 0.f);
		glVertexPointer(3, GL_FLOAT, 0, vertices.data());
		glDrawArrays(GL_LINE_STRIP, 0, vertices.size() / 3);

		glFlush();
	}

	// Initialization routine.
	void setup(void)
	{
		fillVertices();
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glEnableClientState(GL_VERTEX_ARRAY);
	}

	// OpenGL window reshape routine.
	void resize(int w, int h)
	{
		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0, 100.0, 0.0, 100.0, -1.0, 1.0);
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
		case '+':
			N++;
			clearVertices();
			fillVertices();
			glutPostRedisplay();
			break;
		case '-':
			if (N > 3)
			{
				N--;
				clearVertices();
				fillVertices();
				glutPostRedisplay();
			}
			break;
		default:
			break;
		}
	}

	// Routine to output interaction instructions to the C++ window.
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
		glutCreateWindow("circle");
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