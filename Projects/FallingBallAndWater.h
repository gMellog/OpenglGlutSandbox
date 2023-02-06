
#define _USE_MATH_DEFINES

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <vector>
#include <functional>
#include <chrono>
#include <vector>
#include <utility>
#include <string>
#include <sstream>
#include <iomanip>
#include <map>
#include <cmath>
#include <memory>

namespace FallingBallAndWater
{
	static bool isAnimate;
	static int animationPeriod = 25;
	static float g = 15.81f;
	static float drag = 1.5;
	static uint16_t gravityHandlerId;

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

		Vector operator+(const Vector& rhs) const noexcept
		{
			return { X + rhs.X, Y + rhs.Y, Z + rhs.Z };
		}

		Vector operator*(float v) const noexcept
		{
			return { X * v, Y * v, Z * v };
		}

		float X;
		float Y;
		float Z;
	};

	Vector normalize(const Vector& v)
	{
		return { v.X / v.length(), v.Y / v.length(), v.Z / v.length() };
	}

	bool isNearlyEqual(float a, float b, float e)
	{
		return (a - b) < e;
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
		virtual void collideWith(Actor& actor) = 0;
		virtual Transform getTransform() const = 0;
		virtual std::string getCollideType() const = 0;
	};

	std::vector<std::unique_ptr<Actor>> actors;

	struct Ball : Actor
	{
		Ball(const Vector& pVelocity, const Vector& pLoc)
			:
			velocity{ pVelocity },
			loc{ pLoc },
			radius{ 2.f },
			springiness{ 1.f },
			declineChanges{},
			id{}
		{

		}

		void tick(float deltaTime) override
		{
			if (isAnimate)
			{
				if (!changersForFuture.empty())
				{
					for (auto& i : changersForFuture)
						forceChangers.insert(i);

					changersForFuture.clear();
				}

				if (!removeChangersForFuture.empty())
				{
					for (const auto& i : removeChangersForFuture)
					{
						auto it = forceChangers.find(i.first);
							
						if (it != forceChangers.end())
						{
							forceChangers.erase(it);
						}	
					}

					removeChangersForFuture.clear();
				}

				Vector velocityChange;

				for (const auto forceChange : forceChangers)
				{
					const Vector change = forceChange.second(deltaTime);

					if (!declineChanges)
					{
						velocityChange += change;
					}
					else
					{
						velocityChange = 0.f;
						declineChanges = false;
					}
				}

				velocity += velocityChange;

				loc += velocity * deltaTime;
			}

			draw();
		}

		void collideWith(Actor& actor) override
		{
		}

		Transform getTransform() const override
		{
			return Transform{ {loc.X, loc.Y - 2.f, loc.Z}, {} };
		}

		std::string getCollideType() const
		{
			return "Ball";
		}

		static const std::string collideType;

		float getRadius() const noexcept
		{
			return radius;
		}

		void setLocation(const Vector& newLocation)
		{
			loc = newLocation;
			glutPostRedisplay();
		}

		Vector getLocation() const noexcept
		{
			return loc;
		}

		void setVelocity(const Vector& newVelocity)
		{
			velocity = newVelocity;
		}

		Vector getVelocity() const noexcept
		{
			return velocity;
		}

		void reinitForceChangers()
		{
			forceChangers.clear();
		}

		Vector getDir() const noexcept
		{
			return normalize(velocity);
		}

		void setDir(const Vector& newDir) noexcept
		{
			velocity = newDir * velocity.length();
		}

		void setSpringiness(float newSpringiness)
		{
			springiness = newSpringiness;
		}

		float getSpringiness() const noexcept
		{
			return springiness;
		}

		void declineVelocityChanges()
		{
			declineChanges = true;
		}

		uint16_t addVelocityChanger(const std::function<Vector(float)>& velocityChanger)
		{
			changersForFuture.push_back({ id, velocityChanger });
			return id++;
		}

		void removeVelocityChanger(const uint16_t id)
		{
			for (auto& i : forceChangers)
			{
				if (i.first == id)
				{
					removeChangersForFuture.push_back(i);
					break;
				}
			}
		}
	
private:
		Vector velocity;
		Vector loc;

		std::map<uint16_t,std::function<Vector(float)>> forceChangers;
		std::vector<std::pair<uint16_t, std::function<Vector(float)>>> changersForFuture;
		std::vector<std::pair<uint16_t, std::function<Vector(float)>>> removeChangersForFuture;
		
		float radius;
		float springiness;
		bool declineChanges;
		uint16_t id;

		void draw()
		{
			glPushMatrix();
			glTranslated(loc.X, loc.Y, loc.Z);
			glColor3d(1.0, 0.0, 0.0);
			glutSolidSphere(radius, 20, 20);
			glPopMatrix();
		}

	};

	const std::string Ball::collideType{ "Ball" };

	static const std::string collideType{ "Wall" };

	void checkCollide()
	{
		for (const auto& actor1 : actors)
		{
			if (!actor1) continue;

			for (const auto& actor2 : actors)
			{
				if (!actor2) continue;

				if (actor1 != actor2)
				{
					actor1->collideWith(*actor2);
				}
			}
		}
	}

	void addGravityToBall(Ball& ball)
	{
		gravityHandlerId = ball.addVelocityChanger([](float deltaTime) {
			Vector r;

			r.X = 0.f;
			r.Y = -g * deltaTime;

			return r;
			});
	}

	struct WaterWall : Actor
	{
		WaterWall(const Transform& pTransform, float pSize)
			:
			transform{ pTransform },
			size{ pSize },
			ballAtSurfaceOffset{2.0f},
			t{},
			amplitude{0.3f},
			floating{},
			ballInWater{},
			handlerId{},
			ballMiddleLoc{},
			ball{}
		{

		}

		void tick(float deltaTime) override
		{
			draw();

			if (floating)
			{
				if (t >= 2 * M_PI)
				{
					t -= 2 * M_PI;
				}

				const auto newLoc = Vector{
					ballMiddleLoc.X,
					ballMiddleLoc.Y + amplitude * sin(t) * 2.f,
					ballMiddleLoc.Z
				};

				t += deltaTime;
				ball->setLocation(newLoc);
			}
		}

		void collideWith(Actor& actor) override
		{
			if(!ballInWater)
			{
				if (actor.getCollideType() == Ball::collideType)
				{
					auto& ball = (Ball&)actor;

					if (insideBox(ball))
					{
						if (!this->ball)
							this->ball = &ball;

						handlerId = ball.addVelocityChanger([&ball, this](float deltaTime) mutable {
							Vector r;
							
							const auto vel = ball.getVelocity();

							if (!isNearlyEqual(vel.length(), 0, 0.5f))
							{
								r.Y = g * drag * deltaTime * 1.5f;
							}
							else
							{
								ball.removeVelocityChanger(gravityHandlerId);
								ball.removeVelocityChanger(handlerId);
								ball.declineVelocityChanges();
								ball.setVelocity({0.f, 1.f, 0.f});

								const auto initialLoc = ball.getTransform().translation;

								const auto lambda = [&ball, this, initialLoc](float deltaTime)  mutable {
									Vector r;
									const auto ballLoc = ball.getTransform().translation;
									const auto waterWallLoc = getTransform().translation;
									const auto endLocY = initialLoc.Y + (waterWallLoc.Y - initialLoc.Y) * 1.2f;

									if (ballLoc.Y >= endLocY)
									{
										ball.setVelocity({});
										ball.removeVelocityChanger(handlerId);
										ballAtFloat();
									}

									return r;
								};

								handlerId = ball.addVelocityChanger(lambda);
							}

							return r;
							});
					
						ballInWater = true;
					}
				}
			}
		}

		Transform getTransform() const override
		{
			return transform;
		}

		std::string getCollideType() const
		{
			return collideType;
		}

	private:

		Transform transform;
		float size;
		float ballAtSurfaceOffset;
		float t;
		float amplitude;
		bool floating;
		bool ballInWater;
		uint16_t handlerId;
		Vector ballMiddleLoc;
		Ball* ball;

		void ballAtFloat()
		{
			t = 0;
			floating = true;
			ballMiddleLoc = ball->getLocation();
		}

		void draw()
		{
			const auto& loc = transform.translation;
			const auto& scale = transform.scale;

			glColor3d(0.0, 0.0, 1.0);
			glPushMatrix();
			glTranslated(loc.X, loc.Y, loc.Z);
			glScaled(scale.X, scale.Y, scale.Z);

			glutSolidCube(size);
			glPopMatrix();
		}

		bool insideBox(const Ball& ball) const noexcept
		{
			const auto R = ball.getRadius();
			const auto dir = ball.getDir();
			const auto loc = ball.getTransform().translation;

			const auto point = loc + dir * R;

			const auto minX = transform.translation.X - size / 2 * transform.scale.X;
			const auto maxX = transform.translation.X + size / 2 * transform.scale.X;
			const auto minY = transform.translation.Y - size / 2 * transform.scale.Y;
			const auto maxY = transform.translation.Y + size / 2 * transform.scale.Y;
			const auto minZ = transform.translation.Z - size / 2 * transform.scale.Z;
			const auto maxZ = transform.translation.Z + size / 2 * transform.scale.Z;

			return (minX <= point.X && point.X <= maxX) &&
				(minY <= point.Y && point.Y <= maxY) &&
				(minZ <= point.Z && point.Z <= maxZ);
		}

	};

	void tick(float deltaTime)
	{
		for (const auto& actor : actors)
			if (actor) {
				actor->tick(deltaTime);
			}
	}

	void drawScene(void)
	{
		using namespace std::chrono;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		auto d = duration_cast<milliseconds>(system_clock::now() - tp);
		auto deltaTime = float(d.count()) * milliseconds::period::num / milliseconds::period::den;
		tp = system_clock::now();

		tick(deltaTime);
		checkCollide();

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
		Vector initialBallVelocity = { 0.f, -2.5f, 0.f };
		Vector initialBallLocation = { 0.0, 6.0, -15.0 };

		auto ball = std::make_unique<Ball>(
			initialBallVelocity,
			initialBallLocation
			);

		addGravityToBall(*ball);
		actors.push_back(std::move(ball));

		auto waterWall = std::make_unique<WaterWall>(
			Transform{ {0.f, -10.f, -15.f},
			{10.f, 1.f, 1.f} },
			5.f 
			);
		actors.push_back(std::move(waterWall));
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
		std::cout << "Press space to toggle between animation on and off." << std::endl
			<< "Press right/left arrow kes to increase/decrease the initial horizontal velocity.\n"
			<< "Press up/down arrow keys to increase/decrease the initial vertical velocity.\n"
			<< "Press page up/down keys to increase/decrease springiness with right wall.\n"
			<< "Press page up/down keys to increase/decrease gravitational acceleration.\n"
			<< "Press r to reset.\n";
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
		glutCreateWindow("FallingBallAndWater");
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
