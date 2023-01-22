///////////////////////////////////////////////////////////////////////////////////////////        
//  This program draws BullSEye with help of glMultiDrawArrays()
/////////////////////////////////////////////////////////////////////////////////////////// 

#define _USE_MATH_DEFINES 

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <vector>

namespace BullSEye
{
	static int N = 40;
	static std::vector<float> verticesAndColors;
	static std::vector<int> firsts;
	static std::vector<int> counts;

	void collectVerticesForDisc(float R0, float X, float Y, float Z, float R, float G, float B)
	{
		float t;
		int i;

		verticesAndColors.push_back(X);
		verticesAndColors.push_back(Y);
		verticesAndColors.push_back(Z);

		verticesAndColors.push_back(R);
		verticesAndColors.push_back(G);
		verticesAndColors.push_back(B);


		for (i = 0; i <= N; ++i)
		{
			t = 2 * M_PI * i / N;

			verticesAndColors.push_back(X + R0 * cos(t));
			verticesAndColors.push_back(Y + R0 * sin(t));
			verticesAndColors.push_back(Z);

			verticesAndColors.push_back(R);
			verticesAndColors.push_back(G);
			verticesAndColors.push_back(B);
		}
	}

	void fillVertices()
	{
		collectVerticesForDisc(20.0, 50.0, 50.0, 0.0, 1.f, 0.f, 0.f);
		collectVerticesForDisc(50.0 / 3.0, 50.0, 50.0, 0.25, 1.f, 1.f, 1.f);
		collectVerticesForDisc(40.0 / 3.0, 50.0, 50.0, 0.5, 1.f, 0.f, 0.f);
		collectVerticesForDisc(10.0, 50.0, 50.0, 0.75, 1.f, 1.f, 1.f);
		collectVerticesForDisc(5.0, 50.0, 50.0, 0.76, 1.f, 0.f, 0.f);
	}

	void fillFirsts()
	{
		for (int i = 0; i < 6; ++i)
		{
			firsts.push_back((N + 2) * i);
		}
	}

	void fillCounts()
	{
		for (int i = 0; i < 5; ++i)
		{
			counts.push_back(N + 2);
		}
	}

	void drawScene(void)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT, GL_FILL);

		glEnable(GL_DEPTH_TEST);

		glMultiDrawArrays(GL_TRIANGLE_FAN, firsts.data(), counts.data(), counts.size());

		glDisable(GL_DEPTH_TEST);

		glFlush();
	}

	void setup(void)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);

		fillVertices();
		fillFirsts();
		fillCounts();

		glVertexPointer(3, GL_FLOAT, 6 * sizeof(float), verticesAndColors.data());
		glColorPointer(3, GL_FLOAT, 6 * sizeof(float), verticesAndColors.data() + 3);
		glClearColor(1.0, 1.0, 1.0, 0.0);
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

		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
		glutInitWindowSize(500, 500);
		glutInitWindowPosition(100, 100);
		glutCreateWindow("bullSEye");
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