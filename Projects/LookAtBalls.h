#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h> 

namespace LookAtBalls
{
	static bool isAnimate;
	float t;
	float animatePeriod = 100.f;
	const int ballsAmount = 10;

	template<typename T1, typename T2>
	T1 lerp(T1 a, T1 b, T2 t)
	{
		return (1 - t) * a + b * t;
	}

	void drawScene(void)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(0.0, 0.0, 0.0);
		glLoadIdentity();

		if(isAnimate)
			t += 0.01;
		
		const auto X = lerp(-7.f, -7.f + 7.f * ballsAmount, (t >= 1.f ? 1.f : t));

		gluLookAt(X, 3, 0, X + 1, 2.0, 0.0, 0.0, 1.0, 1.0);
		for (int i = 0; i < ballsAmount; ++i)
		{
			if (i != 0)
				glTranslatef(7.f, 0.f, 0.f);
			glColor3f(1.f, 0.f, 0.f);
			glutWireSphere(2.0, 20, 20);
		}

		glutSwapBuffers();
	}

	void animate(int v)
	{
		if (isAnimate)
		{
			glutPostRedisplay();
			glutTimerFunc(animatePeriod, animate, 1);
		}
	}

	void setup(void)
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);
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
			animate(1);
		default:
			break;
		}
	}

	void printInteraction()
	{
		std::cout << "Interaction\n";
		std::cout << "Press space to start/stop animation\n";
	}

	int main(int argc, char** argv)
	{
		printInteraction();
		glutInit(&argc, argv);

		glutInitContextVersion(4, 3);
		glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
		glutInitWindowSize(500, 500);
		glutInitWindowPosition(100, 100);
		glutCreateWindow("LookAtBalls");
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

