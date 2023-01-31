
#define _USE_MATH_DEFINES

#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <vector>

namespace ColoredDisc
{
	static unsigned int base;
	void drawScene(void)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glLoadIdentity();

		glPushMatrix();
		glColor3f(1.f, 0.f, 0.f);
		glCallList(1);
		glPopMatrix();
		
		glPushMatrix();
		glRotated(60.0, 0.0, 0.0, 1.0);
		glColor3f(0.f, 1.f, 0.f);
		glCallList(1);
		glPopMatrix();

		glPushMatrix();
		glRotated(120.0, 0.0, 0.0, 1.0);
		glColor3f(0.f, 0.f, 1.f);
		glCallList(1);
		glPopMatrix();

		glPushMatrix();
		glRotated(180.0, 0.0, 0.0, 1.0);
		glColor3f(1.f, 1.f, 0.f);
		glCallList(1);
		glPopMatrix();

		glPushMatrix();
		glRotated(240.0, 0.0, 0.0, 1.0);
		glColor3f(1.f, 0.f, 1.f);
		glCallList(1);
		glPopMatrix();

		glPushMatrix();
		glRotated(300.0, 0.0, 0.0, 1.0);
		glColor3f(135.f / 255.f, 206.f / 255.f, 235.f / 255.f);
		glCallList(1);
		glPopMatrix();

		glFlush();
	}

	void setup(void)
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);

		glEnableClientState(GL_VERTEX_ARRAY);

		base = glGenLists(1);
		glNewList(base, GL_COMPILE);

		std::vector<float> v;

		const auto N = 6;
		const auto R = 15.f;
		float t;
		
		v.push_back(0.f);
		v.push_back(0.f);
		v.push_back(-5.f);

		for (int i = 0; i <= N; ++i)
		{
			t = M_PI / 3 * i / N;
			v.push_back(0.f + R * cos(t));
			v.push_back(0.f + R * sin(t));
			v.push_back(-5.f);
		}

		glVertexPointer(3, GL_FLOAT, 0, v.data());
		glDrawArrays(GL_TRIANGLE_FAN, 0, v.size() / 3);

		glEndList();

		glListBase(base);
	}

	void resize(int w, int h)
	{
		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-25.0, 25.0, -25.0, 25.0, 5.0, 100.0);
		glMatrixMode(GL_MODELVIEW);
	}

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

	int main(int argc, char** argv)
	{
		glutInit(&argc, argv);

		glutInitContextVersion(4, 3);
		glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
		glutInitWindowSize(500, 500);
		glutInitWindowPosition(100, 100);
		glutCreateWindow("ColoredDisc");
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