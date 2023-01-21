///////////////////////////////////////////////////////         
//
// This program make spiral band.
//
///////////////////////////////////////////////////////        

#define _USE_MATH_DEFINES 

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>

namespace SpiralBand
{
	static float Xangle = 50.f, Yangle = 0.f, Zangle = 0.f;
	static float R = 20.f;
	static std::vector<float> vertices;
	static std::vector<unsigned int> indices;

	void fillVertices()
	{
		float t;
		for (t = -10 * M_PI; t <= 10 * M_PI; t += M_PI / 20.0)
		{
			vertices.push_back(R * cos(t));
			vertices.push_back(R * sin(t));
			vertices.push_back(t - 60.0);

			vertices.push_back(R * cos(t));
			vertices.push_back(R * sin(t));
			vertices.push_back(t - 55.0);
		}
	}

	void fillIndices()
	{
		float t;
		int k;
		for (k = 0,t = -10 * M_PI; t <= 10 * M_PI; t += M_PI / 20.0)
		{
			indices.push_back(++k);
			indices.push_back(++k);
		}
	}

	// Drawing routine.
	void drawScene(void)
	{
		glLoadIdentity();

		glTranslatef(0.0, -55.0, -50.0);

		glRotatef(Zangle, 0.0, 0.0, 1.0);
		glRotatef(Yangle, 0.0, 1.0, 0.0);
		glRotatef(Xangle, 1.0, 0.0, 0.0);

		glClear(GL_COLOR_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor3f(0.0, 0.0, 0.0);

		glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, indices.data());

		glFlush();
	}

	// Initialization routine.
	void setup(void)
	{
		fillVertices();
		fillIndices();
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, vertices.data());
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

	// Main routine.
	int main(int argc, char** argv)
	{
		glutInit(&argc, argv);

		glutInitContextVersion(4, 3);
		glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
		glutInitWindowSize(500, 500);
		glutInitWindowPosition(100, 100);
		glutCreateWindow("spiralBand");
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

