
#define _USE_MATH_DEFINES 

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <random>
#include <type_traits>

namespace Clown
{
	static float angle = 0.0; // Rotation.angle of hat.
	static bool isAnimate; // Animated?
	static int animationPeriod = 100; // Time interval between frames.

	struct Color
	{
		Color(float pR = 0.f, float pG = 0.f, float pB = 0.f)
			:
			R{ pR },
			G{ pG },
			B{ pB }
		{

		}

		float R;
		float G;
		float B;
	};

	template<typename T1>
	struct Random
	{
		static T1 getRandomValue(T1 from, T1 to)
		{
			return getRandomValueImpl(std::bool_constant<std::is_floating_point_v<T1>>(), from, to);
		}

	private:

		static T1 getRandomValueImpl(std::true_type, T1 from, T1 to)
		{
			std::uniform_real_distribution<T1> di(from, to);
			return di(mt);
		}

		static T1 getRandomValueImpl(std::false_type, T1 from, T1 to)
		{
			std::uniform_int_distribution<T1> di(from, to);
			return di(mt);
		}

		static std::mt19937 mt;
	};

	template<typename T>
	std::mt19937 Random<T>::mt{ (unsigned int)time(nullptr) };

	float getRBasedOnAngle(float minR, float maxR)
	{
		return std::abs(minR + maxR * sin(M_PI / 180 * angle));
	}

	void rotateEye(float eyePosX, float eyePosY, const Color& eyeColor)
	{
		glPushMatrix();

		glTranslated(eyePosX, eyePosY, 2.5f);
		glColor3f(0.f, 0.f, 0.f);

		const auto minOuterR = 0.2f;
		const auto maxOuterR = 0.25f;
		const auto maxMinRatio = maxOuterR / minOuterR;
		const auto t = std::abs(sin(M_PI / 180 * angle));

		const auto outerSphereR = minOuterR + t * (maxOuterR - minOuterR);
		glutSolidSphere(outerSphereR, 15, 2);

		glColor3f(eyeColor.R, eyeColor.G, eyeColor.B);

		const auto R = 0.07f;
		const auto ROffset = R + t * (R * maxMinRatio);

		const auto whiteEyeX = R * cos(M_PI / 180 * angle);
		const auto whiteEyeY = R * sin(M_PI / 180 * angle);

		const auto Z = 0.2f;
		glTranslated(whiteEyeX, whiteEyeY, Z);
		const auto inSphereR = 0.075f;
		const auto inSphereROffset = inSphereR + t * (inSphereR * maxMinRatio / 2);
		glutSolidSphere(inSphereROffset, 15, 2);

		glPopMatrix();
	}

	void drawScene(void)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		glTranslatef(0.0, 0.0, -9.0);

		glColor3f(0.0, 0.0, 1.0);
		glutWireSphere(2.0, 20, 20);

		glPushMatrix();

		const auto coneNoseBase = 0.175f + 0.2f * std::abs(sin((M_PI / 180) * angle));
		const auto coneNoseHeight = 1.f + 2.f * std::abs(sin((M_PI / 180) * angle));

		glTranslatef(0.f, 0.f, 3.5);
		glRotatef(220.f, 0.f, 1.f, 0.f);
		glColor3f(Random<int>::getRandomValue(0, 1), Random<int>::getRandomValue(0, 1), Random<int>::getRandomValue(0, 1));
		glutWireCone(coneNoseBase, coneNoseHeight, 10, 10);

		glPopMatrix();

		Color eyeColor{
			(float)Random<int>::getRandomValue(0, 1),
			(float)Random<int>::getRandomValue(0, 1),
			(float)Random<int>::getRandomValue(0, 1)
		};

		rotateEye(-0.5f, 0.5f, eyeColor);
		rotateEye(0.5f, 0.5f, eyeColor);

		glPushMatrix();

