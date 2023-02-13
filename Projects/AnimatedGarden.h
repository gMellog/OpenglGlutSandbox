#define _USE_MATH_DEFINES 

#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <iostream>
#include <random>
#include <vector>

namespace AnimatedGarden
{
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

struct Color
{
	Color(float pR = 0.f, float pG = 0.f, float pB = 0.f)
		:
		R{ pR },
		G{ pG },
		B{ pB }
	{

	}

	float R;
	float G;
	float B;
};


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

struct Random
{
	static float getRandomFloat(float from, float to)
	{
		std::uniform_real_distribution<float> di(from, to);
		return di(mt);
	}

	static int getRandomInt(int from, int to)
	{
		std::uniform_int_distribution<int> di(from, to);
		return di(mt);
	}

private:
	static std::mt19937 mt;
};

std::mt19937 Random::mt{ static_cast<unsigned int>(time(nullptr)) };

static float Xangle = 0.0, Yangle = 260.0, Zangle = 0.0;
static bool isAnimate;
static int animationPeriod = 40;

void drawHemisphere(float radius, int longSlices, int latSlices)
{
	int  i, j;
	for (j = 0; j < latSlices; j++)
	{
		glBegin(GL_TRIANGLE_STRIP);
		for (i = 0; i <= longSlices; i++)
		{
			glVertex3f(radius * cos((float)(j + 1) / latSlices * M_PI / 2.0) * cos(2.0 * (float)i / longSlices * M_PI),
				radius * sin((float)(j + 1) / latSlices * M_PI / 2.0),
				-radius * cos((float)(j + 1) / latSlices * M_PI / 2.0) * sin(2.0 * (float)i / longSlices * M_PI));
			glVertex3f(radius * cos((float)j / latSlices * M_PI / 2.0) * cos(2.0 * (float)i / longSlices * M_PI),
				radius * sin((float)j / latSlices * M_PI / 2.0),
				-radius * cos((float)j / latSlices * M_PI / 2.0) * sin(2.0 * (float)i / longSlices * M_PI));
		}
		glEnd();
	}
}

void drawCircle(float radius, int numVertices)
{
	int i;
	float t;
	glBegin(GL_LINE_LOOP);
	for (i = 0; i < numVertices; ++i)
	{
		t = 2 * M_PI * i / numVertices;
		glVertex3f(radius * cos(t), radius * sin(t), 0.0);
	}
	glEnd();
}

template<typename T1, typename T2>
T1 interpolate(T1 a, T1 b, T2 t)
{
	return (1 - t) * a + t * b;
}

struct Sepal
{
	Sepal(float pHemisphereScaleFactor)
		:
		hemisphereScaleFactor{ pHemisphereScaleFactor }
	{

	}

	void draw()
	{
		glColor3f(1.0, 0.0, 0.0);
		glPushMatrix();
		glRotatef(90.0, 0.0, 0.0, 1.0);

		glScalef(hemisphereScaleFactor, 1.0, hemisphereScaleFactor);
		drawHemisphere(2.0, 6, 6);

		glPopMatrix();
	}

private:

	float hemisphereScaleFactor;
};

struct Petal
{
	Petal(float pHemisphereScaleFactor,
		float pPetalOpenAngle,
		float pPetalAspectRatio
	)
		:
		hemisphereScaleFactor{ pHemisphereScaleFactor },
		petalOpenAngle{ pPetalOpenAngle },
		petalAspectRatio{ pPetalAspectRatio }
	{

	}

	void draw(float angle, const Color& color)
	{
		glColor3f(color.R, color.G, color.B);
		glPushMatrix();

		glRotatef(angle, 1.0, 0.0, 0.0);
		glTranslatef(2.0, 0.0, 2.0 * hemisphereScaleFactor);

		glTranslatef(-2.0, 0.0, 0.0);
		glRotatef(petalOpenAngle, 0.0, 1.0, 0.0);
		glTranslatef(2.0, 0.0, 0.0);

		glScalef(1.0, petalAspectRatio, 1.0);
		drawCircle(2.0, 10);
		glPopMatrix();
	}

private:

