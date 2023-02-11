
#define _USE_MATH_DEFINES

#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <iostream>
#include <vector>
#include <functional>
#include <chrono>
#include <map>
#include <memory>

namespace BallRollingHelix
{
	static bool isAnimate;
	static int animationPeriod = 25;
	static float deltaTime;
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
		virtual void setTransform(const Transform& newTransform) = 0;
		virtual Transform getTransform() const = 0;

		std::vector<std::string> tags;
	};

	std::vector<std::unique_ptr<Actor>> actors;

	struct Helix
	{
		Helix(const Transform& pTransform, int pN, float pR)
			:
			transform{ pTransform },
			N{ pN },
			R{ pR }
		{
			fillVertexVector();
		}

		std::vector<Vector> getDirs() const 
		{
			std::vector<Vector> r;

			for (int i = 0; i < vertexVector.size() - 1; ++i)
				r.push_back(normalize(vertexVector[i + 1] - vertexVector[i]));
			
			return r;
		}

		float getSegmentPath() const noexcept
		{
			return 2 * R * sin(M_PI / N);
		}

		void draw()
		{
			glColor3d(0.0, 0.0, 0.0);

			glPushMatrix();
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			glEnableClientState(GL_VERTEX_ARRAY);

			glVertexPointer(3, GL_FLOAT, 0, vertexVector.data());
			glDrawArrays(GL_LINE_STRIP, 0, vertexVector.size());

			glDisableClientState(GL_VERTEX_ARRAY);
			glPopMatrix();
		}

	private:
		Transform transform;
		int N;
		float R;
		std::vector<Vector> vertexVector;

		void fillVertexVector()
		{
			auto circleCenter = Vector{ -R,  0.f, 0.f };
			const auto ZOffset = -5.f;
			const auto Circles = 5;

			std::vector<Vector> v;
			v.reserve(Circles * (N + 1));

			float t{};
			for (int i = 0; i < Circles; ++i)
			{
				for (int j = 0; j <= N; ++j)
				{
					v.push_back({
					circleCenter.X + R * cos(t),
					circleCenter.Y + R * sin(t),
					circleCenter.Z + ZOffset * i + ZOffset * j / N }
					);

					t += 2 * M_PI / N;
				}
			}

			v.shrink_to_fit();
			vertexVector = std::move(v);
		}

	};

	struct Ball : Actor
	{
		Ball(const Vector& pVelocity, const Transform& pTransform)
			:
			velocity{ pVelocity },
			transform{ pTransform },
			helixTrajectory{transform, 15, 10.f},
			radius{ 2.f },
			declineChanges{},
			rotateBallAngle{},
			accelerationId{},
			dirs{helixTrajectory.getDirs()},
			dir{ dirs[0] },
			ballLocation{transform.translation},
			start{ballLocation}
		{
			addXAccelerationToBall();
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

				ballLocation += velocity * deltaTime;
			}

			const auto segmentPath = helixTrajectory.getSegmentPath();
			 
			if (((start - ballLocation).length() >= segmentPath) && !dirs.empty())
			{
				dirs.erase(std::begin(dirs));
				if (!dirs.empty())
				{
					start += dir * segmentPath;
					dir = *dirs.begin();
					velocity = dir * velocity.length();
				}
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

		float getRadius() const noexcept
		{
			return radius;
		}

		void setVelocity(const Vector& newVelocity)
		{
			velocity = newVelocity;
		}

		Vector getVelocity() const noexcept
		{
			return velocity;
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
		Transform transform;

		Vector ballLocation;
		Helix helixTrajectory;

		std::map<uint16_t, std::function<Vector(float)>> forceChangers;
		std::vector<std::pair<uint16_t, std::function<Vector(float)>>> changersForFuture;
		std::vector<std::pair<uint16_t, std::function<Vector(float)>>> removeChangersForFuture;

		float radius;
		bool declineChanges;
		uint16_t id;

		uint16_t accelerationId;

		float rotateBallAngle;

		std::vector<Vector> dirs;
		Vector dir;
		Vector start;

		void draw()
		{
			const auto loc = transform.translation;
			const auto rot = transform.rotation;
			const auto scale = transform.scale;

			glPushMatrix();

			glPushMatrix();
			glTranslated(loc.X, loc.Y, loc.Z);
			helixTrajectory.draw();
			glPopMatrix();

				glPushMatrix();

				glTranslated(ballLocation.X, ballLocation.Y, ballLocation.Z);

				rotateBallAngle += 1.5 * velocity.length();
				if (rotateBallAngle >= 360.f)
					rotateBallAngle -= 360.f;
				glRotated(rotateBallAngle, 0., 0, 1.);

				glScaled(scale.X, scale.Y, scale.Z);
				glColor3d(1.0, 0.0, 0.0);
				glutWireSphere(radius, 20, 20);

				glPopMatrix();

			
			glPopMatrix();
		}

		void addXAccelerationToBall()
		{
			accelerationId = addVelocityChanger([this](float deltaTime) {

				Vector r;

				r = dir * deltaTime * 5.f;

				return r;
				});
		}

	};

	std::unique_ptr<Ball> createBall()
	{
		return std::make_unique<Ball>( 
			Vector{},
			Transform{ { 5.0, 0.0, -22.0 },
			  {1.f, 1.f, 1.f},
			  {
				0.f, {0.f, 0.f, 1.f}
			  }
			}
		);
	}

	void tick(float deltaTime)
	{
		glTranslated(0.f, -15.f, -10.f);
		glRotated(45., 1., 0., 0.);

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
		glutCreateWindow("BallRollingHelix");
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
