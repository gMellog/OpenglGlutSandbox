#define _USE_MATH_DEFINES

#include <iostream>
#include <cmath>
#include <GL/glew.h>
#include <GL/freeglut.h> 

namespace FlyingCameraOnSphere
{
	static bool isAnimate;
	float t;
	float animatePeriod = 100.f;
	const int ballsAmount = 10;
	
	const std::size_t p{30};
	const std::size_t q{30};

	struct Vector
	{
		Vector(float pX = 0.f, float pY = 0.f, float pZ = 0.f)
			:
			X{ pX },
			Y{ pY },
			Z{ pZ }
		{

		}

		float length() const noexcept
		{
			return std::sqrt(X * X + Y * Y + Z * Z);
		}

		Vector& operator+=(const Vector& rhs) noexcept
		{
			return *this = *this + rhs;;
		}

		Vector& operator-=(const Vector& rhs) noexcept
		{
			return *this = *this - rhs;;
		}

		Vector operator+(const Vector& rhs) const noexcept
		{
			return { X + rhs.X, Y + rhs.Y, Z + rhs.Z };
		}

		Vector operator-(const Vector& rhs) const noexcept
		{
			return { X - rhs.X, Y - rhs.Y, Z - rhs.Z };
		}

		Vector operator*(float v) const noexcept
		{
			return { X * v, Y * v, Z * v };
		}

		float X;
		float Y;
		float Z;
	};

	Vector operator*(float scalar, const Vector& rhs)
	{
		return rhs * scalar;
	}

	std::ostream& operator<<(std::ostream& os, const Vector& v)
	{
		os << "X: " << v.X << " Y: " << v.Y << " Z: " << v.Z;
		return os;
	}

	float scalarProduct(const Vector& lhs, const Vector& rhs)
	{
		return lhs.X * rhs.X + lhs.Y * rhs.Y + lhs.Z * rhs.Z;
	}

	Vector perpOfScalarProduct(const Vector& on, const Vector& of)
	{
		return of - (scalarProduct(on, of) / (on.length() * of.length())) * on;
	}

	Vector normalize(const Vector& v)
	{
		return v * (1.f / v.length());
	}

	template<typename T>
	T clamp(T min, T max, T val)
	{
		return min > val ? min : val > max ? max : val;
	}

	struct Camera
	{
		explicit Camera(float pR)
			:
			R{ pR },
			RStep{0.25f},
			RMin{10.f},
			RMax{30.f},
			center{},
			i{},
			j{}
		{
		}

		void tick() noexcept
		{
			const auto loc = getNextCoord(i, j);
			const auto upDir = getUpDir();

			gluLookAt(loc.X, loc.Y, loc.Z, center.X, center.Y, center.Z, upDir.X, upDir.Y, upDir.Z);

			tryToRecalcCoords();
		}

		void specialKeys(int key, int x, int y) noexcept
		{
			if (key == GLUT_KEY_LEFT)
			{
				i -= 1;
				glutPostRedisplay();
			}
			else if (key == GLUT_KEY_RIGHT)
			{
				i += 1;
				glutPostRedisplay();
			}
			else if (key == GLUT_KEY_UP)
			{
				j += 1;
				glutPostRedisplay();
			}
			else if (key == GLUT_KEY_DOWN)
			{
				if (j == 0)
				{
					j = q;
				}

				j -= 1;

				glutPostRedisplay();
			}
		}

		void mouseWheel(int dir) noexcept
		{
			R = clamp(RMin, RMax, R + RStep * dir * -1.f);
			glutPostRedisplay();
		}

	private:

		float R;
		float RStep;
		float RMin;
		float RMax;

		Vector center;

		int i;
		int j;

		Vector getNextCoord(int nextI, int nextJ) const noexcept
		{
			const float fi = 2.0 * M_PI * nextJ / q;
			const float th = 2.0 * M_PI * nextI / p;

			return { R * cos(fi) * cos(th), R * sin(fi), R * cos(fi) * sin(th) };
		}

		bool inRange(float min, float max, float v) const noexcept
		{
			return (min <= v && v <= max);
		}

		Vector getUpDir() const noexcept
		{
			const float fi = 2.0 * M_PI * j / q;
			const auto lookDown = Vector{ 0.f, -1.f, 0.f };
			const auto lookUp = Vector{ 0.f, 1.f, 0.f };

			return inRange(M_PI_2, 3 * M_PI_2, fi) ? lookDown : lookUp;
		}

		void tryToRecalcCoords()
		{
			const float fi = 2.0 * M_PI * j / q;
			const float th = 2.0 * M_PI * i / p;

			if (fi >= 2.0 * M_PI)
				j -= q;

			if (th >= 2.0 * M_PI)
				i -= p;
		}

	} camera{ 15.f };

	void drawScene(void)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glColor3f(0.0, 0.0, 0.0);
		glLoadIdentity();
		
		camera.tick();
		glColor3f(1.f, 0.f, 0.f);

		glRotatef(90.f, 0.f, 1.f, 0.f);
		glutWireTeapot(5.f);

		glutSwapBuffers();
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
		default:
			break;
		}
	}

	void specialCallback(int key, int x, int y)
	{
		camera.specialKeys(key, x, y);
	}

	void mouseWheelCallback(int button, int dir, int x, int y)
	{
		camera.mouseWheel(dir);
	}

	void printInteraction()
	{
		std::cout << "Interaction:\n";
		std::cout << "Rotate camera with left/right/up/down arrow keys respectively\n";
		std::cout << "And of course you can get closer/farther with mouse wheel\n";
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
		glutCreateWindow("FlyingCameraOnSphere");
		glutDisplayFunc(drawScene);
		glutReshapeFunc(resize);
		glutKeyboardFunc(keyInput);
		glutSpecialFunc(specialCallback);
		glutMouseWheelFunc(mouseWheelCallback);

		glewExperimental = GL_TRUE;
		glewInit();

		setup();

		glutMainLoop();

		return 0;
	}
}