	float hemisphereScaleFactor;
	float petalOpenAngle;
	float petalAspectRatio;
};

struct Stem
{
	void draw(float angle)
	{
		glRotatef(angle, 0.0, 0.0, 1.0);
		glColor3f(0.0, 1.0, 0.0);
		glLineWidth(3.0);
		glBegin(GL_LINES);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(5.0, 0.0, 0.0);
		glEnd();
	}
};

struct Leaf
{
	void draw(bool verse, float t)
	{
		const std::vector<Vector> v{
			{0.f, 0.f, 0.f},
			{-1.5f, 2.5f, 0.f},
			{0.f, 8.5f, 0.f},
			{1.5f, 2.5f, 0.f}
		};

		const auto mult = (verse ? 1.f : -1.f);

		const float leafScale = interpolate(0.0f, 0.75f * mult, t);

		glEnableClientState(GL_VERTEX_ARRAY);

		glVertexPointer(3, GL_FLOAT, 0, v.data());

		glColor3f(0.f, 1.f, 0.f);
		glLineWidth(3.f);

		glPushMatrix();

		glRotated(-30.f * mult, 0.f, 1.f, 0.f);
		glScaled(leafScale, leafScale, leafScale);
		glRotated(-Yangle, 0.f, 1.f, 0.f);
		glRotated(-45.f, 0.f, 0.f, 1.f);
		glRotated(-120.f, 0.f, 1.f, 0.f);
		glDrawArrays(GL_LINE_LOOP, 0, v.size());

		glPopMatrix();
		glDisableClientState(GL_VERTEX_ARRAY);
	}
};

struct Flower
{
	Flower(const Transform& pTransform, float pTMult)
		:
		transform{ pTransform },
		t{},
		windT{},
		tMult{ pTMult },
		RWind{},
		petalColor{ initPetalColor() }
	{

	}

	void tick()
	{
		if (t >= 1.f)
		{
			if (RWind == 0.f)
				RWind = Random::getRandomFloat(3.f, 10.f);

			windT += 0.1;
			if (windT >= 2 * M_PI)
			{
				windT -= 2 * M_PI;
				RWind = Random::getRandomFloat(3.f, 10.f);
			}
		}

		if (!(t >= 1.f))
			t += 0.01 * tMult;

		draw();
	}

private:

	Transform transform;
	float t;
	float tMult;
	float windT;
	float RWind;
	Color petalColor;

	Color initPetalColor()
	{
		const std::vector<Color> v{
			{1.f, 0.f, 0.f},
			{1.f, 0.f, 1.f},
			{1.f, 1.f, 0.f},
			{0.f, 1.f, 1.f}
		};

		return v[Random::getRandomInt(0, v.size() - 1)];
	}

