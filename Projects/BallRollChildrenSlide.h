
#define _USE_MATH_DEFINES

#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <iostream>
#include <vector>
#include <functional>
#include <chrono>
#include <map>
#include <memory>

namespace BallRollChildrenSlide
{
	static bool isAnimate;
	static int animationPeriod = 25;
	static float g = 15.81f;
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

	struct ChildrenSlide : Actor
	{
		ChildrenSlide(const Transform& pTransform)
			:
			transform{ pTransform },
			circleSegments{10},
			startAngle{ M_PI + M_PI_2 / 3 },
			endAngle{ 3 * M_PI_2},
			xPath{25.f},
			R{ xPath / sin(endAngle - startAngle) },
			closeCircleCenter{ 
				-20.f - R * cos(getStartAngleInRad()),  
				 7.f - R * sin(getStartAngleInRad()),
				 -30.f
			},
			farCircleCenter{
				-15.f - R * cos(getStartAngleInRad()),
				 10.f - R * sin(getStartAngleInRad()),
				 -37.5f
			}
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

		float getCircleSegments() const noexcept
		{
			return circleSegments;
		}

		float getStartAngleInRad() const noexcept
		{
			return 	startAngle;
		}

		float getMoveAngleInRad() const noexcept
		{
			return (endAngle - startAngle) / circleSegments;
		}

		float getSegmentLength() const noexcept
		{
			return (vertexVector[2] - vertexVector[4]).length();
		}

		std::vector<Vector> getVerticies() const 
		{
			return vertexVector;
		}

	private:
		Transform transform;
		int circleSegments;
		float startAngle;
		float endAngle;
		std::vector<Vector> vertexVector;
		
		float xPath;
		float R;
		Vector closeCircleCenter;
		Vector farCircleCenter;

		void fillVertexVector()
		{
			vertexVector.reserve(6 + (circleSegments + 1) * 2);

			vertexVector.push_back(
				{ -20.f, -10.f, -30.f }
			);
			vertexVector.push_back(
				{ -15.f, -10.f, -37.5f }
			);
			vertexVector.push_back(
				{ -20.f, 7.f, -30.f }
			);
			vertexVector.push_back(
				{ -15.f, 10.f, -37.5f }
			);

			float t;
			for (int i = 1; i <= circleSegments; ++i)
			{
				t = (getStartAngleInRad() + getMoveAngleInRad() * i);

				vertexVector.push_back({
					closeCircleCenter.X + R * cos(t), 
					closeCircleCenter.Y + R * sin(t), 
					closeCircleCenter.Z
					});
				vertexVector.push_back({
					farCircleCenter.X + R * cos(t),
					farCircleCenter.Y + R * sin(t),
					farCircleCenter.Z
					});
			}

			vertexVector.push_back({
					closeCircleCenter.X + R * cos(t), -10.f, closeCircleCenter.Z
				});
			vertexVector.push_back({
					farCircleCenter.X + R * cos(t), -10.f, farCircleCenter.Z
				});
				
		}

		void draw()
		{
			glColor3d(0.0, 0.0, 0.0);

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			glEnableClientState(GL_VERTEX_ARRAY);

			glVertexPointer(3, GL_FLOAT, 0, vertexVector.data());
			glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexVector.size());

			glDisableClientState(GL_VERTEX_ARRAY);
		}
	};

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
			mass{3.f},
			accelerationId{},
			childrenSlide{},
			start{ transform.translation },
			dir{}
		{
			addXAccelerationToBall();
		}

		void setChildrenSlide(const ChildrenSlide& newChildrenSlide)
		{
			childrenSlide = &newChildrenSlide;

			if (childrenSlide == nullptr)
			{
				std::cerr << "childrenSlide is nullptr\n";
				std::exit(1);
			}

			const auto verticies = childrenSlide->getVerticies();

			const auto lastCloseCirclePoint = verticies[verticies.size() - 4];
			const auto lastClosePlanePoint = verticies[verticies.size() - 2];
			const auto btmClose = verticies[0];
			const auto upClose = verticies[2];
			const auto upFar = verticies[3];

			const auto nextVert = verticies[4];
			transform.translation = upClose + (upFar - upClose) * 0.5f;
			start = transform.translation;
			
			dirs.reserve(verticies.size() / 2);

			// -3 cause we don't include first pair, last exist pair and last imaginary pair(it doesn't exist)
			for (int i = 0, j = 2; i < (verticies.size() / 2 - 3); ++i, j += 2)
				dirs.push_back(normalize(verticies[j + 2] - verticies[j]));
			
			dir = dirs[0];
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

				transform.translation += velocity * deltaTime;
			}

			draw();

			if (childrenSlide == nullptr)
			{
				std::cerr << "childrenSlide is nullptr\n";
				std::exit(1);
			}

			const auto loc = transform.translation;
			const auto segmentLength = childrenSlide->getSegmentLength();
			
			if (((start - loc).length() >= segmentLength) && !stopRotateAngle)
			{
				const auto moveAngle = childrenSlide->getMoveAngleInRad();
				start += dir * segmentLength;
				dirs.erase(std::begin(dirs));
				if (!dirs.empty())
				{
					const auto length = velocity.length();
					dir = dirs.front();
					velocity = dir * length;
				}
				else
				{
					stopRotateAngle = true;
					removeVelocityChanger(accelerationId);
					accelerationId = addVelocityChanger([this](float deltaTime) -> Vector {
						Vector r;

						r.Y = -mass * g * deltaTime;

						return r;
						});
				}
			}
			
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

		uint16_t accelerationId;

		float rotateBallAngle;

		float mass;
		Vector start;

		const ChildrenSlide* childrenSlide;

		Vector dir;

		std::vector<Vector> dirs;

		void draw()
		{
			const auto loc = transform.translation;
			const auto rot = transform.rotation;
			const auto scale = transform.scale;

			glPushMatrix();

			glTranslated(loc.X, loc.Y + radius, loc.Z);
			
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

				r = mass * g * dir * deltaTime;

				return r;
				});
		}

	};

	std::unique_ptr<Ball> createBall()
	{
		auto r = std::make_unique<Ball>(Vector{},
			Transform{ {-19.f, 0.f, -30.f},
			  {1.f, 1.f, 1.f},
			  {
				0.f, {0.f, 0.f, 1.f}
			  }
			}
		);

		return r;
	}

	void tick(float deltaTime)
	{
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

	std::unique_ptr<ChildrenSlide> createChildrenSlide()
	{
		return std::make_unique<ChildrenSlide>(
			Transform{ {}, {} }
			);
	}

	void initActors()
	{
		auto ball = createBall();
		auto childrenSlide = createChildrenSlide();

		ball->setChildrenSlide(*childrenSlide);

		actors.push_back(std::move(ball));
		actors.push_back(std::move(childrenSlide));
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
		glutCreateWindow("BallRollChildrenSlide");
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
