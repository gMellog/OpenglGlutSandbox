
#define _USE_MATH_DEFINES

#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <iostream>
#include <vector>
#include <functional>
#include <chrono>
#include <map>
#include <memory>

namespace BallBouncingUp
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

	struct Actor
	{
		virtual ~Actor() = default;
		virtual void tick(float deltaTime) = 0;
		virtual void setTransform(const Transform& newTransform) = 0;
		virtual Transform getTransform() const = 0;

		std::vector<std::string> tags;
	};

	std::vector<std::unique_ptr<Actor>> actors;

	struct Box : Actor
	{
		Box(const Transform& pTransform, float pSize)
			:
			transform{ pTransform },
			size{pSize},
			path{5.f},
			moveSystem{}
		{
			
		}

		void tick(float deltaTime) override
		{
			if (isAnimate)
			{
				moveSystem = path * sin(t);
			}

			draw();
		}

		void setTransform(const Transform& newTransform) override
		{
			transform = newTransform;
		}

		Transform getTransform() const override
		{
			return transform;
		}

	private:
		Transform transform;
		float size;
		float path;
		float moveSystem;

		void draw()
		{
			const auto& loc = transform.translation;
			const auto& scale = transform.scale;

			glColor3d(0.0, 0.0, 0.0);
			
			glTranslated(moveSystem, 0.f, 0.f);

			glPushMatrix();
			
			glTranslated(loc.X, loc.Y, loc.Z);
			glScaled(scale.X, scale.Y, scale.Z);
			glutWireCube(10.f);

			glPopMatrix();
		}
	};

	struct Ball : Actor
	{
		explicit Ball(const Transform& pTransform)
			:
			transform{ pTransform },
			distance{10.f},
			radius{ 4.f },
			movedPath{distance}
		{
		}

		void tick(float deltaTime) override
		{
			if (isAnimate)
			{
				movedPath = std::abs(distance * cosf(t));
			}

			draw();
		}

		void setTransform(const Transform& newTransform) override
		{
			transform = newTransform;
		}

		Transform getTransform() const override
		{
			return transform;
		}

		static const std::string collideType;

		float getRadius() const noexcept
		{
			return radius;
		}

	private:

		Transform transform;
		float distance;
		float radius;
		float movedPath;

		void draw()
		{
			const auto& loc = transform.translation;
			const auto& rot = transform.rotation;
			const auto& scale = transform.scale;

			glPushMatrix();

			glTranslated(loc.X, loc.Y + radius + movedPath, loc.Z);
			
			glScaled(scale.X, scale.Y, scale.Z);
			glColor3d(1.0, 0.0, 0.0);
			glutWireSphere(radius, 20, 20);
			glPopMatrix();
		}
	};

	std::unique_ptr<Ball> createBall()
	{
		auto r = std::make_unique<Ball>(
			Transform{ {0.f, 5.f, -30.f},
			  {1.f, 1.f, 1.f},
			}
		);

		return r;
	}

	std::unique_ptr<Box> createBox()
	{
		return std::make_unique<Box>(
			Transform{ {0.f, 0.f, -30.f}, {
				2.f, 1.f, 2.f
			} },
			5.f
			);
	}

	void tick(float deltaTime)
	{
		glTranslated(0.f, -7.f, 0.f);

		if (isAnimate)
		{
			t += 0.05f;
			if (t >= 2 * M_PI)
				t -= 2 * M_PI;
		}

		for (auto& actor : actors)
			actor->tick(deltaTime);
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
		if (isAnimate)
		{
			glutPostRedisplay();
			glutTimerFunc(animationPeriod, animate, 1);
		}
	}

	void initActors()
	{
		auto ball = createBall();
		auto box = createBox();

		actors.push_back(std::move(box));
		actors.push_back(std::move(ball));
	}

	void reinitBall()
	{
		isAnimate = false;
		actors.clear();
		initActors();
		glutPostRedisplay();
	}

	void setup(void)
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glEnable(GL_DEPTH_TEST);
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

	template<typename T>
	T clamp(T min, T max, T val)
	{
		return min > val ? min : val > max ? max : val;
	}

	void keyInput(unsigned char key, int x, int y)
	{
		switch (key)
		{
		case 27:
			exit(0);
			break;
		case ' ':
			isAnimate = !isAnimate;

			if (isAnimate)
			{
				tp = std::chrono::system_clock::now();
				animate(1);
			}
			break;
		case 'r':
		case 'R':
			reinitBall();
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

		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
		glutInitWindowSize(500, 500);
		glutInitWindowPosition(100, 100);
		glutCreateWindow("BallBouncingUp");
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
