
#define _USE_MATH_DEFINES

#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <iostream>
#include <vector>
#include <functional>
#include <chrono>
#include <map>
#include <memory>

namespace BallRollingDown
{
	static bool isAnimate;
	static int animationPeriod = 25;
	static float g = 15.81f;
	static float deltaTime;
	static float angleZInclinedPlane;
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

	struct Plane : Actor
	{
		Plane(const Transform& pTransform, const std::function<void(const Plane&)>& pApplyTransformCallback, float pLength = 10.f, float pWidth = 5.f, std::size_t pSteps = 2)
			:
			transform{ pTransform },
			applyTransformCallback{ pApplyTransformCallback },
			vertexVector{},
			length{pLength},
			width{pWidth},
			steps{pSteps}
		{
			fillVertexVector();
		}

		void tick(float deltaTime) override
		{
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

		float getLength() const noexcept
		{
			return steps * length;
		}

	private:
		Transform transform;

		std::vector<Vector> vertexVector;

		float length;
		float width;

		std::size_t steps;

		std::function<void(const Plane&)> applyTransformCallback;

		void fillVertexVector()
		{
			const auto startLoc = Vector(-1.f * steps / 2 * length, 0.f, width / 2);
		
			std::vector<Vector> v;
			v.reserve(steps * 2 + 2);

			v.push_back(startLoc);
			v.push_back({ startLoc.X, startLoc.Y, startLoc.Z - width});

			float xStep = length;
			for (int i = 0; i < steps; ++i, xStep += length)
			{
				v.push_back({ 
					startLoc.X + xStep,
					startLoc.Y, 
					startLoc.Z }
					);
				v.push_back({
					startLoc.X + xStep,
					startLoc.Y,
					startLoc.Z - width
					});
			}

			v.shrink_to_fit();
			vertexVector = std::move(v);
		}

		void draw()
		{
			const auto& loc = transform.translation;
			const auto& rot = transform.rotation;
			const auto& scale = transform.scale;

			glColor3d(0.0, 0.0, 0.0);
			
			glPushMatrix();
			applyTransformCallback(*this);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			glEnableClientState(GL_VERTEX_ARRAY);

			glVertexPointer(3, GL_FLOAT, 0, vertexVector.data());
			glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexVector.size());

			glDisableClientState(GL_VERTEX_ARRAY);
			glPopMatrix();
		}
	};

	std::unique_ptr<Plane> createInclinedPlane()
	{
		auto callback = [](const Plane& plane) {
			const auto loc = plane.getTransform().translation;
			const auto rot = plane.getTransform().rotation;

			glRotated(rot.angle, rot.dirs.X, rot.dirs.Y, rot.dirs.Z);
			glTranslated(-plane.getLength() / 2, 0., -19.5);
		};
		auto r = std::make_unique<Plane>(
			Transform{ {-9., 0.f, -20.f},
					   {1.f, 1.f, 1.f},
					{-30.f, {0.f, 0.f, 1.f} }},
					callback,
			5.f,
			10.f,
			4u);

		r->tags.push_back("InclinedPlane");
		return r;
	}

	std::unique_ptr<Plane> createBottomPlane()
	{
		auto callback = [](const Plane& plane) {
			const auto loc = plane.getTransform().translation;
			const auto rot = plane.getTransform().rotation;

			glTranslated(loc.X, loc.Y, loc.Z);
			glRotated(rot.angle, rot.dirs.X, rot.dirs.Y, rot.dirs.Z);
		};

		const auto length = 5.f;
		const auto width = 10.f;
		const auto steps = 4u;

		return std::make_unique<Plane>(Transform{ {length * steps / 2, 0.f, -19.5f},
				{1.f, 1.f, 1.f},
				{} },
			callback,
			length,
			width,
			steps);
	}

	struct Ball : Actor
	{
		Ball(const Vector& pVelocity, const Transform& pTransform)
			:
			velocity{ pVelocity },
			transform{ pTransform },
			radius{ 2.f },
			declineChanges{},
			id{},
			stopRotateAngle{},
			rotateBallAngle{},
			fallingDown{},
			plane{},
			angleZ{},
			mass{3.f},
			followPlane{},
			frictionId{},
			accelerationId{}
		{
			addXAccelerationToBall();
		}

		void setRotateZAngle(float newAngleZ)
		{
			angleZ = newAngleZ;
		}

		void setFollowAnglePlane(const Plane& newFollowPlane)
		{
			followPlane = &newFollowPlane;
		}

		void tick(float deltaTime) override
		{
			if (followPlane == nullptr)
			{
				std::cerr << "follow plane didn't set\n";
				std::exit(1);
			}

			const auto rot = followPlane->getTransform().rotation;
			angleZ = rot.angle;

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

				transform.translation += velocity * deltaTime;
			}

			draw();
			const auto loc = transform.translation;
			if (loc.X >= 0.f && !stopRotateAngle)
			{
				stopRotateAngle = true;

				removeVelocityChanger(accelerationId);
				frictionId = addVelocityChanger([this](float deltaTime) -> Vector {
					Vector r;

					if (!isNearlyEqual(velocity.length(), 0.f, 00001.f) && velocity.X > 0)
					{
						r.X -= deltaTime * g * 2;
					}
					else
					{
						removeVelocityChanger(frictionId);
						velocity = Vector{};
					}

					return r;
					});
			}

			if (plane == nullptr)
			{
				std::cerr << "bottom plane didn't set\n";
				std::exit(1);
			}

			if (loc.X >= (plane->getLength()) && !fallingDown)
			{
				removeVelocityChanger(frictionId);
				fallingDown = true;
				addVelocityChanger([](float deltaTime) -> Vector {
					Vector r;

					r.Y = -g * deltaTime;

					return r;
					});

			}
		}

