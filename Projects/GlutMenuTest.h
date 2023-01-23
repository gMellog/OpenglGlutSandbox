//////////////////////////////////////////////////////////////////////////         
// This program shows how to create simple menus with GLUT
//////////////////////////////////////////////////////////////////////////

#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <array>

namespace GlutMenuTest
{
	static float square_color[3] = { 1.0, 0.0, 0.0 }; // Color of the square.
	static bool isWire = false;

	float X = 20.f;
	float Y = 20.f;

	const float small = 30.f;
	const float medium = 45.f;
	const float large = 60.f;

	float width = large;
	float height = large;

	std::array<float, 3> arr
	{
		small, medium, large
	};

	void drawScene(void)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glPolygonMode(GL_FRONT_AND_BACK, isWire ? GL_LINE : GL_FILL);

		glColor3fv(square_color);
		glBegin(GL_POLYGON);
		glVertex2f(X, Y);
		glVertex2f(X + width, Y);
		glVertex2f(X + width, Y + height);
		glVertex2f(X, Y + height);
		glEnd();

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

	void printInteraction(void)
	{
		std::cout << "Interaction:" << std::endl;
		std::cout << "Press the right mouse button to see options." << std::endl;
	}

	void top_menu(int id)
	{
		if (id == 1) exit(0);
	}

	void color_menu(int id)
	{
		if (id == 2)
		{
			square_color[0] = 1.0; square_color[1] = 0.0; square_color[2] = 0.0;
		}
		if (id == 3)
		{
			square_color[0] = 0.0; square_color[1] = 0.0; square_color[2] = 1.0;
		}
		glutPostRedisplay();
	}

	void modeMenu(int id)
	{
		isWire = id;
		glutPostRedisplay();
	}

	void sizeMenu(int id)
	{
		std::cout << "in sizeMenu()\n";
	}

	void widthMenu(int id)
	{
		width = arr[id];
		glutPostRedisplay();
	}

	void heightMenu(int id)
	{
		height = arr[id];
		glutPostRedisplay();
	}
	
	void makeMenu(void)
	{
		int sub_menu;
		sub_menu = glutCreateMenu(color_menu);
		glutAddMenuEntry("Red", 2);
		glutAddMenuEntry("Blue", 3);

		int modeMenuID;
		modeMenuID = glutCreateMenu(modeMenu);
		glutAddMenuEntry("Fill", 0);
		glutAddMenuEntry("Outline", 1);

		int widthMenuID;
		widthMenuID = glutCreateMenu(widthMenu);
		glutAddMenuEntry("Small", 0);
		glutAddMenuEntry("Medium", 1);
		glutAddMenuEntry("Large", 2);

		int heightMenuID;
		heightMenuID = glutCreateMenu(heightMenu);
		glutAddMenuEntry("Small", 0);
		glutAddMenuEntry("Medium", 1);
		glutAddMenuEntry("Large", 2);

		int sizeMenuID;
		sizeMenuID = glutCreateMenu(sizeMenu);
		glutAddSubMenu("Width", widthMenuID);
		glutAddSubMenu("Height", heightMenuID);

		glutCreateMenu(top_menu);
		glutAddSubMenu("Color", sub_menu);
		glutAddSubMenu("Mode", modeMenuID);
		glutAddSubMenu("Size", sizeMenuID);
		glutAddMenuEntry("Quit", 1);

		glutAttachMenu(GLUT_RIGHT_BUTTON);
	}

	// Initialization routine.
	void setup(void)
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);

		makeMenu();
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
		glutCreateWindow("menus.cpp");
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