/////////////////////////////////////////////////////////////////////         
// Interaction:
// Click two points on Opengl window, first for center of the circle,
// second for point on the circle
///////////////////////////////////////////////////////////////////// 

#define _USE_MATH_DEFINES 

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <vector>

namespace MakeCircleWithTwoClicks
{
	static int N = 30;
	static unsigned int width = 500;
	static unsigned int height = 500;

	struct Point2D
	{
		Point2D(float pX = 0.f, float pY = 0.f)
			:
			X{pX},
			Y{pY}
		{

		}

		float X;
		float Y;
	};

	struct Circle
	{
		Point2D center;
		Point2D circlePoint;
	};

	struct MasterVertices
	{
		std::vector<float> vertices;
		std::vector<int> firsts;
		std::vector<int> counts;
		std::vector<Circle> circles;

		MasterVertices()
			:
			vertices{},
			firsts{},
			counts{},
			circles{},
			temp{},
			fillCenter{true}
		{

		}

		void drawScene()
		{
			if (!vertices.empty())
			{
				glVertexPointer(3, GL_FLOAT, 0, vertices.data());
				glMultiDrawArrays(GL_LINE_STRIP, firsts.data(), counts.data(), circles.size());
			}
		}

		void removeEverything()
		{
			vertices.clear();
			firsts.clear();
			counts.clear();
		}

		void drawVertices()
		{
			fillVertices();
			fillFirsts();
			fillCounts();
		}

		void mouseControl(int button, int state, int x, int y)
		{
			if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
			{
				Point2D p{static_cast<float>(x), static_cast<float>(height - y)};
				if (fillCenter)
				{
					temp.center = p;
				}
				else
				{
					temp.circlePoint = p;
					addCircle();
					glutPostRedisplay();
				}

				fillCenter = !fillCenter;
			}
		}

		void keyInput(char key, int x, int y)
		{
			switch (key)
			{
			case 27:
				exit(0);
				break;
			case '+':
				if (!vertices.empty())
				{
					N++;
					removeEverything();
					drawVertices();
					glutPostRedisplay();
				}
				break;
			case '-':
				if (!vertices.empty() && N > 3)
				{
					N--;
					removeEverything();
					drawVertices();
					glutPostRedisplay();
				}
				break;
			case 'x':
			case 'X':
				if (!vertices.empty())
				{
					master.removeEverything();
					circles.clear();
					glutPostRedisplay();
				}
			default:
				break;
			}
		}
		
	private:

		Circle temp;
		bool fillCenter;
		
		void addCircle()
		{
			circles.push_back(temp);

			removeEverything();
			drawVertices();
		}
		
		void fillVertices()
		{
			vertices.reserve(circles.size() * (N + 1) * 3);
			for (auto it = std::begin(circles); it != std::end(circles); it += 1)
			{
				const auto center = it->center;
				const auto circlePoint = it->circlePoint;

				const float RX = center.X - circlePoint.X;
				const float RY = center.Y - circlePoint.Y;

				const auto R = sqrt(RX * RX + RY * RY);

				float t;
				for (int i = 0; i <= N; ++i)
				{
					t = 2 * M_PI * i / N;
					vertices.push_back(center.X + R * cos(t));
					vertices.push_back(center.Y + R * sin(t));
					vertices.push_back(0.0);
				}
			}

			vertices.shrink_to_fit();
		}

		void fillFirsts()
		{
			firsts.resize(circles.size());

			for (int i = 0; i < firsts.size(); ++i)
			{
				firsts[i] = i * (N + 1);
			}

			firsts.shrink_to_fit();
		}

		void fillCounts()
		{
			counts.resize(circles.size());

			for (int i = 0; i < counts.size(); ++i)
			{
				counts[i] = N + 1;
			}

			counts.shrink_to_fit();
		}

	} master;


	void drawScene(void)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glColor3f(0.f, 0.f, 0.f);
	
		master.drawScene();

		glFlush();
	}

	void setup(void)
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glEnableClientState(GL_VERTEX_ARRAY);
	}

	void resize(int w, int h)
	{
		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		width = w;
		height = h;
		glOrtho(0.0, width, 0.0, height, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	void keyInput(unsigned char key, int x, int y)
	{
		master.keyInput(key, x, y);
	}

	void printInteraction(void)
	{
		std::cout << "Interaction:" << std::endl;
		std::cout << "1. Click two points on Opengl window, first for center of the circle, second for point on the circle\n";
		std::cout << "2. Press +/- to increase/decrease the number of vertices on the circle." << std::endl;
		std::cout << "3. Press x / X to decline circle that you drew\n";
		std::cout << "3*. Or you can simple select new two points (calculated)\n";
	}

	void mouseControl(int button, int state, int x, int y)
	{
		master.mouseControl(button, state, x, y);
	}

	int main(int argc, char** argv)
	{
		printInteraction();
		glutInit(&argc, argv);

		glutInitContextVersion(4, 3);
		glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
		glutInitWindowSize(width, height);
		glutInitWindowPosition(100, 100);
		glutCreateWindow("TwoClicksCircle");
		glutDisplayFunc(drawScene);
		glutReshapeFunc(resize);
		glutKeyboardFunc(keyInput);
		glutMouseFunc(mouseControl);

		glewExperimental = GL_TRUE;
		glewInit();

		setup();

		glutMainLoop();

		return 0;
	}
}