		void setBottomPlane(const Plane& pPlane)
		{
			plane = &pPlane;
		}

		void setTransform(const Transform& newTransform) override
		{
			transform = newTransform;
		}

		Transform getTransform() const override
		{
			return transform;
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

		float getMass() const noexcept
		{
			return mass;
		}

	private:
		Vector velocity;
		Transform transform;

		std::map<uint16_t, std::function<Vector(float)>> forceChangers;
		std::vector<std::pair<uint16_t, std::function<Vector(float)>>> changersForFuture;
		std::vector<std::pair<uint16_t, std::function<Vector(float)>>> removeChangersForFuture;

		float radius;
		bool declineChanges;
		uint16_t id;

		bool stopRotateAngle;

		uint16_t frictionId;
		uint16_t accelerationId;

		float rotateBallAngle;

		bool fallingDown;

		const Plane* plane;

		float angleZ;
		float mass;

		const Plane* followPlane;

		void draw()
		{
			const auto loc = transform.translation;
			const auto rot = transform.rotation;
			const auto scale = transform.scale;

			glPushMatrix();

			if (!stopRotateAngle)
				glRotated(angleZ, 0., 0., 1.);

			glTranslated(loc.X, loc.Y, loc.Z);
			glRotated(rot.angle, rot.dirs.X, rot.dirs.Y, rot.dirs.Z);

			rotateBallAngle += 1.5 * velocity.length();
			if (rotateBallAngle >= 360.f)
				rotateBallAngle -= 360.f;
			glRotated(rotateBallAngle, 0., 0, 1.);

			glScaled(scale.X, scale.Y, scale.Z);
			glColor3d(1.0, 0.0, 0.0);
			glutWireSphere(radius, 20, 20);
			glPopMatrix();
		}

		void addXAccelerationToBall()
		{
			accelerationId = addVelocityChanger([this](float deltaTime) {

				Vector r;

				if (angleZInclinedPlane == 0.f)
					removeVelocityChanger(accelerationId);

				const auto m = getMass();
				r.X = m * g * sin(std::abs(M_PI / 180 * angleZInclinedPlane)) * deltaTime;

				return r;
				});
		}

	};

	std::unique_ptr<Ball> createBall()
	{
		return std::make_unique<Ball>( Vector{},
			Transform{ { -20.0, 0.0, -22.0 },
			  {1.f, 1.f, 1.f},
			  {
				0.f, {0.f, 0.f, 1.f}
			  }
			}
		);
	}

	void tick(float deltaTime)
	{
		glTranslated(0.f, -20.f, -15.f);
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

		auto inclinedPlane = createInclinedPlane();
		auto bottomPlane = createBottomPlane();

		ball->setBottomPlane(*bottomPlane);
		ball->setFollowAnglePlane(*inclinedPlane);

		actors.push_back(std::move(ball));
		actors.push_back(std::move(inclinedPlane));
		actors.push_back(std::move(bottomPlane));
	}

	Plane* getInclinedPlane()
	{
		Plane* inclinedPlane{};

		for (auto& actor : actors)
			for (const auto& tag : actor->tags)
				if (tag == "InclinedPlane")
					inclinedPlane = (Plane*)actor.get();

					return inclinedPlane;
	}
	
	Plane& getInclinedPlaneForSure()
	{
		auto inclinedPlane = getInclinedPlane();
		if (inclinedPlane == nullptr)
		{
			std::cerr << "can't find InclinedPlane\n";
			std::exit(1);
		}

		return *inclinedPlane;
	}

	void reinitBall()
	{
		isAnimate = false;
		actors.clear();
		initActors();
		auto& inclinedPlane = getInclinedPlaneForSure();

		auto newTransform = inclinedPlane.getTransform();
		newTransform.rotation = { angleZInclinedPlane, {0.f, 0.f, 1.f} };

		inclinedPlane.setTransform(newTransform);

		glutPostRedisplay();
	}

	void setup(void)
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glEnable(GL_DEPTH_TEST);
		tp = std::chrono::system_clock::now();
		initActors();
		
		const auto& inclinedPlane = getInclinedPlaneForSure();
		angleZInclinedPlane = inclinedPlane.getTransform().rotation.angle;
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

	void changeInclinedPlaneZAngle(float newAngle)
	{
		const float clampedAngle = clamp(-75.f, 0.f, newAngle);

		auto& inclinedPlane{ getInclinedPlaneForSure() };

		auto newTransform = inclinedPlane.getTransform();
		newTransform.rotation = Rotation{
			clampedAngle,
			{
			newTransform.rotation.dirs.X,
			newTransform.rotation.dirs.Y,
			newTransform.rotation.dirs.Z
			}
		};

		inclinedPlane.setTransform(newTransform);
		angleZInclinedPlane = clampedAngle;
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
			<< "Press up/down arrow keys to increase/decrease the initial angle of left plane.\n"
			<< "Press r to start animation again\n";
	}

	void specialFunc(int key, int x, int y)
	{
		if (key == GLUT_KEY_UP)
		{
			changeInclinedPlaneZAngle(angleZInclinedPlane - 5.f);
			glutPostRedisplay();
		}
		else if (key == GLUT_KEY_DOWN)
		{
			changeInclinedPlaneZAngle(angleZInclinedPlane + 5.f);
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
		glutInitWindowSize(500, 500);
		glutInitWindowPosition(100, 100);
		glutCreateWindow("BallRollingDown");
		glutDisplayFunc(drawScene);
		glutReshapeFunc(resize);
		glutKeyboardFunc(keyInput);
		glutSpecialFunc(specialFunc);

		glewExperimental = GL_TRUE;
		glewInit();

		setup();

		glutMainLoop();

		return 0;
	}
}