		// Transformations of the hat and brim.
		glRotatef(angle, 0.0, 1.0, 0.0);
		glRotatef(30.0, 0.0, 0.0, 1.0);
		glRotatef(-90.0, 1.0, 0.0, 0.0);
		glTranslatef(0.0, 0.0, 2.0);

		// Hat.
		glColor3f(0.0, 1.0, 0.0);
		glutWireCone(2.0, 4.0, 20, 20);

		// Brim.
		glutWireTorus(0.2, 2.2, 10, 25);

		glPopMatrix();

		glPushMatrix();

		glTranslatef(sin((M_PI / 180.0) * angle), 0.0, 0.0);
		glTranslatef(3.5, 0.0, 0.0);

		glColor3f(1.0, 0.0, 0.0);
		glutWireSphere(0.5, 10, 10);

		glPopMatrix();

		glPushMatrix();

		glTranslatef(-sin((M_PI / 180.0) * angle), 0.0, 0.0);
		glTranslatef(-3.5, 0.0, 0.0);

		glColor3f(1.0, 0.0, 0.0);
		glutWireSphere(0.5, 10, 10);

		glPopMatrix();

		glPushMatrix();

		glTranslatef(-2.0, 0.0, 0.0);
		glScalef(-1 - sin((M_PI / 180.0) * angle), 1.0, 1.0);

		glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_LINE_STRIP);
		for (float t = 0.0; t <= 1.0; t += 0.05)
			glVertex3f(t, 0.25 * cos(10.0 * M_PI * t), 0.25 * sin(10.0 * M_PI * t));
		glEnd();

		glPopMatrix();

		glPushMatrix();

		// Transformations of the spring to the right ear.
		glTranslatef(2.0, 0.0, 0.0);
		glScalef(1 + sin((M_PI / 180.0) * angle), 1.0, 1.0);

		// Spring to right ear.
		glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_LINE_STRIP);
		for (float t = 0.0; t <= 1.0; t += 0.05)
			glVertex3f(t, 0.25 * cos(10.0 * M_PI * t), 0.25 * sin(10.0 * M_PI * t));
		glEnd();

		glPopMatrix();
		glutSwapBuffers();
	}

	// Routine to increase the rotation angle.
	void increaseAngle(void)
	{
		angle += 5.0;
		if (angle > 360.0) angle -= 360.0;
	}

	// Timer function.
	void animate(int value)
	{
		if (isAnimate)
		{
			increaseAngle();

			glutPostRedisplay();
			glutTimerFunc(animationPeriod, animate, 1);
		}
	}

	// Initialization routine.
	void setup(void)
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glEnable(GL_DEPTH_TEST);
	}

	// OpenGL window reshape routine.
	void resize(int w, int h)
	{
		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);

		glMatrixMode(GL_MODELVIEW);
	}

	// Keyboard input processing routine.
	void keyInput(unsigned char key, int x, int y)
	{
		switch (key)
		{
		case 27:
			exit(0);
			break;
		case ' ':
			if (isAnimate) isAnimate = 0;
			else
			{
				isAnimate = 1;
				animate(1);
			}
			glutPostRedisplay();
			break;
		default:
			break;
		}
	}

	// Callback routine for non-ASCII key entry.
	void specialKeyInput(int key, int x, int y)
	{
		if (key == GLUT_KEY_DOWN) animationPeriod += 5;
		if (key == GLUT_KEY_UP) if (animationPeriod > 5) animationPeriod -= 5;
		glutPostRedisplay();
	}

	// Routine to output interaction instructions to the C++ window.
	void printInteraction(void)
	{
		std::cout << "Interaction:" << std::endl;
		std::cout << "Press space to toggle between animation on and off." << std::endl
			<< "Press the up/down arrow keys to speed up/slow down animation." << std::endl;
	}

	// Main routine.
	int main(int argc, char** argv)
	{
		printInteraction();
		glutInit(&argc, argv);

		glutInitContextVersion(4, 3);
		glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
		glutInitWindowSize(500, 500);
		glutInitWindowPosition(100, 100);
		glutCreateWindow("clown3.cpp");
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

