
#define _USE_MATH_DEFINES

#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <iostream>
#include <vector>
#include <functional>
#include <chrono>
#include <map>
#include <memory>

namespace BallRollOnPoolTable
{
	static bool isAnimate;
	static int animationPeriod = 25;
	static float deltaTime;
	static float angleZInclinedPlane;
	std::chrono::system_clock::time_point tp;

	static float Xangle = 0.0, Yangle = 0.0, Zangle = 0.0;
	static int width = 500;
	static int height = 500;

	static float minX;
	static float maxX;
	static float minZ;
	static float maxZ;

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

	bool inRange(float min, float max, float v)
	{
		return (min <= v) && (v <= max);
	}

	float getAngleBetweenVectors(const Vector& lhs, const Vector& rhs)
	{
		const auto scalarProduct = lhs.X * rhs.X + lhs.Y * rhs.Y + lhs.Z * rhs.Z;
		const auto lengthsMult = lhs.length() * rhs.length();

		return acosf(scalarProduct / lengthsMult) / M_PI * 180;
	}

	template<typename T>
	T clamp(T min, T max, T val)
	{
		return min > val ? min : val > max ? max : val;
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
		Transform(const Vector& pTranslation = {},
			const Vector& pScale = {},
			const Rotation& pRotation = {}
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

	struct Wall : Actor
	{
		Wall(const Transform& pTransform, float pSize = 2.5f)
			:
			transform{ pTransform },
			size{ pSize }
		{
		}

		void tick(float deltaTime) override
		{
			draw();
		}

		void setTransform(const Transform& newTransform)
		{
			transform = newTransform;
		}

		Transform getTransform() const override
		{
			return transform;
		}

		float getSize() const noexcept
		{
			return size;
		}

	private:

		void draw()
		{
			const auto& loc = transform.translation;
			const auto& rot = transform.rotation;
			const auto& scale = transform.scale;

			glColor3d(0.0, 0.0, 1.0);
			glPushMatrix();
			glTranslated(loc.X, loc.Y, loc.Z);
			glRotated(rot.angle, rot.dirs.X, rot.dirs.Y, rot.dirs.Z);
			glScaled(scale.X, scale.Y, scale.Z);

			glutWireCube(size);
			glPopMatrix();
		}

		Transform transform;
		float size;
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
			rotateBallAngle{},
			frictionId{}
		{
			tags.push_back(tag);
		}

		void tick(float deltaTime) override
		{
			if (velocity.length() != 0.f)
			{
				frictionId = addVelocityChanger([this](float deltaTime) -> Vector {
					Vector r;
					
					if (!isNearlyEqual(velocity.length(), 0.f, 0.5f))
					{
						r.X += ((velocity.X > 0.f) ? -1.f : 1.f) * 0.02 * deltaTime;
						r.Z += ((velocity.Z > 0.f) ? -1.f : 1.f) * 0.02 * deltaTime;
					}
					else
					{
						setVelocity({});
						removeVelocityChanger(frictionId);
					}

					return r;
					});
			}

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

			const auto futureLoc = transform.translation + velocity * deltaTime;

			if (futureLoc.X >= maxX || futureLoc.X <= minX)
			{
				velocity.X *= -0.75f;
			}
			
			if (futureLoc.Z >= maxZ || futureLoc.Z <= minZ)
			{
				velocity.Z *= -0.75f;
			}
			
			transform.translation += velocity * deltaTime;

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

		static const std::string tag;

	private:
		Vector velocity;
		Transform transform;

		std::map<uint16_t, std::function<Vector(float)>> forceChangers;
		std::vector<std::pair<uint16_t, std::function<Vector(float)>>> changersForFuture;
		std::vector<std::pair<uint16_t, std::function<Vector(float)>>> removeChangersForFuture;

		float radius;
		bool declineChanges;
		uint16_t id;

		uint16_t frictionId;

		float rotateBallAngle;
		
		void draw()
		{
			const auto& loc = transform.translation;
			const auto& rot = transform.rotation;
			const auto& scale = transform.scale;

			glPushMatrix();
			
			glTranslated(loc.X, loc.Y + radius, loc.Z);
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

	};

	const std::string Ball::tag{"Ball"};
	
	struct DirArrow : Actor
	{
		DirArrow(Ball& pBall, float pInitLength)
			:
			ball{ pBall },
			transform{ {}, {2.f, 1.f, 1.f}, {0.f, {0.f, 1.f, 0.f}} },
			length{pInitLength},
			verticies{fillVerticies()},
			hidden{true}
		{
			tags.push_back(tag);
		}

		void tick(float deltaTime) override
		{
			if(!hidden)
				draw();
		}

		void setTransform(const Transform& newTransform)
		{
			transform = newTransform;
		}

		Transform getTransform() const override
		{
			return { getLoc(), {transform.scale}, transform.rotation };
		}

		void setHidden(bool newHidden) noexcept
		{
			hidden = newHidden;
		}

		bool getHidden() const noexcept
		{
			return hidden;
		}

		static const std::string tag;

	private:
		
		std::vector<Vector> fillVerticies() const
		{
			const auto arrowOffset = 1.5f;
			return {
				{0.f, 0.f, 0.f},
				{0.f, 0.f, -length},

				{-arrowOffset, 0.f, -2 * length / 3},
				{0.f, 0.f, -length},

				{arrowOffset, 0.f, -2 * length / 3},
				{0.f, 0.f, -length}
			};
		}

		void draw()
		{
			const auto& loc = getLoc();
			const auto& rot = transform.rotation;
			const auto& scale = transform.scale;

			glColor3d(0.0, 0.0, 1.0);
			glPushMatrix();
			glTranslated(loc.X, loc.Y, loc.Z);
			glScaled(scale.X, scale.Y, scale.Z);
			glRotated(rot.angle, rot.dirs.X, rot.dirs.Y, rot.dirs.Z);

			glEnableClientState(GL_VERTEX_ARRAY);

			glVertexPointer(3, GL_FLOAT, 0, verticies.data());

			glColor3f(0.f, 0.f, 0.f);
			glLineWidth(5.f);
			glDrawArrays(GL_LINES, 0, verticies.size());
			glLineWidth(1.f);

			glDisableClientState(GL_VERTEX_ARRAY);

			glPopMatrix();
		}

		Vector getLoc() const noexcept
		{
			return ball.get().getTransform().translation;
		}

		std::reference_wrapper<Ball> ball;
		Transform transform;
		float length;
		std::vector<Vector> verticies;
		bool hidden;
	};

	const std::string DirArrow::tag{"DirArrow"};

	struct ArrowHandler : Actor
	{
		explicit ArrowHandler(DirArrow& pDirArrow)
			:
			dirArrow{ pDirArrow },
			setDir{},
			length{}
		{
			tags.push_back(tag);
		}

		void tick(float deltaTime) override
		{
		}

		void setTransform(const Transform& newTransform)
		{

		}

		Transform getTransform() const override
		{
			return Transform();
		}

		void setArrow(int x, int y)
		{
			const Vector initDir{ 0.f, 1.f };

			auto& dirArrowRef = dirArrow.get();

			if (dirArrowRef.getHidden())
			{
				dirArrowRef.setHidden(false);
			}

			const auto worldY = static_cast<float>(height - y);

			const auto middle = Vector{ width / 2.f, height / 2.f };
			const auto clickDir = Vector{ static_cast<float>(x), worldY };

			setDir = clickDir - middle;
			const auto setDirLength = setDir.length();
			const auto angle = getAngleBetweenVectors(initDir, setDir);
			const auto XScale = clamp(0.f, 2.f, setDirLength / (width / 4.f));
			auto newTransform = dirArrowRef.getTransform();
			
			length = 5.f + XScale * 37.5f;

			newTransform.scale.X = XScale;
			newTransform.rotation.angle = ((clickDir.X < middle.X) ? 1.f : -1.f) * angle;
			dirArrowRef.setTransform(newTransform);

			glutPostRedisplay();
		}

		void hideArrow()
		{
			dirArrow.get().setHidden(true);
		}

		Vector getArrowPoint() const noexcept
		{
			const auto normalizedSetDir = normalize(setDir);

			const auto arrowDir = Vector{
				normalizedSetDir.X,
				0.f,
				-normalizedSetDir.Y
			};

			return arrowDir * length;
		}

		static const std::string tag;

	private:

		std::reference_wrapper<DirArrow> dirArrow;
		Vector setDir;
		float length;
	};

	const std::string ArrowHandler::tag{ "ArrowHandler" };

	std::unique_ptr<Ball> createBall()
	{
		return std::make_unique<Ball>(Vector{},
			Transform{ { 0.0, 0.0, -22.0 },
			  {1.f, 1.f, 1.f},
			  {
				0.f, {0.f, 0.f, 1.f}
			  }
			}
		);
	}

	std::unique_ptr<Wall> createTable()
	{
		return std::make_unique<Wall>(
			 Transform(Vector{0.f, 0.f, -20.f}, Vector{0.25f, 15.f, 12.f},{90.0, Vector{0.0, 0.0, 1.0}}));
	}

	std::unique_ptr<Wall> createVerticalWallWidth(float height, const Wall& table)
	{
		const auto transform = table.getTransform();
		const auto size = table.getSize();

		return std::make_unique<Wall>(
			Transform{ {0.f, height + size / 2, transform.translation.Z}, {transform.scale.X, height, transform.scale.Z} }, size);
	}
	
	std::unique_ptr<Wall> createVerticalWallLength(float height, const Wall& table)
	{
		const auto transform = table.getTransform();
		const auto size = table.getSize();

		return std::make_unique<Wall>(
			Transform{ {0.f, height + size / 2, transform.translation.Z}, {transform.scale.Y, height, transform.scale.X} }, size);
	}

	void tick(float deltaTime)
	{
		glTranslated(0.f, -20.f, -25.f);
		glRotated(45., 1., 0., 0.);

		glRotatef(Zangle, 0.0, 0.0, 1.0);
		glRotatef(Yangle, 0.0, 1.0, 0.0);
		glRotatef(Xangle, 1.0, 0.0, 0.0);

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
		glutPostRedisplay();
		glutTimerFunc(animationPeriod, animate, 1);
	}

	std::unique_ptr<Wall> createVertWallLeftRight(bool left, float height, const Wall& table)
	{
		auto vertWall = createVerticalWallWidth(height, table);
		const auto tableTransform = table.getTransform();
		auto vertWallTransform = vertWall->getTransform();

		const float mult = left ? 1.f : -1.f;

		vertWallTransform.translation.X += -1 * mult * (tableTransform.scale.Y * table.getSize()) / 2 
									  + mult * (vertWallTransform.scale.X * vertWall->getSize() / 2);
		vertWall->setTransform(vertWallTransform);

		return vertWall;
	}

	std::unique_ptr<Wall> createVertWallTopBottom(bool top, float height, const Wall& table)
	{
		auto vertWall = createVerticalWallLength(height, table);
		const auto tableTransform = table.getTransform();
		auto vertWallTransform = vertWall->getTransform();

		const float mult = top ? 1.f : -1.f;

		vertWallTransform.translation.Z += -1 * mult * (tableTransform.scale.Z * table.getSize()) / 2;
											 + mult * (vertWallTransform.scale.Z * vertWall->getSize() / 2);
		vertWall->setTransform(vertWallTransform);
		
		return vertWall;
	}

	void initBounds(const Wall& wallLeft, const Wall& wallRight, const Wall& wallTop, const Wall& wallDown)
	{
		minX = wallLeft.getTransform().translation.X;
		maxX = wallRight.getTransform().translation.X;
		minZ = wallTop.getTransform().translation.Z;
		maxZ = wallDown.getTransform().translation.Z;
	}

	void initActors()
	{
		auto table = createTable();
		auto ball = createBall();
	
		const auto tableTransform = table->getTransform();

		const auto ballSurfaceOffset = table->getSize() * tableTransform.scale.X / 2;
		auto newBallTransform = ball->getTransform();
		newBallTransform.translation.Y += ballSurfaceOffset;
		ball->setTransform(newBallTransform);

		const auto height = 5.f;

		auto vertWallLeft = createVertWallLeftRight(true, height, *table);
		auto vertWallRight = createVertWallLeftRight(false, height, *table);
		auto vertWallTop = createVertWallTopBottom(true, height, *table);
		auto vertWallBottom = createVertWallTopBottom(false, height, *table);

		initBounds(*vertWallLeft, *vertWallRight, *vertWallTop, *vertWallBottom);

		const auto& loc = ball->getTransform().translation;
		auto dirArrow = std::make_unique<DirArrow>(*ball, 10.f);

		auto arrowHandler = std::make_unique<ArrowHandler>(*dirArrow);

		actors.push_back(std::move(ball));
		actors.push_back(std::move(table));
		actors.push_back(std::move(vertWallLeft));
		actors.push_back(std::move(vertWallRight));
		actors.push_back(std::move(vertWallTop));
		actors.push_back(std::move(vertWallBottom));
		actors.push_back(std::move(dirArrow));
		actors.push_back(std::move(arrowHandler));
	}

	void setup(void)
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glEnable(GL_DEPTH_TEST);
		initActors();
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
		case 'r':
		case 'R':
			break;
		default:
			break;
		}
	}

	void printInteraction(void)
	{
		std::cout << "Interaction:" << std::endl;
		std::cout << "Press left mouse button and while holding it move mouse around, you'll see a direction arrow, use it wisely\n";
	}

	template<typename T>
	T& getActor()
	{
		T* r{};
		
		for (auto& actor : actors)
		{
			for (const auto& tag : actor->tags)
			{
				if (tag == T::tag)
					r = static_cast<T*>(actor.get());
			}
		}

		if (r == nullptr)
		{
			std::cerr << "can't find " << T::tag << '\n';
			std::exit(1);
		}

		return *r;
	}

	void mouseCallback(int button, int state, int x, int y)
	{
		static auto& arrowHandler{ getActor<ArrowHandler>() };
		static auto& ball{ getActor<Ball>() };

		if (button == GLUT_LEFT_BUTTON)
		{
			if (state == GLUT_DOWN)
			{
				arrowHandler.setArrow(x, y);
			}
			else if (state == GLUT_UP)
			{
				arrowHandler.hideArrow();

				auto newVelocity = arrowHandler.getArrowPoint();
				ball.setVelocity(newVelocity);
				glutPostRedisplay();
			}
		}
	}

	void motionCallback(int x, int y)
	{
		static auto& arrowHandler{ getActor<ArrowHandler>() };

		arrowHandler.setArrow(x, y);
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
		glutCreateWindow("BallRollOnPoolTable");
		glutDisplayFunc(drawScene);
		glutReshapeFunc(resize);
		glutKeyboardFunc(keyInput);
		glutMouseFunc(mouseCallback);
		glutMotionFunc(motionCallback);

		glewExperimental = GL_TRUE;
		glewInit();

		setup();

		glutMainLoop();

		return 0;
	}
}
