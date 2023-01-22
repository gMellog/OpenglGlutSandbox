//////////////////////////////////////////////////////////////////////////////////////          
// This program draws sphere as two hemispheres with using two VAOs with VBOs inside
// 
// Interaction:
// Press x, X, y, Y, z, Z to turn the sphere.
////////////////////////////////////////////////////////////////////////////////////// 

#define _USE_MATH_DEFINES 

#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <vector>

namespace VAOSphere
{
	enum HSPH
	{
		UP,
		DOWN
	};

	enum ELEMS
	{
		VERTICES,
		INDICES
	};

	static float RADIUS = 5.f;
	static const int LONG_SLICES = 20;
	static const int LAT_SLICES = 10;
	static float Xangle = 0.0, Yangle = 0.0, Zangle = 0.0;
	static float vertices[3 * (LONG_SLICES + 1) * (LAT_SLICES + 1)];
	static unsigned int indices[LAT_SLICES][2 * (LONG_SLICES + 1)];
	static int countIndices[LAT_SLICES]; 
	void* offsets[LAT_SLICES];
	static unsigned int buffer[2];
	static unsigned int VAOs[2];

	void fillVertices(HSPH hsph)
	{
		int i, j, k, until;
		j = hsph == HSPH::UP ? 0 : -LAT_SLICES;
		until = hsph == HSPH::UP ? LAT_SLICES : 0;

		k = 0;
		for (; j <= until; j++)
			for (i = 0; i <= LONG_SLICES; i++)
			{
				vertices[k++] = RADIUS * cos((float)j / LAT_SLICES * M_PI / 2.0) * cos(2.0 * (float)i / LONG_SLICES * M_PI);
				vertices[k++] = RADIUS * sin((float)j / LAT_SLICES * M_PI / 2.0);
				vertices[k++] = -RADIUS * cos((float)j / LAT_SLICES * M_PI / 2.0) * sin(2.0 * (float)i / LONG_SLICES * M_PI);
			}
	}

	void fillIndices()
	{
		int i, j;
		for (j = 0; j < LAT_SLICES; j++)
		{
			for (i = 0; i <= LONG_SLICES; i++)
			{
				indices[j][2 * i] = (j + 1) * (LONG_SLICES + 1) + i;
				indices[j][2 * i + 1] = j * (LONG_SLICES + 1) + i;
			}
		}
	}

	void fillCountIndices(void)
	{
		int j;
		for (j = 0; j < LAT_SLICES; j++) countIndices[j] = 2 * (LONG_SLICES + 1);
	}

	void fillOffsets(void)
	{
		int j;
		for (j = 0; j < LAT_SLICES; j++) offsets[j] = (void*)(2 * (LONG_SLICES + 1) * j * sizeof(unsigned int));
	}

	void initVAO(HSPH hsph)
	{
		glBindVertexArray(VAOs[hsph]);

		fillVertices(hsph);
		fillIndices();

		glGenBuffers(2, buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer[VERTICES]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[INDICES]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, 0);
	}

	void setup(void)
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);

		glGenVertexArrays(2, VAOs);

		initVAO(HSPH::UP);
		initVAO(HSPH::DOWN);

		fillOffsets();
		fillCountIndices();
	}

	void drawScene(void)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glLoadIdentity();

		glTranslatef(0.0, 0.0, -10.0);

		glRotatef(Zangle, 0.0, 0.0, 1.0);
		glRotatef(Yangle, 0.0, 1.0, 0.0);
		glRotatef(Xangle, 1.0, 0.0, 0.0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor3f(0.0, 0.0, 0.0);

		glBindVertexArray(VAOs[HSPH::UP]);
		glMultiDrawElements(GL_TRIANGLE_STRIP, countIndices, GL_UNSIGNED_INT, (const void**)offsets, LAT_SLICES);

		glBindVertexArray(VAOs[HSPH::DOWN]);
		glMultiDrawElements(GL_TRIANGLE_STRIP, countIndices, GL_UNSIGNED_INT, (const void**)offsets, LAT_SLICES);

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
		std::cout << "Interaction:" << std::endl;
		std::cout << "Press x, X, y, Y, z, Z to turn the sphere" << std::endl;
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
		glutCreateWindow("sphereWithVAOs");
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