///////////////////////////////////////////////////////////////////////////////////////          
//Usage of Cohen-Sazerland clip algorithm
/////////////////////////////////////////////////////////////////////////////////////// 

#define _USE_MATH_DEFINES 

#include <cmath>
#include <vector>
#include <iostream>
#include <random>
#include <GL/glew.h>
#include <GL/freeglut.h>

namespace TestCohenSazerland
{
	int screenWidth = 640;
	int screenHeight = 480;

	const float R = 1.0;

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

	struct RealRect
	{
		RealRect(float pL = 0.f, float pR = 0.f, float pB = 0.f, float pT = 0.f)
			:
			l{pL}, t{pT}, r{pR}, b{pB}
		{}

		float l;
		float t;
		float r;
		float b;
	} world{0.f, screenWidth, 0.f, screenHeight}, viewport{world};

	std::ostream& operator<<(std::ostream& os, const RealRect& rect)
	{
		os << "l: " << rect.l << " r: " << rect.r << " b: " << rect.b << " t: " << rect.t;
		return os;
	}

	struct RectangleSetter
	{
		RectangleSetter()
			:
		v1{}, v2{}, interV2{}, v1Ready{}, v2Ready{}
		{}

		bool IsV1Ready() const noexcept
		{
			return v1Ready;
		}

		bool IsCompleted() const noexcept
		{
			return v1Ready && v2Ready;
		}

		Vector GetV1() const noexcept { return v1; }
		void SetV1(const Vector& newV1) noexcept
		{
			v1Ready = true;
			v1 = newV1;
		}

		Vector GetV2() const noexcept { return v2; }
		void SetV2(const Vector& newV2) noexcept
		{
			v2Ready = true;
			v2 = newV2;
		}

		Vector GetInterV2() const noexcept { return interV2; }
		void SetInterV2(const Vector& newInterV2)
		{
			interV2 = newInterV2;
		}	

		RealRect static GetRect(const Vector& v1, const Vector& v2)
		{
			RealRect r;

			const auto lowerX = std::min(v1.X, v2.X);
			const auto maxX = std::max(v1.X, v2.X);
			const auto lowerY = std::min(v1.Y, v2.Y);
			const auto maxY = std::max(v1.Y, v2.Y);

			r = {lowerX, maxX, lowerY, maxY};

			return r;
		}
		
		private:
		
		Vector v1;
		Vector v2;
		Vector interV2;

		bool v1Ready;
		bool v2Ready;
	
	} rectSetter;


	std::ostream& operator<<(std::ostream& os, const Vector& v)
	{
		os << "X: " << v.X << " Y: " << v.Y << '\n'; 
		return os;
	}

	using Line = std::pair<Vector, Vector>;
	
	std::vector<Line> lines;

	void InitLines()
	{
		const auto randLines = 100;
		
		lines.reserve(randLines);
		
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_real_distribution<> distX(0.f, screenWidth - 1.f);
		std::uniform_real_distribution<> distY(0.f, screenHeight - 1.f);

		for(int i = 0; i < randLines; ++i)
		{
			const auto line = Line{ {distX(rng), distY(rng)}, {distX(rng), distY(rng)} };
			lines.push_back(line);
		}	
	}

	void setup(void)
        {
		InitLines();
                glClearColor(1.0, 1.0, 1.0, 0.0);
                glColor3f(0.f, 0.f, 0.f);
                glPointSize(4.0);
        }

	struct CohenSazerland
	{
		static bool ClipSegment(Vector& v1, Vector& v2, const RealRect& pWindow)
		{
			window = pWindow;
			return ClipSegmentImpl(v1, v2);
		}

	private:
		enum LOCATIONS{LEFT = 8, TOP = 4, RIGHT = 2, BOTTOM = 1};

		static bool IsOutside(const unsigned char code)
		{
			return code != 0;
		}

		static unsigned char GetCodesForPoint(const Vector& v)
		{
			unsigned char code{};
			
			if(v.X < window.l) code |= LEFT;
			if(v.Y > window.t) code |= TOP;
			if(v.X > window.r) code |= RIGHT;
			if(v.Y < window.b) code |= BOTTOM;

			return code;
		}


		static bool IsTrivialAccept(const unsigned char code1, const unsigned char code2)
		{
			return !(code1 | code2);
		}

		static bool IsTrivialReject(const unsigned char code1, const unsigned char code2)
		{
			return (code1 & code2);
		}

		static void ChopLine(Vector& v, unsigned char code, float delx, float dely)
		{
			if(code & LEFT)
			{
				v.Y += (window.l - v.X) * dely / delx;
				v.X = window.l;
			}
			else if(code & RIGHT)
			{
				v.Y += (window.r - v.X) * dely / delx;
				v.X = window.r;
			}
			else if(code & BOTTOM)
			{
				v.X += (window.b - v.Y) * delx / dely;
				v.Y = window.b;
			}
			else if(code & TOP)
			{
				v.X += (window.t - v.Y) * delx / dely;
				v.Y = window.t;
			}
		}

		static bool IsPointInBounds(const Vector& v)
		{
			return ((v.X >= window.l) && (v.X <= window.r))
				&&
			       ((v.Y >= window.b) && (v.Y <= window.t));
		}

		static bool ClipSegmentImpl(Vector& v1, Vector& v2)
		{
			for(int i = 0; i < 4; ++i)
			{
				const auto code1 = GetCodesForPoint(v1);
				const auto code2 = GetCodesForPoint(v2);
				
				if(IsTrivialAccept(code1, code2)) return true;
				if(IsTrivialReject(code1, code2)) return false;
			
				const auto delx = v2.X - v1.X;
				const auto dely = v2.Y - v1.Y;	

				ChopLine(v1, code1, delx, dely);
				ChopLine(v2, code2, delx, dely);
			}

			return IsPointInBounds(v1) && IsPointInBounds(v2);
		}

