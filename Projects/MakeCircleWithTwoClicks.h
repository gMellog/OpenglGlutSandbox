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
			X{ pX },
			Y{ pY }
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

	template<typename T>
	T clamp(const T& min, const T& max, const T& value)
	{
		return value <= min ? min : value >= max ? max : value;
	}

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
			fillCenter{ true },
			inPassiveMode{},
			passiveOpen{ true },
			wheelFirstTime{ true },
			minR{5.f},
			maxR{100.f},
			wheelOffset{1.f}
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
				Point2D p{ static_cast<float>(x), static_cast<float>(height - y) };
				if (fillCenter)
				{
					temp.center = p;
				}
				else
				{
					inPassiveMode = false;
				}

				fillCenter = !fillCenter;
			}
			else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
			{
				if (passiveOpen)
				{
					wheelFirstTime = true;
				}

				passiveOpen = !passiveOpen;
			}
		}

		void passiveMouseMotion(int x, int y)
		{
			if (passiveOpen & (!fillCenter))
			{
				Point2D p{ static_cast<float>(x), static_cast<float>(height - y) };

				temp.circlePoint = p;

				if (!inPassiveMode)
				{
					inPassiveMode = true;
				}
				else
				{
					eraseLastCircle();
				}

				addCircle();
				glutPostRedisplay();
			}
		}

		void mouseWheel(int wheel, int direction, int x, int y)
		{
			if (!passiveOpen)
			{
				if (fillCenter && wheelFirstTime)
				{
					temp.center.X = x;
					temp.center.Y = height - y;
				}
				
				Point2D p{ temp.center.X + minR, temp.center.Y};

				temp.circlePoint = p;

				if (wheelFirstTime)
				{
					wheelFirstTime = false;
				}
				else
				{
					temp.circlePoint = circles.back().circlePoint;
					temp.circlePoint.X = clamp(temp.center.X + minR, temp.center.X + maxR, temp.circlePoint.X + wheelOffset * direction);
					
					eraseLastCircle();
				}

				addCircle();
				glutPostRedisplay();
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
		bool inPassiveMode;
		bool passiveOpen;
		bool wheelFirstTime;

		float minR;
		float maxR;

		float wheelOffset;



		void eraseLastCircle()
		{
			circles.erase(std::end(circles) - 1);
			const auto end = vertices.cend();

			const auto it = (end - (3 * (N + 1)));

			vertices.erase(end - (3 * (N + 1)), end);
			firsts.erase(std::end(firsts) - 1);
			counts.erase(std::end(counts) - 1);
		}

		void addCircle()
		{
			circles.push_back(temp);
			fillCircle(temp.center, temp.circlePoint);
			fillFirst(circles.size() - 1);
			fillCount();
		}

		void fillCircle(Point2D center, Point2D circlePoint)
		{
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

		void fillFirst(int i)
		{
			firsts.push_back(i * (N + 1));
		}

		void fillCount()
		{
			counts.push_back(N + 1);
		}

		void fillVertices()
		{
			vertices.reserve(circles.size() * 3 * (N + 1));
			for (auto it = std::begin(circles); it != std::end(circles); it += 1)
			{
				fillCircle(it->center, it->circlePoint);
			}

			vertices.shrink_to_fit();
		}

		void fillFirsts()
		{
			firsts.reserve(circles.size());

			for (int i = 0; i < circles.size(); ++i)
			{
				fillFirst(i);
			}

			firsts.shrink_to_fit();
		}

		void fillCounts()
		{
			counts.reserve(circles.size());

			for (int i = 0; i < circles.size(); ++i)
			{
				fillCount();
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

	void passiveMouseMotion(int x, int y)
	{
		master.passiveMouseMotion(x, y);
	}

	void mouseWheel(int wheel, int direction, int x, int y)
	{
		master.mouseWheel(wheel, direction, x, y);
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
		glutPassiveMotionFunc(passiveMouseMotion);
		glutMouseWheelFunc(mouseWheel);

		glewExperimental = GL_TRUE;
		glewInit();

		setup();

		glutMainLoop();

		return 0;
	}
}