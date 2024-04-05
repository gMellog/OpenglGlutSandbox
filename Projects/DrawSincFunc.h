///////////////////////////////////////////////////////////////////////////////////////          
//Draw sinc function
/////////////////////////////////////////////////////////////////////////////////////// 

#define _USE_MATH_DEFINES 

#include <cmath>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

namespace DrawSincFunc
{
	int screenWidth = 640;
	int screenHeight = 480;

	const float R = 4.0;

	void setWindow(float left, float right, float bottom, float top)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(left, right, bottom, top);
	}

	void setViewport(int left, int right, int bottom, int top)
	{
		glViewport(left, bottom, right - left, top - bottom);
	}

	 void setup(void)
        {
                glClearColor(1.0, 1.0, 1.0, 0.0);
                glColor3f(0.f, 0.f, 0.f);
                glPointSize(4.0);
        }

	struct BoxBounds
	{
		BoxBounds(float pL = 0.f, float pR = 0.f, float pB = 0.f, float pT = 0.f)
		:
		l{pL},r{pR},
		b{pB},t{pT}	
		{}

		float l;
		float r;
		float b;
		float t;

	} world{-4.f, 4.f, -1.f, 1.f}, viewport{};

	std::ostream& operator<<(std::ostream& os, const BoxBounds& bbs)
	{
		os << "l: " << bbs.l << " r: " << bbs.r << " b: " << bbs.b << " t: " << bbs.t;

		return os;
	}

	struct Vector
	{
		Vector(float pX = 0.f, float pY = 0.f)
			:
			X{pX}, Y{pY}
		{
		}

		float X;
		float Y;
	};

	std::ostream& operator<<(std::ostream& os, const Vector& v)
	{
		os << "X: " << v.X << " Y: " << v.Y << '\n'; 

		return os;
	}

	struct Extent
	{
		Extent()
		:
		pos{},
		width{},
		height{}
		{}

		Vector pos;
		float width;
		float height;
	};
	
	Extent GetExtent(const std::vector<Vector>& points)
	{
		Extent r;
	
		float minX = screenWidth;
		float minY = screenHeight;
		float maxX{};
		float maxY{};

		for(const auto& i : points)
		{
			minX = std::min(i.X, minX);
			minY = std::min(i.Y, minY);
	
			maxX = std::max(i.X, maxX);
			maxY = std::max(i.Y, maxY);
		}

		r.pos = Vector{minX, minY};
		r.width = maxX - minX;
		r.height = maxY - minY;

		return r;
	}

	struct ExtentPoints
	{
		Vector leftBottom;
		Vector leftTop;
		Vector rightTop;
		Vector rightBottom;
	};

	ExtentPoints GetExtentPoints(const Extent& extent)
	{
		ExtentPoints r;

		r.leftBottom = extent.pos;
		r.leftTop = Vector{ r.leftBottom.X, r.leftBottom.Y + extent.height };
		r.rightTop = Vector{ r.leftTop.X + extent.width, r.leftTop.Y };
		r.rightBottom = Vector{ r.rightTop.X, r.leftBottom.Y };
		
		return r;
	}

	std::vector<Vector> GetPoints()
	{
		std::vector<Vector> r;

		for(GLfloat x = -4.0; x < 4.0; x += 0.1)
			r.push_back(Vector{x, std::sin(M_PI * x) / (M_PI * x)});

		return r;
	}
	
	void ShowExtPoints(const ExtentPoints& ExtPoints)
	{
		std::cout << "ExtPoints are:\n" << '\n';
		std::cout << ExtPoints.leftBottom << '\n';
		std::cout << ExtPoints.leftTop << '\n';
		std::cout << ExtPoints.rightTop << '\n';
		std::cout << ExtPoints.rightBottom << '\n';
	}


	void drawScene(void)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		
		const auto GraphPoints = GetPoints();		
		const auto Ext = GetExtent(GraphPoints);
		
		const auto ExtPoints = GetExtentPoints(Ext);		
		
		glBegin(GL_LINE_LOOP);

		glVertex2f(ExtPoints.leftBottom.X, ExtPoints.leftBottom.Y);
		glVertex2f(ExtPoints.leftTop.X, ExtPoints.leftTop.Y);
		glVertex2f(ExtPoints.rightTop.X, ExtPoints.rightTop.Y);
		glVertex2f(ExtPoints.rightBottom.X, ExtPoints.rightBottom.Y);

		glEnd();
		
		glBegin(GL_LINE_STRIP);
		
		for(const auto& p : GraphPoints)
		{
			glVertex2f(p.X, p.Y);
		}

		glEnd();
		
		glFlush();
		
	}

	void ShowABCD()
	{

		const auto A = (viewport.r - viewport.l) / (world.r - world.l);
		const auto C = (viewport.t - viewport.b) / (world.t - world.b);
		const auto B = viewport.l - A * world.l;
		const auto D = viewport.b - C * world.b;

		std::cout << "World to Viewport coefficients are:\n";
		std::cout << "A: " << A << '\n';
		std::cout << "B: " << B << '\n';
		std::cout << "C: " << C << '\n';
		std::cout << "D: " << D << '\n';
	}

	void resize(int w, int h)
	{
		std::cout << "w: " << w << " h: " << h << '\n';
		setWindow(world.l, world.r, world.b, world.t);
		
		if( R > (w / h) )
		{
			
			const auto leftX = (screenWidth / 2) - (w / 2);
			const auto bottomY = (screenHeight / 2) - ( (w / R) / 2);
			glViewport(leftX, bottomY, w, w / R);
			
			viewport.l = leftX;
			viewport.r = leftX + w;
			viewport.b = bottomY;
			viewport.t = bottomY + w / R;
			//setViewport(0, w, 0, w / R);
			//viewport.l = 
		}
		else
		{
			
			const auto leftX = (screenWidth / 2) - ((h * R) / 2);
			const auto bottomY = (screenHeight / 2) - ( h / 2);
			glViewport(leftX, bottomY, h * R, h);
			//setViewport(0, h * R, 0, h);
		
			viewport.l = leftX;
			viewport.r = leftX + (h * R);
			viewport.b = bottomY;
			viewport.t = bottomY + h;
		}

		screenWidth = w;
		screenHeight = h;	

		ShowABCD();
	}

	void keyInput(unsigned char key, int x, int y)
	{
		switch (key)
		{
		case 27:
			exit(0);
			break;
		}
	}

	void printInteraction(void)
	{
		std::cout << "Just shows sinc function.\n";
		std::cout << "But you can quit with using of ESC!\n";
	}

	int main(int argc, char** argv)
	{
		printInteraction();
		glutInit(&argc, argv);

		glutInitContextVersion(4, 3);
		glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
		glutInitWindowSize(screenWidth, screenHeight);
		glutInitWindowPosition(100, 100);
		glutCreateWindow("Sinc example");
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
