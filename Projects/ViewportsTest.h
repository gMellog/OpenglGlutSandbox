#define _USE_MATH_DEFINES 

#include <cmath>
#include <iostream>
#include <array>
#include <GL/glew.h>
#include <GL/freeglut.h> 

namespace ViewportsTest
{
	static int width, height;

	void writeStrokeString(void* font, const std::string& str)
	{
		for (const auto ch : str)
			glutStrokeCharacter(font, ch);
	}

	// Drawing routine.
	void drawScene(void)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glColor3d(0.0, 0.0, 0.0);
		
		glViewport(0, 0, width, height); //important thing, actually, if you remove that line then when you move the window you'll get a gift
		glLineWidth(2.f);

		const float arrV[]{
			0.f, 50.f,
			100.f, 50.f,
			50.f, 100.f,
			50.f, 0.f
		};

		const int first[]{
			0, 2
		};

		const int count[]{
			2, 2
		};

		glVertexPointer(2, GL_FLOAT, 0, arrV);
		glMultiDrawArrays(GL_LINES, first, count, 2);

		glLineWidth(3.f);

		glViewport(0, height / 2, width / 2, height / 2);

		glPushMatrix();
		glTranslated(30.0, 30.0, 0.0);
		glScaled(0.1, 0.1, 0.1);
		writeStrokeString(GLUT_STROKE_ROMAN, "This");
		glPopMatrix();

		glViewport(width / 2, height / 2, width / 2, height / 2);

		glPushMatrix();
		glTranslated(30.0, 30.0, 0.0);
		glScaled(0.1, 0.1, 0.1);
		writeStrokeString(GLUT_STROKE_ROMAN, "is");
		glPopMatrix();

		glViewport(0, 0, width / 2, height / 2);
		glPushMatrix();
		glTranslated(37.5, 60.0, 0.0);
		glScaled(0.1, 0.1, 0.1);
		writeStrokeString(GLUT_STROKE_ROMAN, "so");
		glPopMatrix();

		glViewport(width / 2, 0, width / 2, height / 2);

		glPushMatrix();
		glTranslated(23.5, 60.0, 0.0);
		glScaled(0.1, 0.1, 0.1);
		writeStrokeString(GLUT_STROKE_ROMAN, "easy");
		glPopMatrix();

		glFlush();
	}

	void setup(void)
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glEnableClientState(GL_VERTEX_ARRAY);
	}

	void resize(int w, int h)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0, 100.0, 0.0, 100.0, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		width = w;
		height = h;
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
		glutCreateWindow("ViewportsTest");
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