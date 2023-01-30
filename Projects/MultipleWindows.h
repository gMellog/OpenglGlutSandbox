#define _USE_MATH_DEFINES 

#include <cmath>
#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h> 

namespace MultipleWindows
{
	void writeStrokeString(void* font, const std::string& str)
	{
		for (const auto ch : str) glutStrokeCharacter(font, ch);
	}

	void writeLabel(double x, double y, const std::string& str)
	{
		glColor3f(0.f, 0.f, 0.f);
		glLineWidth(3.f);
		glPushMatrix();
		glTranslated(x, y, 0.0);
		glScaled(0.1, 0.1, 0.1);
		writeStrokeString(GLUT_STROKE_ROMAN, str);
		glPopMatrix();
	}

	void drawScene1(void)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		writeLabel(40.0, 40.0, "Red");

		glFlush();
	}

	void drawScene2(void)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		writeLabel(35.0, 40.0, "Green");

		glFlush();
	}

	void drawScene3(void)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		writeLabel(40.0, 40.0, "Blue");

		glFlush();
	}

	void setup1(void)
	{
		glClearColor(1.0, 0.0, 0.0, 0.0);
	}

	void setup2(void)
	{
		glClearColor(0.0, 1.0, 0.0, 0.0);
	}

	void setup3(void)
	{
		glClearColor(0.0, 0.0, 1.0, 0.0);
	}

	void resize1(int w, int h)
	{
		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0, 100.0, 0.0, 100.0, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	void resize2(int w, int h)
	{
		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0, 100.0, 0.0, 100.0, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	void resize3(int w, int h)
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

		glutInitWindowSize(250, 500);
		glutInitWindowPosition(100, 100);

		glutCreateWindow("window 1");

		setup1();
		glutDisplayFunc(drawScene1);
		glutReshapeFunc(resize1);
		glutKeyboardFunc(keyInput);

		glutInitWindowSize(250, 500);
		glutInitWindowPosition(400, 100);

		glutCreateWindow("window 2");

		setup2();
		glutDisplayFunc(drawScene2);
		glutReshapeFunc(resize2);
		glutKeyboardFunc(keyInput);

		glutInitWindowSize(250, 500);
		glutInitWindowPosition(700, 100);

		glutCreateWindow("window 3");

		setup3();
		glutDisplayFunc(drawScene3);
		glutReshapeFunc(resize3);
		glutKeyboardFunc(keyInput);

		glewExperimental = GL_TRUE;
		glewInit();

		glutMainLoop();

		return 0;
	}
}