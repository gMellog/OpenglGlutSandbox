#define _USE_MATH_DEFINES

#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <iostream>
#include <vector>
#include <chrono>

namespace SpinningCube
{
	static bool isWire{true};
	static int animationPeriod = 25;
	static float deltaTime;
	static int width = 500;
	static int height = 500;

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

		bool operator==(const Vector& rhs) const noexcept
		{
			return (X == rhs.X) && (Y == rhs.Y) && (Z == rhs.Z);
		}

		bool operator!=(const Vector& rhs) const noexcept
		{
			return !((*this) == rhs);
		}

		float X;
		float Y;
		float Z;

	};

	static const Vector ZeroVector;

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

	struct Cube
	{
		Cube(const Transform& pTransform)
			:
			transform{ pTransform },
			size{5.f},
			prevDir{},
			currDir{},
			angle{},
			t{},
			verticies{ fillVerticies() }
		{
		}

		void tick(float deltaTime) 
		{
			if (currDir != ZeroVector)
			{
				angle += 90.f * deltaTime;
			}

			draw();
		}

		void setSpinDir(const Vector& newDir)
		{
			angle = 0.f;

			if ((prevDir == ZeroVector) && (currDir == ZeroVector))
			{
				prevDir = currDir = newDir;
				t = 1.f;
			}
			else
			{
				prevDir = currDir;
				currDir = newDir;
				t = 0.f;
			}
		}

		Transform getTransform() const noexcept
		{
			return transform;
		}

	private:

		Transform transform;
		Vector prevDir;
		Vector currDir;
		float size;
		float angle;
		float t;

		std::vector<Vector> verticies;

		std::vector<Vector> cubeVerticies;

		std::vector<Vector> fillVerticies()
		{
			const auto& loc = transform.translation;
			const auto half = (size / 2) + 0.1f;

			return {
				{-half,-half, half},
				{ half,-half, half},
				{-half, half, half},
				{ half, half, half},
				{-half,-half, -half},
				{ half,-half, -half},
				{-half, half, -half},
				{ half, half, -half}
			};
		}

		void drawPoints()
		{
			glEnableClientState(GL_VERTEX_ARRAY);

			glPointSize(5.f);
			glColor3f(1.f, 1.f, 1.f);
			glVertexPointer(3, GL_FLOAT, 0, verticies.data());

			glDrawArrays(GL_POINTS, 0, verticies.size());

			glDisableClientState(GL_VERTEX_ARRAY);
		}

		void draw()
		{
			const auto& loc = transform.translation;
			const auto dir = currDir;

			glColor3f(1.f, 127 / 255.f, 80 / 255.f);

			glPushMatrix();
			
			glTranslated(loc.X, loc.Y, loc.Z);
			glRotated(angle, -dir.Y, dir.X, dir.Z);

			if (isWire)
				glutWireCube(size);
			else
				glutSolidCube(size);
			
			drawPoints();

			glPopMatrix();
		}

	};

	Cube createCube()
	{
		return { {{0.f, 0.f, -15.f}, {}} };
	}

	Cube cube{ createCube() };

	void tick(float deltaTime)
	{
		cube.tick(deltaTime);
	}

	void drawScene(void)
	{
		using namespace std::chrono;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		auto d = duration_cast<milliseconds>(system_clock::now() - tp);
		deltaTime = float(d.count()) * milliseconds::period::num / milliseconds::period::den;
		tp = system_clock::now();

		tick(deltaTime);
		glutSwapBuffers();
	}

	void animate(int value)
	{
		glutPostRedisplay();
		glutTimerFunc(animationPeriod, animate, 1);
	}

	void setup(void)
	{
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glEnable(GL_DEPTH_TEST);
		tp = std::chrono::system_clock::now();
		animate(1);
	}

	void resize(int w, int h)
	{
		glViewport(0, 0, w, h);
		width = w;
		height = h;
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
			isWire = !isWire;
			glutPostRedisplay();
			break;
		case 'r':
		case 'R':
			cube = createCube();
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

	void mouseCallback(int button, int pressed, int x, int y)
	{
		if ((button == GLUT_LEFT_BUTTON) && (pressed == GLUT_DOWN))
		{
			const float screenY = height - y;
			const auto cubeLocScreen = Vector{ 250.f, 250.f};
			const auto dirPoint = Vector{ x * 1.f, screenY};

			const auto newDir = normalize(dirPoint - cubeLocScreen);

			cube.setSpinDir(newDir);
			glutPostRedisplay();
		}
	}

	int main(int argc, char** argv)
	{
		printInteraction();
		glutInit(&argc, argv);

		glutInitContextVersion(4, 3);
		glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
		glutInitWindowSize(width, height);
		glutInitWindowPosition(100, 100);
		glutCreateWindow("SpinningCube");
		glutDisplayFunc(drawScene);
		glutReshapeFunc(resize);
		glutKeyboardFunc(keyInput);
		glutMouseFunc(mouseCallback);

		glewExperimental = GL_TRUE;
		glewInit();

		setup();

		glutMainLoop();

		return 0;
	}
}
