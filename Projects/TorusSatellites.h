#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h> 

namespace TorusSatellites
{
	static float latAngle = 0.0;
	static float longAngle = 0.0;
	static float Xangle = 0.0, Yangle = 0.0, Zangle = 0.0;
	static bool isAnimate;
	static int animationPeriod = 100; // Time interval between frames.

	// Drawing routine.
	void drawScene(void)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();
		glTranslatef(0.0, 0.0, -25.0);

		glRotatef(Zangle, 0.0, 0.0, 1.0);
		glRotatef(Yangle, 0.0, 1.0, 0.0);
		glRotatef(Xangle, 1.0, 0.0, 0.0);

		glColor3f(0.0, 1.0, 0.0);
		glutWireTorus(2.0, 12.0, 20, 20);

		glRotatef(longAngle, 0.0, 0.0, 1.0);

		glTranslatef(12.0, 0.0, 0.0);
		glRotatef(latAngle, 0.0, 1.0, 0.0);
		glTranslatef(-12.0, 0.0, 0.0);

		glTranslatef(20.0, 0.0, 0.0);

		glColor3f(0.0, 0.0, 1.0);
		glutWireSphere(2.0, 10, 10);

		glPushMatrix();
		glRotated(latAngle * 2.5f, 0.0, 1.0, 0.0);
		glRotated(120, 0.0, 1.0, 0.0);
		glTranslated(5.0, 0.0, 0.0);

		glColor3f(1.0, 0.0, 0.0);
		glutWireSphere(1.25, 10, 10);
		glPopMatrix();

		glPushMatrix();
		glRotated(latAngle * 2.5f, 0.0, 1.0, 0.0);
		glRotated(240, 0.0, 1.0, 0.0);
		glTranslated(6.0, 0.0, 0.0);

		glColor3f(0.0, 1.0, 0.0);
		glutWireSphere(1.0, 10, 10);
		glPopMatrix();

		glutSwapBuffers();
	}

	void animate(int value)
	{
		if (isAnimate)
		{
			latAngle += 5.0;
			if (latAngle > 360.0) latAngle -= 360.0;
			longAngle += 1.0;
			if (longAngle > 360.0) longAngle -= 360.0;

			glutPostRedisplay();
			glutTimerFunc(animationPeriod, animate, 1);
		}
	}

	void setup(void)
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glEnable(GL_DEPTH_TEST);
	}

	void resize(int w, int h)
	{
		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);

		glMatrixMode(GL_MODELVIEW);
	}

	void keyInput(unsigned char key, int x, int y)
	{
		switch (key)
		{
		case 27:
			exit(0);
			break;
		case ' ':
			isAnimate = !isAnimate;
			if (isAnimate)
			{
				animate(1);
			}
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

	void specialKeyInput(int key, int x, int y)
	{
		if (key == GLUT_KEY_DOWN) animationPeriod += 5;
		if (key == GLUT_KEY_UP) if (animationPeriod > 5) animationPeriod -= 5;
		glutPostRedisplay();
	}

	void printInteraction(void)
	{
		std::cout << "Interaction:" << std::endl;
		std::cout << "Press space to toggle between animation on and off." << std::endl
			<< "Press the up/down arrow keys to speed up/slow down animation." << std::endl
			<< "Press the x, X, y, Y, z, Z keys to rotate the scene." << std::endl;
	}

	int main(int argc, char** argv)
	{
		printInteraction();
		glutInit(&argc, argv);

		glutInitContextVersion(4, 3);
		glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
		glutInitWindowSize(500, 500);
		glutInitWindowPosition(100, 100);
		glutCreateWindow("TorusSatellite's");
		glutDisplayFunc(drawScene);
		glutReshapeFunc(resize);
		glutKeyboardFunc(keyInput);
		glutSpecialFunc(specialKeyInput);

		glewExperimental = GL_TRUE;
		glewInit();

		setup();

		glutMainLoop();
		
		return 0;
	}
}