	void draw()
	{
		const auto& loc = transform.translation;
		const auto& scale = transform.scale;
		const auto& rot = transform.rotation;

		glPushMatrix();

		glTranslated(loc.X, loc.Y, loc.Z);
		glRotated(rot.angle, rot.dirs.X, rot.dirs.Y, rot.dirs.Z);
		glScaled(scale.X, scale.Y, scale.Z);

		float angleFirstSegment = interpolate(60.f, 80.f, t);
		float angleSecondSegment = interpolate(-30.f, -20.f, t);
		float angleThirdSegment = interpolate(-30.f, -20.f, t);
		float angleFourthSegment = interpolate(-30.f, -20.f, t);

		float hemisphereScaleFactor = interpolate(0.1f, 0.75f, t);
		float petalAspectRatio = interpolate(0.1f, 1.f, t);
		float petalOpenAngle = interpolate(-10.f, -60.f, t);

		Sepal sepal{ hemisphereScaleFactor };
		Petal petal{ hemisphereScaleFactor, petalOpenAngle, petalAspectRatio };
		Stem stem{};
		Leaf leaf;

		stem.draw(angleFirstSegment + RWind * sin(windT));

		glTranslatef(5.0, 0.0, 0.0);

		stem.draw(angleSecondSegment);

		glTranslatef(5.0, 0.0, 0.0);

		leaf.draw(true, t);
		leaf.draw(false, t);

		stem.draw(angleThirdSegment);

		glTranslatef(5.0, 0.0, 0.0);
		stem.draw(angleFourthSegment);

		glTranslatef(7.0, 0.0, 0.0);
		sepal.draw();

		petal.draw(30.f, petalColor);
		petal.draw(90.f, petalColor);
		petal.draw(150.f, petalColor);
		petal.draw(210.f, petalColor);
		petal.draw(270.f, petalColor);
		petal.draw(330.f, petalColor);

		glPopMatrix();
	}
};

std::vector<Flower> flowers;

void initFlowers()
{
	flowers.push_back(Flower({ {0.f, 0.f, 7.5f}, {0.5f, 0.5f, 0.5f} }, Random::getRandomFloat(0.5f, 1.f)));
	flowers.push_back(Flower({ {-5.f, 0.f, 7.5f}, {-0.5f, 0.5f, 0.5f} }, Random::getRandomFloat(0.5f, 1.f)));
	flowers.push_back(Flower({ {0.f, 0.f, 0.f}, {0.5f, 0.5f, 0.5f} }, Random::getRandomFloat(0.5f, 1.f)));
	flowers.push_back(Flower({ {-5.f, 0.f, 0.f}, {-0.5f, 0.5f, 0.5f} }, Random::getRandomFloat(0.5f, 1.f)));
	flowers.push_back(Flower({ {0.f, 0.f, -7.5f}, {0.5f, 0.5f, 0.5f} }, Random::getRandomFloat(0.5f, 1.f)));
	flowers.push_back(Flower({ {-5.f, 0.f, -7.5f}, {-0.5f, 0.5f, 0.5f} }, Random::getRandomFloat(0.5f, 1.f)));
	flowers.push_back(Flower({ {-2.5f, 0.f, 9.f}, {0.5f, 0.5f, 0.5f}, {-90.f, {0.f, 1.f, 0.f}} }, Random::getRandomFloat(0.5f, 1.f)));
	flowers.push_back(Flower({ {-2.5f, 0.f, -9.f}, {0.5f, 0.5f, 0.5f}, {90.f, {0.f, 1.f, 0.f}} }, Random::getRandomFloat(0.5f, 1.f)));
}

void tick()
{
	for (auto& i : flowers)
		i.tick();
}

void drawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0.0, -10.0, -30.0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glRotatef(Zangle, 0.0, 0.0, 1.0);
	glRotatef(Yangle, 0.0, 1.0, 0.0);
	glRotatef(Xangle, 1.0, 0.0, 0.0);

	tick();

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

void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);

	glMatrixMode(GL_MODELVIEW);
}

void setup(void)
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	initFlowers();
}

void keyInput(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case ' ':
		if (isAnimate) isAnimate = 0;
		else
		{
			isAnimate = 1;
			animate(1);
		}
		break;
	case 127:
		if (isAnimate) isAnimate = 0;
		flowers.clear();
		initFlowers();
		glutPostRedisplay();
		break;
	case 'x':
		Xangle += 5.0;
		if (Xangle > 360.0) Xangle -= 360.0;
		glutPostRedisplay();
		break;
	case 'X':
		Xangle -= 5.0;
		if (Xangle < 0.0) Xangle += 360.0;
		glutPostRedisplay();
		break;
	case 'y':
		Yangle += 5.0;
		if (Yangle > 360.0) Yangle -= 360.0;
		glutPostRedisplay();
		break;
	case 'Y':
		Yangle -= 5.0;
		if (Yangle < 0.0) Yangle += 360.0;
		glutPostRedisplay();
		break;
	case 'z':
		Zangle += 5.0;
		if (Zangle > 360.0) Zangle -= 360.0;
		glutPostRedisplay();
		break;
	case 'Z':
		Zangle -= 5.0;
		if (Zangle < 0.0) Zangle += 360.0;
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

void printInteraction(void)
{
	std::cout << "Interaction:" << std::endl;
	std::cout << "Press space to toggle between animation on and off." << std::endl
		<< "Press delete to reset." << std::endl
		<< "Press the x, X, y, Y, z, Z keys to rotate the scene." << std::endl;
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
	glutCreateWindow("AnimatedGarden");
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