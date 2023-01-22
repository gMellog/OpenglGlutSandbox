/////////////////////////////////////////////////////////////////////////////////////         
// This program using multiple  display lists.  
/////////////////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <vector>

namespace MultipleLists
{
	static unsigned int base;
	static std::vector<int> offsets{ 3, 0, 3, 2, 3, 1, 3, 1, 3, 0, 3, 2, 3 };

	void setup(void)
	{
		base = glGenLists(4);

		glNewList(base, GL_COMPILE);
		// Red triangle.
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_TRIANGLES);
		glVertex2f(10.0, 10.0);
		glVertex2f(20.0, 10.0);
		glVertex2f(20.0, 40.0);
		glEnd();
		glTranslatef(15.0, 0.0, 0.0);
		glEndList();

		glNewList(base + 1, GL_COMPILE);
		// Green  rectangle.
		glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_POLYGON);
		glVertex2f(10.0, 10.0);
		glVertex2f(20.0, 10.0);
		glVertex2f(20.0, 40.0);
		glVertex2f(10.0, 40.0);
		glEnd();
		glTranslatef(15.0, 0.0, 0.0);
		glEndList();

		glNewList(base + 2, GL_COMPILE);
		// Blue pentagon.
		glColor3f(0.0, 0.0, 1.0);
		glBegin(GL_POLYGON);
		glVertex2f(10.0, 10.0);
		glVertex2f(20.0, 10.0);
		glVertex2f(20.0, 20.0);
		glVertex2f(15.0, 40.0);
		glVertex2f(10.0, 20.0);
		glEnd();
		glTranslatef(15.0, 0.0, 0.0);
		glEndList();

		glNewList(base + 3, GL_COMPILE);
		glLineWidth(3.f);
		glColor3f(0.f, 0.f, 0.f);
		glBegin(GL_LINES);
		glVertex2f(7.5f, 40.f);
		glVertex2f(7.5f, 10.f);
		glEnd();
		glEndList();

		glClearColor(1.0, 1.0, 1.0, 0.0);
	}

	void drawScene(void)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glLoadIdentity();

		glListBase(base);

		glCallLists(offsets.size(), GL_INT, offsets.data());

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
		glutCreateWindow("multipleLists");
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