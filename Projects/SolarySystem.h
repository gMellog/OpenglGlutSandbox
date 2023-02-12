
#define _USE_MATH_DEFINES

#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <iostream>
#include <vector>
#include <chrono>
#include <memory>

namespace SolarySystem
{
	static bool isAnimate;
	static int animationPeriod = 25;

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

		float X;
		float Y;
		float Z;
	};

	struct Color
	{
		Color(float pR = 0.f, float pG = 0.f, float pB = 0.f)
			:
			R{pR},
			G{pG},
			B{pB}
		{

		}

		float R;
		float G;
		float B;
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

	struct Sun : Actor
	{
		explicit Sun(const Transform& pTransform)
			:
			transform{ pTransform }
		{

		}

		void tick(float deltaTime) override
		{
			transform.rotation.angle += 45 * deltaTime;
			if (transform.rotation.angle >= 360.)
				transform.rotation.angle -= 360.f;

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

		void draw()
		{
			glColor3f(1.f, 165 / 255.f, 0);
			glPushMatrix();
			glRotated(transform.rotation.angle, 0., 1., 0.);
			glutWireSphere(5, 30, 30);
			glPopMatrix();
		}
	};

	struct Moon 
	{
		Moon(const Transform& pTransform, const Color& pColor, float pSpeedAnglePerSec)
			:
			transform{pTransform},
			color{pColor},
			speedAnglePerSec{ pSpeedAnglePerSec }
		{

		}

		void tick(float deltaTime)
		{
			auto& angle = transform.rotation.angle;

			angle += speedAnglePerSec * deltaTime;
			if (angle >= 360.f)
				angle -= 360.f;
		}

		void draw()
		{
			const auto& moonLoc = transform.translation;
			const auto& moonRot = transform.rotation;

			glPushMatrix();
			glColor3f(color.R, color.G, color.B);

			glRotated(moonRot.angle, moonRot.dirs.X, moonRot.dirs.Y, moonRot.dirs.Z);
			glTranslated(moonLoc.X, moonLoc.Y, moonLoc.Z);
			glRotated(moonRot.angle, 0.f, 1.f, 0.f);

			glutWireSphere(1., 30, 30);
			glPopMatrix();
		}

	private:
		
		Transform transform;
		Color color;
		float speedAnglePerSec;
	
	};

	struct Earth : Actor
	{
		Earth(const Sun& pSun, float pR)
			:
			sun{pSun},
			r{pR},
			transform{ initTransform() },
			moonRotX{ initMoonRotX() },
			moonRotY{ initMoonRotY() }
		{
		}

		void tick(float deltaTime) override
		{
			transform.rotation.angle += 30 * deltaTime * 5.f;

			if (transform.rotation.angle >= 360.)
				transform.rotation.angle -= 360.f;

			moonRotX.tick(deltaTime);
			moonRotY.tick(deltaTime);

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

		const std::reference_wrapper<const Sun> sun;
		float r;
		Transform transform;
		Moon moonRotX;
		Moon moonRotY;

		Transform initTransform() const noexcept
		{
			auto sunTransform = sun.get().getTransform();
			sunTransform.translation.X += r;
			sunTransform.rotation.dirs = { 0.f, 1.f, 0.f };

			return sunTransform;
		}

		Moon initMoonRotX() const noexcept
		{
			Transform transform{ {}, {} };

			transform.translation.X = 5.f;
			transform.rotation.angle = 30.f;
			transform.rotation.dirs = { 0.f, 1.f, 0.f };
			
			return Moon(transform, {1.f, 0.f, 0.f}, 90.f);
		}

		Moon initMoonRotY() const noexcept
		{
			Transform transform{ {}, {} };

			transform.translation.Y = 5.f;
			transform.rotation.angle = 0.f;
			transform.rotation.dirs = { 1.f, 0.f, 0.f };

			return Moon(transform, { 0.7f, 0.7f, 0.7f }, 180.f);
		}

		void draw()
		{
			const auto& loc = transform.translation;
			const auto& rot = transform.rotation;

			glPushMatrix();

			glRotated(rot.angle, rot.dirs.X, rot.dirs.Y, rot.dirs.Z);
			glTranslated(loc.X, loc.Y, loc.Z);
			glRotated(rot.angle, 0.f, 1.f, 0.f);

			moonRotX.draw();
			moonRotY.draw();

			glColor3f(0.f, 0.f, 1.f);
			glutWireSphere(2., 30, 30);
			
			glPopMatrix();
		}
	};

	struct DiagPlanet : Actor
	{
		DiagPlanet(const Transform& pTransform, const Color& pColor, float pSpeedAnglePerSec = 30.f, float pRadius = 3.5f)
			:
			transform{pTransform},
			rotUpDown{ 0.f, {0.f, 0., 1.f} },
			color{ pColor },
			speedAnglePerSec{ pSpeedAnglePerSec },
			radius{pRadius}
		{

		}

		void tick(float deltaTime) override
		{
			rotUpDown.angle += speedAnglePerSec * deltaTime * 5.f;

			if (rotUpDown.angle >= 360.)
				rotUpDown.angle -= 360.f;
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
		Rotation rotUpDown;
		Color color;
		float speedAnglePerSec;
		float radius;

		void draw()
		{
			const auto& loc = transform.translation;
			const auto& rot = transform.rotation;
			glColor3f(color.R, color.G, color.B);

			glPushMatrix();
			glRotated(rot.angle, rot.dirs.X, rot.dirs.Y, rot.dirs.Z);
				
				glPushMatrix();
			
				glRotated(rotUpDown.angle, rotUpDown.dirs.X, rotUpDown.dirs.Y, rotUpDown.dirs.Z);
				glTranslated(loc.X, loc.Y, loc.Z);
				glRotated(rotUpDown.angle, 0.f, 1.f, 0.f);

				glutWireSphere(radius, 30, 30);
			
				glPopMatrix();
			
			glPopMatrix();
		}
	};

	
	std::unique_ptr<DiagPlanet> createMagentaGiant()
	{
		return std::make_unique<DiagPlanet>(
			Transform{ {-12.5f, 0.f, 0.f}, {}, { 30 , {0.f, 1.f, 0.f} } }, Color{1.f, 0.f, 1.f}
			);
	}

	std::unique_ptr<DiagPlanet> createGreenMiddle()
	{
		return std::make_unique<DiagPlanet>(
			Transform{ {20.f, 0.f, 0.f}, {}, { -30 , {0.f, 1.f, 0.f} } }, Color{ 0.f, 1.f, 0.f }, 20.f, 2.5f
		);
	}

	void tick(float deltaTime)
	{
		glTranslated(0.f, 0.f, -30.f);
		glRotated(45., 1.f, 0.f, 0.f);
		for (auto& actor : actors)
			actor->tick(deltaTime);

	}

	void drawScene(void)
	{
		using namespace std::chrono;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		auto d = duration_cast<milliseconds>(system_clock::now() - tp);
		const auto deltaTime = float(d.count()) * milliseconds::period::num / milliseconds::period::den;
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
		auto sun = std::make_unique<Sun>(Transform{ {0.f, 0.f, 0.f}, {} });
		auto earth = std::make_unique<Earth>(*sun, 11.f);
		auto magentaGiant = createMagentaGiant();
		auto greenMiddle = createGreenMiddle();

		actors.push_back(std::move(sun));
		actors.push_back(std::move(earth));
		actors.push_back(std::move(magentaGiant));
		actors.push_back(std::move(greenMiddle));
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
		std::cout << "Press space to toggle between animation on and off.\n";
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
		glutCreateWindow("SolarySystem");
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
