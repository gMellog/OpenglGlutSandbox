/////////////////////////////////////////////////////////////////////         
//
// This program draws a circle with of VBO and glDrawArrays()
//
///////////////////////////////////////////////////////////////////// 

#define _USE_MATH_DEFINES 

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <vector>

namespace VAOCircle
{
	static float R = 40.0; // Radius of circle.
	static float X = 50.0; // X-coordinate of center of circle.
	static float Y = 50.0; // Y-coordinate of center of circle.
	static unsigned int N = 40; // Number of vertices on circle.
	static unsigned int VAO;
	static std::vector<float> vertices;

	static unsigned int verticyID;

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
		
		//it was already bounded (in setup())so we don't need to call it again
		glDrawArrays(GL_LINE_STRIP, 0, vertices.size() / 3);

		glFlush();
	}

	// Initialization routine.
	void setup(void)
	{
		fillVertices();

		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &verticyID);

		glBindBuffer(GL_ARRAY_BUFFER, verticyID);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, 0);

		glClearColor(1.0, 1.0, 1.0, 0.0);
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
		glutInitWindowPosition(100, 100);
		glutCreateWindow("VAOCircle");
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