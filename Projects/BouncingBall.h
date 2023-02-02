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

namespace BouncingBall
{
	static bool isAnimate;
	static bool showSimulationParams{1};
	static int animationPeriod = 25;
	static float g = 10.1f;

	std::chrono::system_clock::time_point tp;

	struct Vector
	{
		Vector(float pX = 0.f, float pY = 0.f, float pZ = 0.f)
			:
			X{pX},
			Y{pY},
			Z{pZ}
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
			return {X + rhs.X, Y + rhs.Y, Z + rhs.Z};
		}

		Vector operator*(float v) const noexcept
		{
			return { X * v, Y * v, Z * v};
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
			angle{pAngle},
			dirs{pDirs}
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
			translation{pTranslation},
			scale{pScale},
			rotation{pRotation}
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

	std::vector<std::reference_wrapper<Actor>> actors;

	Vector initialBallVelocity = { 10.f, 3.f, 0.f };
	Vector initialBallLocation = { -5.0, 4.5, -15.0 };

	struct Ball : Actor
	{
		Ball(const Vector& pVelocity, const Vector& pLoc)
			:
			velocity{ pVelocity },
			loc{pLoc},
			radius{2.f},
			springiness{1.f}
		{

		}

		void tick(float deltaTime) override
		{
			if (isAnimate)
			{ 

				Vector velocityChange;
		
				for (const auto forceChange : forceChangers)
					velocityChange += forceChange(deltaTime);
				
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

		std::vector<std::function<Vector(float)>> forceChangers;

	private:
		Vector velocity;
		Vector loc;
		float radius;
		float springiness;

		void draw()
		{
			glPushMatrix();
			glTranslated(loc.X, loc.Y, loc.Z);
			glColor3d(1.0, 0.0, 0.0);
			glutWireSphere(radius, 10, 10);
			glPopMatrix();
		}

	} ball{ Vector{initialBallVelocity}, Vector{initialBallLocation} };

	const std::string Ball::collideType{ "Ball" };
	
	static const std::string collideType{ "Wall" };

	struct VerticalWall : Actor
	{
		VerticalWall(const Transform& pTransform)
			:
			transform{pTransform},
			size{5.f}
		{

		}

		void tick(float deltaTime) override
		{
			draw();
		}

		void collideWith(Actor& actor) override
		{
			if (actor.getCollideType() == Ball::collideType)
			{
				auto& ball = (Ball&)actor;
				if (insideBox(ball))
				{
					const auto ballDir = ball.getDir();
					const auto springiness = ball.getSpringiness();
					ball.setDir({ ballDir.X * -0.5f * springiness, ballDir.Y * -0.5f * springiness, -ballDir.Z});
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

		Transform transform;
		float size;
	} 
	vertWall{ Transform(Vector{10.25, 0.1, -15.0}, Vector{0.25, 3.5, 1.0}) };

	struct HorizontalWall : Actor
	{
		HorizontalWall(const Transform& pTransform)
			:
			transform{ pTransform },
			size{ 5.f }
		{

		}

		void tick(float deltaTime) override
		{
			draw();
		}

		void collideWith(Actor& actor) override
		{
			if (actor.getCollideType() == Ball::collideType)
			{
				auto& ball = (Ball&)actor;
				if (insideBox(ball)) 
				{
					const auto ballVel = ball.getVelocity();
					if (!isNearlyEqual(ballVel.length(), 0, 0.2))
					{
						const auto ballDir = ball.getDir();
						ball.setDir({ ballDir.X * 0.75f, -ballDir.Y * 0.5f, ballDir.Z });
					}
					else if (ballVel.length() != 0)
					{
						ball.setVelocity({});
						ball.forceChangers.clear();
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

		bool insideBox(const Ball& ball) const noexcept
		{
			const auto R = ball.getRadius();
			const auto dir = ball.getDir();
			const auto loc = ball.getTransform().translation;

			const auto point = loc + dir * R;

			const auto minX = transform.translation.X - size / 2 * transform.scale.Y;
			const auto maxX = transform.translation.X + size / 2 * transform.scale.Y;
			const auto minY = transform.translation.Y - size / 2 * transform.scale.X;
			const auto maxY = transform.translation.Y + size / 2 * transform.scale.X;
			const auto minZ = transform.translation.Z - size / 2 * transform.scale.Z;
			const auto maxZ = transform.translation.Z + size / 2 * transform.scale.Z;

			return (minX <= point.X && point.X <= maxX) &&
				(minY <= point.Y && point.Y <= maxY) &&
				(minZ <= point.Z && point.Z <= maxZ);
		}

		Transform transform;
		float size;
	} 
	horizWall{ Transform(Vector{0.0, -10.0, -15.0}, Vector{0.25, 4.75, 1.0},{90.0, Vector{0.0, 0.0, 1.0}}) };

	void checkCollide()
	{
		for (const auto& actor1 : actors)
		{
			for (const auto& actor2 : actors)
			{
				if ((&actor1.get() != &actor2.get()))
				{
					actor1.get().collideWith(actor2);
				}
			}
		}
	}

	void tick(float deltaTime)
	{
		for (const auto& actor : actors)
			actor.get().tick(deltaTime);
	}

	void glutBitmapStr(void* font, const std::string& str)
	{
		for (const auto ch : str) glutBitmapCharacter(font, ch);
	}

	void glutStrokeStr(void* font, const std::string& str)
	{
		for (const auto ch : str) glutStrokeCharacter(font, ch);
	}


	int getSetWSize(const std::string& str)
	{
		int r{};

		if (!str.empty())
			r = str.back() != '0' ? str.size() : str.size() - 1;

		return r;
	}

	void drawParam(const std::string& label, float param, const Vector& rasterPos)
	{
		std::stringstream ss;
		std::stringstream val;

		val << param;
		ss << label << std::fixed << std::setw(val.str().size() - 1) << std::setprecision(1) << std::showpoint << param;
		glRasterPos3d(rasterPos.X, rasterPos.Y, rasterPos.Z);
		glutBitmapStr(GLUT_BITMAP_TIMES_ROMAN_24, ss.str());
	}

	void drawSetParams()
	{
 		if (!isAnimate && showSimulationParams)
		{
			glColor3f(1.f, 0.f, 0.f);
			glLineWidth(2.8f);

			glPushMatrix();
			glTranslated(-8.0, 9.0, -15.0);
			glScaled(0.0125, 0.0125, 0.0125);
			glutStrokeStr(GLUT_STROKE_ROMAN, "SET PARAMS MODE");
			glPopMatrix();

			glLineWidth(1.f);

			glColor3f(0.f, 0.f, 0.f);
			
			drawParam("vX: ", initialBallVelocity.X, {-7.5f, 0.7f, -15.f});
			drawParam("vY: ", initialBallVelocity.Y, { -7.5f, -1.3f, -15.f });

			glRasterPos3d(-7.5f, -3.3f, -15.f);
			glutBitmapStr(GLUT_BITMAP_TIMES_ROMAN_24, "springniness ");

			drawParam("right wall: ", ball.getSpringiness(), { -7.5f, -5.3f, -15.f });
			drawParam("g: ", g, { -7.5f, -7.3f, -15.f });
		}
	}

	void drawScene(void)
	{
		using namespace std::chrono;

		glClear(GL_COLOR_BUFFER_BIT);
		glLoadIdentity();

		auto d = duration_cast<milliseconds>(system_clock::now() - tp);
		auto deltaTime = float(d.count()) * milliseconds::period::num / milliseconds::period::den;
		tp = system_clock::now();
		
		tick(deltaTime);
		checkCollide();

		drawSetParams();

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

	void addGravityToBall()
	{
		ball.forceChangers.push_back([](float deltaTime) {
			Vector r;

			r.X = 0.f;
			r.Y = -g * deltaTime;

			return r;
			});
	}

	void initActors()
	{
		addGravityToBall();
		actors.push_back(ball);
		actors.push_back(vertWall);
		actors.push_back(horizWall);
	}

	void reinitBall()
	{
		isAnimate = false;
		showSimulationParams = true;

		ball.setVelocity(initialBallVelocity);
		ball.setLocation(initialBallLocation);
		ball.forceChangers.clear();
		addGravityToBall();

		glutPostRedisplay();
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
			isAnimate = !isAnimate;
			
			if(showSimulationParams)
				showSimulationParams = false;
			
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

		case 'W':
		case 'w':
			initialBallLocation.Y += 0.1f;
			reinitBall();
			break;
		case 'S':
		case 's':
			initialBallLocation.Y -= 0.1f;
			reinitBall();
			break;
		case 'A':
		case 'a':
			initialBallLocation.X -= 0.1f;
			reinitBall();
			break;
		case 'D':
		case 'd':
			initialBallLocation.X += 0.1f;
			reinitBall();
			break;
		default:
			break;
		}
	}

	void specialKeyInput(int key, int x, int y)
	{
		if (key == GLUT_KEY_UP) initialBallVelocity.Y += 0.05f;
		if (key == GLUT_KEY_DOWN) if (initialBallVelocity.Y > 0.1f) initialBallVelocity.Y -= 0.05f;
		if (key == GLUT_KEY_RIGHT) initialBallVelocity.X += 0.05f;
		if (key == GLUT_KEY_LEFT) if (initialBallVelocity.X > 0.1f) initialBallVelocity.X -= 0.05f;
		if (key == GLUT_KEY_PAGE_UP) ball.setSpringiness(ball.getSpringiness() + 0.01f);
		if (key == GLUT_KEY_PAGE_DOWN) if(ball.getSpringiness() > 0.1f) ball.setSpringiness(ball.getSpringiness() - 0.01f);
		if (key == GLUT_KEY_HOME) g += 0.1f;
		if (key == GLUT_KEY_END) if (g > 0.1f) g -= 0.1f;

		reinitBall();
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

		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
		glutInitWindowSize(500, 500);
		glutInitWindowPosition(100, 100);
		glutCreateWindow("BouncingBall");
		glutDisplayFunc(drawScene);
		glutReshapeFunc(resize);
		glutKeyboardFunc(keyInput);
		glutSpecialFunc(specialKeyInput);

		glewExperimental = GL_TRUE;
		glewInit();

		setup();

		glutMainLoop();

		return 0;
	}
}
