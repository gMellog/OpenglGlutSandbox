#define _USE_MATH_DEFINES

#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <iostream>
#include <vector>
#include <chrono>

namespace SquareToLine
{
	static bool isAnimate;
	static int animationPeriod = 25;
	static float deltaTime;
	static float t;
	std::chrono::system_clock::time_point tp;

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

		Vector& operator*=(float v) noexcept
		{
			return *this = *this * v;
		}

		float X;
		float Y;
		float Z;
	};

	bool isNearlyEqual(float a, float b, float e)
	{
		return (a - b) < e;
	}

	Vector normalize(const Vector& v)
	{
		return { v.X / v.length(), v.Y / v.length(), v.Z / v.length() };
	}

	Vector operator* (float lhs, const Vector& rhs)
	{
		return rhs * lhs;
	}

	std::ostream& operator<<(std::ostream& os, const Vector& v)
	{
		os << "X: " << v.X << " Y: " << v.Y << " Z: " << v.Z;
		return os;
	}

	struct Rotation
	{
		Rotation(float pAngle = 0, const Vector& pDirs = Vector{})
			:
			angle{ pAngle },
			dirs{ pDirs }
		{

		}

		float angle;
		Vector dirs;
	};

	struct Transform
	{
		Transform(const Vector& pTranslation,
			const Vector& pScale,
			const Rotation& pRotation = Rotation{}
		)
			:
			translation{ pTranslation },
			scale{ pScale },
			rotation{ pRotation }
		{

		}

		Vector translation;
		Vector scale;
		Rotation rotation;
	};

	template<typename T>
	T clamp(T min, T max, T val)
	{
		return min > val ? min : val > max ? max : val;
	}
	
	struct LineSegment
	{
		explicit LineSegment(const Vector& pStart, const Vector& pEnd)
			:
			vertices{ pStart, pEnd }
		{
		}

		void draw(float angle)
		{
			glColor3f(0.f, 0.f, 0.f);
			glEnableClientState(GL_VERTEX_ARRAY);

			glLineWidth(5.f);
			glRotated(angle, 0.f, 0.f, 1.f);
			glVertexPointer(3, GL_FLOAT, 0, vertices.data());

			glDrawArrays(GL_LINES, 0, vertices.size());

			glDisableClientState(GL_VERTEX_ARRAY);
		}

	private:
		std::vector<Vector> vertices;
	};

	std::vector<LineSegment> lines;

	template<typename T1, typename T2>
	T1 lerp(T1 a, T1 b, T2 t)
	{
		return (1 - t)* a + t * b;
	}

	void tick()
	{
		const auto angle = lerp(-90.f, 0.f, t);
		const auto translateX = lerp(9.65f, 10.f, t);

		const auto newAngle = isNearlyEqual(std::abs(angle), 0.f, 1.f) ? 0.f : angle;

		glTranslated(-20.f, -20.f, -30.f);
		for (int i = 0; i < lines.size(); ++i)
		{
			lines[i].draw((i > 0) * newAngle);
			glTranslated(0.f, translateX, 0.f);
		}
	}

	void drawScene(void)
	{
		using namespace std::chrono;

		glClear(GL_COLOR_BUFFER_BIT);
		glLoadIdentity();

		auto d = duration_cast<milliseconds>(system_clock::now() - tp);
		deltaTime = float(d.count()) * milliseconds::period::num / milliseconds::period::den;
		tp = system_clock::now();

		tick();
		glutSwapBuffers();
	}

	void animate(int value)
	{
		if (isAnimate)
		{
			t += 0.01f;
			if (t >= 1.f)
				isAnimate = false;

			glutPostRedisplay();
			glutTimerFunc(animationPeriod, animate, 1);
		}
	}

	void initActors()
	{
		const auto lineSegment = 
			LineSegment(
			{ 0.f, 0.f, 0.f },
			{ 0.f, 10.f, 0.f }
			);

		lines.push_back(lineSegment);
		lines.push_back(lineSegment);
		lines.push_back(lineSegment);
		lines.push_back(lineSegment);
	}

	void setup(void)
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);
		tp = std::chrono::system_clock::now();
		initActors();
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
			if (t >= 1.f) return;

			isAnimate = !isAnimate;

			if (isAnimate)
			{
				tp = std::chrono::system_clock::now();
				animate(1);
			}
			break;
		case 'r':
		case 'R':
			t = 0.f;
			glutPostRedisplay();
			break;
		default:
			break;
		}
	}

	void printInteraction(void)
	{
		std::cout << "Interaction:" << std::endl;
		std::cout << "Press space to toggle between animation on and off.\n"
			<< "Press r to start animation again\n";
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
		glutCreateWindow("SquareToLine");
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
