/////////////////////////////////////////////////////////////////////         
// This program draws a circle with glDrawArrays()
//
// Interaction:
// Press +/- to increase/decrease the number of vertices of the loop. 
///////////////////////////////////////////////////////////////////// 

#define _USE_MATH_DEFINES 

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <vector>

namespace Circle
{
	static float R = 40.0;
	static float X = 50.0; 
	static float Y = 50.0; 
	static int N = 3; 

	static std::vector<float> vertices;
	static std::vector<std::pair<int, int>> stippleModes;
	static int currLineModeIndex = 0;

	void initStippleModes()
	{
		stippleModes =
		{
			{0, 0xFFFF},
			{1, 0x5555},
			{1, 0x0101},
			{1, 0x00FF},
			{5, 0x5555}
		};
	}

	void clearVertices()
	{
		vertices.erase(std::begin(vertices), std::end(vertices));
	}

	void fillVertices()
	{
		vertices.reserve((N + 1) * 3);

		float t;
		for (int i = 0; i <= N; ++i)
		{
			t = 2 * M_PI * i / N;
			vertices.push_back(X + R * cos(t));
			vertices.push_back(Y - R * sin(t));
			vertices.push_back(0.0);
		}

		vertices.shrink_to_fit();
	}

	void drawScene(void)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glColor3f(0.f, 0.f, 0.f);
		glEnable(GL_LINE_STIPPLE);
		const auto& lineMode = stippleModes[currLineModeIndex];
		glLineStipple(lineMode.first, lineMode.second);

		glVertexPointer(3, GL_FLOAT, 0, vertices.data());
		glDrawArrays(GL_LINE_STRIP, 0, vertices.size() / 3);
		glDisable(GL_LINE_STIPPLE);

		glFlush();
	}

	void setup(void)
	{
		initStippleModes();
		fillVertices();
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glEnableClientState(GL_VERTEX_ARRAY);
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
		case '+':
			N++;
			clearVertices();
			fillVertices();
			glutPostRedisplay();
			break;
		case '-':
			if (N > 3)
			{
				N--;
				clearVertices();
				fillVertices();
				glutPostRedisplay();
			}
			break;
		case ' ':
		{
			++currLineModeIndex;
			const auto lastIndex = stippleModes.size() - 1;

			if (currLineModeIndex > lastIndex)
			{
				currLineModeIndex = 0;
			}

			glutPostRedisplay();
		}
			break;
		default:
			break;
		}
	}

	void printInteraction(void)
	{
		std::cout << "Interaction:" << std::endl;
		std::cout << "Press +/- to increase/decrease the number of vertices on the circle." << std::endl;
		std::cout << "Press spacebar if you want switch between line modes\n";
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
		glutCreateWindow("circle");
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