		static RealRect window;
	};

	RealRect CohenSazerland::window{};		

	void DrawWindowBounds(const RealRect& window)
	{
		glBegin(GL_LINE_LOOP);
			glVertex2f(window.l, window.b);
			glVertex2f(window.l, window.t);
			glVertex2f(window.r, window.t);
			glVertex2f(window.r, window.b);
		glEnd();
	}

	void DrawSettingWindow()
	{
		glPointSize(5.f);
		glColor3f(0.f, 0.f, 0.f);
		if(rectSetter.IsV1Ready())
		{
			const auto v1 = rectSetter.GetV1();
			
			glBegin(GL_POINTS);
				glVertex2f(v1.X, v1.Y);
			glEnd();
			
			const auto window = rectSetter.GetRect(rectSetter.GetV1(), rectSetter.GetInterV2());
			DrawWindowBounds(window);
		}

		glFlush();
	}
	
	void DrawWindowAndLines()
	{
		const auto window = rectSetter.GetRect(rectSetter.GetV1(), rectSetter.GetV2());
		glColor3f(0.f, 1.f, 0.f);
		glLineWidth(3.f);
		DrawWindowBounds(window);

		glLineWidth(1.f);
		for(const auto& i : lines)
		{
			auto v1 = i.first;
		      	auto v2 = i.second;	
			
			glBegin(GL_LINES);

			glColor3f(0.f, 0.f, 1.f);
				glVertex2f(v1.X, v1.Y);
				glVertex2f(v2.X, v2.Y);	
			
			const auto accepted = CohenSazerland::ClipSegment(v1, v2, window);
			
			if(accepted)
			{
				glColor3f(1.f, 0.f, 0.f);
					glVertex2f(v1.X, v1.Y);
					glVertex2f(v2.X, v2.Y);
			}

			glEnd();
		}

		glFlush();
	}

	void drawScene(void)
	{	
		glClear(GL_COLOR_BUFFER_BIT);
		
		if(!rectSetter.IsCompleted())
		{
			DrawSettingWindow();
			return;
		}

		DrawWindowAndLines();
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

	float GetAspectRatio(const RealRect& rect)
	{
		const auto width = rect.r - rect.l;
		const auto height = rect.t - rect.b;
		
		return (height == 0.f) ? 0.f : (width / height);
	}

	Vector viewportOffsets;

	void resize(int w, int h)
	{

		setWindow(world.l, world.r, world.b, world.t);
		
		const auto WorldWindowRatio = GetAspectRatio(world);
		const auto ScreenWindowRatio = ((float)(w) / h);
		
		viewportOffsets.X = std::max(( w - (viewport.r - viewport.l)) / 2, 0.f);
		viewportOffsets.Y = std::max((h - (viewport.t - viewport.b)) / 2, 0.f);
		
		if(WorldWindowRatio > ScreenWindowRatio)
		{
			glViewport(viewportOffsets.X, viewportOffsets.Y, w, w / WorldWindowRatio);
			
			viewport.l = 0.f;
			viewport.r = w;
			viewport.b = 0.f;
			viewport.t = w / WorldWindowRatio;
		}
		else
		{
			glViewport(viewportOffsets.X, viewportOffsets.Y, h * WorldWindowRatio, h);
	
			viewport.l = 0.f;
			viewport.r = h * WorldWindowRatio;
			viewport.b = 0.f;
			viewport.t = h;
		}
		
		screenWidth = w;
		screenHeight = h;	
	}

	void keyInput(unsigned char key, int x, int y)
	{
		switch (key)
		{
			case 27:
				exit(0);
				break;
			case 'R':
			case 'r':
				rectSetter = RectangleSetter{};
				glutPostRedisplay();
				break;
		}
	}

	void mouseHandler(int button, int state, int x, int y)
	{
		if(rectSetter.IsCompleted()) return;

		if(button == GLUT_LEFT_BUTTON)
		{
			if(state == GLUT_DOWN)
			{
				const auto v = Vector{x, screenHeight - y};
				if(!rectSetter.IsV1Ready())
				{
					rectSetter.SetV1(v);
					rectSetter.SetInterV2(v);
				}
				else
				{
					rectSetter.SetV2(v);
				}
				
				glutPostRedisplay();
			}
		}
	}

	void mouseMoving(int x, int y)
	{
		if(rectSetter.IsV1Ready())
		{
			rectSetter.SetInterV2({x, screenHeight - y});
			glutPostRedisplay();
		}
	}

	void printInteraction()
	{
		std::cout << "This program shows the possibilities of Cohen Sazerland clip algorithm\n";
		std::cout << "1. Setup window rectangle\n";
		std::cout << "2. See test sample\n";
		std::cout << "Actions: \n";
		std::cout << "1. Press (R) to restart\n";
		std::cout << "2. Quit with (ESC)\n";
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
		glutCreateWindow("Cohen-Sazerland clip");
		glutDisplayFunc(drawScene);
		glutReshapeFunc(resize);
		glutKeyboardFunc(keyInput);
		glutMouseFunc(mouseHandler);
		glutPassiveMotionFunc(mouseMoving);

		glewExperimental = GL_TRUE;
		glewInit();

		setup();

		glutMainLoop();
		
		return 0;
	}

}
