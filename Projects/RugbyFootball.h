///////////////////////////////////////////////////////////////////////////////////////          
// This program draws RugbyFootball
//
// Interaction:
// Press x, X, y, Y, z, Z to turn the lampshade.
/////////////////////////////////////////////////////////////////////////////////////// 

#define _USE_MATH_DEFINES 

#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>
#include <memory>

namespace RugbyFootball
{
	static float Xangle = 40.0, Yangle = 0.0, Zangle = 0.0;
	static float R0 = 20.f;
	static float R1 = 12.5f;
	static float R2 = 7.5f;
	static int N = 15;
	static int p = 12;
	static int q = 10;
	static std::vector<float> vertices;
	static std::vector<unsigned int*> indices;
	static std::vector<int> countIndices;

	void fillVertices()
	{
		vertices.reserve(3 * (2 * q + 1) * (p + 1));

		int j, i;
		for (j = -q; j <= q; j++)
		{
			//This way pushes vertices just in so later we can define their order with indices in fillIndecies procedure for example
			for (i = 0; i <= p; ++i)
			{
				auto fi = M_PI_2 * j / q;
				const auto th = 2.0 * i / p * M_PI;

				vertices.push_back(R0 * cos(fi) * cos(th));
				vertices.push_back(R1 * sin(fi));
				vertices.push_back(-R2 * cos(fi) * sin(th));
			}

			/* Push vertices as they go, so indices have to be just in 0,1,2,..., 4q(p + 1) order
			for (i = 0; i <= p; i++)
			{
				auto fi = M_PI_2 * (j + 1) / q;
				const auto th = 2.0 * i / p * M_PI;

				vertices.push_back(R0 * cos(fi) * cos(th));
				vertices.push_back(R1 * sin(fi));
				vertices.push_back(-R2 * cos(fi) * sin(th));

				fi = M_PI_2 * j / q;

				vertices.push_back(R0 * cos(fi) * cos(th));
				vertices.push_back(R1 * sin(fi));
				vertices.push_back(-R2 * cos(fi) * sin(th));
			}
			*/
		}
	}

	void fillIndices()
	{
		int j, i;
		indices.resize(2 * q);

		for (j = 0; j < indices.size(); j++)
		{
			indices[j] = new unsigned int[2 * (p + 1)];
			
			for (i = 0; i <= p; i++)
			{
				indices[j][2 * i] = ((j + 1) * (p + 1) + i);
				indices[j][2 * i + 1] = (j * (p + 1) + i);
			}
			
			//int k = 0;
			/*
			for (i = 0; i <= p; ++i)
			{
				indices[j][2 * i] = k;
				++k;
				indices[j][2 * i + 1] = k;
				++k;
			}
			*/
		}
	}

	void freeIndices()
	{
		for (auto i : indices)
		{
			if (i != nullptr)
			{
				delete[] i;
				i = nullptr;
			}
		}
	}

	void fillCountIndices()
	{
		countIndices.resize(indices.size());

		for (int i = 0; i < indices.size(); ++i)
		{
			countIndices[i] = 2 * (p + 1);
		}
	}

	void setup(void)
	{
		glEnableClientState(GL_VERTEX_ARRAY);
		fillVertices();
		fillIndices();
		fillCountIndices();
		glClearColor(1.0, 1.0, 1.0, 0.0);
	}


	void drawScene(void)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glLoadIdentity();

		glTranslatef(0.0, 0.0, -30.0);

		glRotatef(Zangle, 0.0, 0.0, 1.0);
		glRotatef(Yangle, 0.0, 1.0, 0.0);
		glRotatef(Xangle, 1.0, 0.0, 0.0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor3f(0.0, 0.0, 0.0);

		glVertexPointer(3, GL_FLOAT, 0, vertices.data());

		glMultiDrawElements(GL_TRIANGLE_STRIP, countIndices.data(), GL_UNSIGNED_INT, (const void**)indices.data(), indices.size());
		//glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size() / 3); easier

		glFlush();
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
			freeIndices();
			exit(0);
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

	void printInteraction(void)
	{
		std::cout << "Interaction:\n";
		std::cout << "Press x, X, y, Y, z, Z to turn the lampshade" << std::endl;
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
		glutCreateWindow("RugbyFootball");
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

