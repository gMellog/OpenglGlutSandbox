///////////////////////////////////////////////////////////////////////////////////////          
//
// This program approximates a sphere with triangle strips.
//
// Interaction:
// Press P/p to increase/decrease the number of longitudinal slices.
// Press Q/q to increase/decrease the number of latitudinal slices.
// Press x, X, y, Y, z, Z to turn the sphere.
//
/////////////////////////////////////////////////////////////////////////////////////// 

#define _USE_MATH_DEFINES 

#include <cmath>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

namespace Sphere
{
	static float R = 5.0;
	static int p = 6;
	static int q = 4;
	static float Xangle = 0.0, Yangle = 0.0, Zangle = 0.0;

	struct SphereHandler
	{
		virtual ~SphereHandler() = default;
		virtual void setup() = 0;
		virtual void drawScene() = 0;
		virtual void pOrQChanged() = 0;
	};
	
	struct ImmediateHandler : SphereHandler
	{
		void setup() override
		{

		}

		void drawScene() override
		{
			int j, i;
			for (j = -q; j < q; j++)
			{
				glBegin(GL_TRIANGLE_STRIP);
				for (i = 0; i <= p; i++)
				{
					auto fi = M_PI_2 * (j + 1) / q;
					const auto th = 2.0 * i / p * M_PI;

					glVertex3f(R * cos(fi) * cos(th), R * sin(fi), -R * cos(fi) * sin(th));

					fi = M_PI_2 * j / q;

					glVertex3f(R * cos(fi) * cos(th), R * sin(fi), -R * cos(fi) * sin(th));

				}
				glEnd();
			}
		}

		void pOrQChanged() override
		{

		}
	};

	struct DrawElementsHandler : SphereHandler
	{
		void setup() override
		{
			glEnableClientState(GL_VERTEX_ARRAY);
		}

		void drawScene() override
		{
			int j, i, index;
			for (j = -q; j < q; j++)
			{
				std::vector<float> vertices;
				vertices.reserve((p + 1) * 3);

				std::vector<unsigned int> indices;
				indices.reserve(p + 1);

				for (i = 0, index = 0; i <= p; i++, index += 2)
				{
					auto fi = M_PI_2 * (j + 1) / q;
					const auto th = 2.0 * i / p * M_PI;

					vertices.push_back(R * cos(fi) * cos(th));
					vertices.push_back(R * sin(fi));
					vertices.push_back(-R * cos(fi) * sin(th));

					indices.push_back(index);

					fi = M_PI_2 * j / q;

					vertices.push_back(R * cos(fi) * cos(th));
					vertices.push_back(R * sin(fi));
					vertices.push_back(-R * cos(fi) * sin(th));

					indices.push_back(index + 1);
				}

				glVertexPointer(3, GL_FLOAT, 0, vertices.data());
				glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, indices.data());
			}
		}

		void pOrQChanged() override
		{

		}
	};

	struct DrawArraysHandler : SphereHandler
	{
		void setup() override
		{
			fillVertices();
			glEnableClientState(GL_VERTEX_ARRAY);
		}

		void drawScene() override
		{
			glVertexPointer(3, GL_FLOAT, 0, vertices.data());
			glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size() / 3);
		}

		void pOrQChanged() override
		{
			clearVertices();
			fillVertices();
		}

		private:

		void fillVertices()
		{
			int j, i;
			for (j = -q; j < q; j++)
			{
				for (i = 0; i <= p; i++)
				{
					auto fi = M_PI_2 * (j + 1) / q;
					const auto th = 2.0 * i / p * M_PI;

					vertices.push_back(R * cos(fi) * cos(th));
					vertices.push_back(R * sin(fi));
					vertices.push_back(-R * cos(fi) * sin(th));

					fi = M_PI_2 * j / q;

					vertices.push_back(R * cos(fi) * cos(th));
					vertices.push_back(R * sin(fi));
					vertices.push_back(-R * cos(fi) * sin(th));

				}
			}
		}
		
		void clearVertices()
		{
			vertices.erase(std::begin(vertices), std::end(vertices));
		}

		std::vector<float> vertices;
	};

	struct ListsHandler : SphereHandler
	{
		void setup() override
		{
			initList(true);
		}

		void drawScene() override
		{
			glCallList(hemSphere);

			glScalef(1.f, -1.f, 1.f);
			glCallList(hemSphere);
		}

		void pOrQChanged() override
		{
			initList(false);
		}

		private:

		void initList(bool initial = true)
		{
			if(initial)
				hemSphere = glGenLists(1);
			
			glNewList(hemSphere, GL_COMPILE);
			int j, i;
			for (j = 0; j < q; j++)
			{
				glBegin(GL_TRIANGLE_STRIP);
				for (i = 0; i <= p; i++)
				{
					auto fi = M_PI_2 * (j + 1) / q;
					const auto th = 2.0 * i / p * M_PI;

					glVertex3f(R * cos(fi) * cos(th), R * sin(fi), -R * cos(fi) * sin(th));

					fi = M_PI_2 * j / q;

					glVertex3f(R * cos(fi) * cos(th), R * sin(fi), -R * cos(fi) * sin(th));

				}
				glEnd();
			}
			glEndList();
		}
		unsigned int hemSphere;

	};

	static std::unique_ptr<SphereHandler> sphereHandler;

	// Initialization routine.
	void setup(void)
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);

		sphereHandler = std::make_unique<ListsHandler>();
		sphereHandler->setup();
	}

	// Drawing routine.
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
		
		if (sphereHandler)
			sphereHandler->drawScene();
		
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
		case 'P':
			p += 1;
			if(sphereHandler)
				sphereHandler->pOrQChanged();
			glutPostRedisplay();
			break;
		case 'p':
			if (p > 3)
			{
				p -= 1;
				if (sphereHandler)
					sphereHandler->pOrQChanged();
				glutPostRedisplay();
			}
			break;
		case 'Q':
			q += 1;
			if (sphereHandler)
				sphereHandler->pOrQChanged();
			glutPostRedisplay();
			break;
		case 'q':
			if (q > 3) 
			{
				q -= 1;
				if (sphereHandler)
					sphereHandler->pOrQChanged();
				glutPostRedisplay();
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

	void printInteraction(void)
	{
		std::cout << "Interaction:" << std::endl;
		std::cout << "Press P/p to increase/decrease the number of longitudinal slices." << std::endl
			<< "Press Q/q to increase/decrease the number of latitudinal slices." << std::endl
			<< "Press x, X, y, Y, z, Z to turn the Sphere." << std::endl;
	}

	// Main routine.
	int main(int argc, char** argv)
	{
		printInteraction();
		glutInit(&argc, argv);

		glutInitContextVersion(4, 3);
		glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
		glutInitWindowSize(500, 500);
		glutInitWindowPosition(100, 100);
		glutCreateWindow("Sphere");
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