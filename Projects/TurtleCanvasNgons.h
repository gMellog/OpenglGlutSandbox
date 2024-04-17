///////////////////////////////////////////////////////////////////////////////////////          
//Example of turtle graphics with ngons
/////////////////////////////////////////////////////////////////////////////////////// 

#define _USE_MATH_DEFINES 

#include <cmath>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

namespace TurtleCanvasNgons
{
	struct Point2
	{
		Point2(float pX = 0.f, float pY = 0.f)
		:
		X{pX}, Y{pY}
		{}

		void Set(float newX, float newY)
		{
			X = newX;
			Y = newY;
		}

		void Draw()
		{
			glBegin(GL_POINTS);
				glVertex2f(X, Y);
			glEnd();
		}

		float X;
		float Y;
	};

	template<typename T>
	struct Rect
	{
		
		Rect(T pL = 0, T pR = 0, T pB = 0, T pT = 0)
		:
		l{pL}, r{pR},
		b{pB}, t{pT}
		{}

		void Set(T newL, T newR, T newB, T newT)
		{
			l = newL;
			r = newR;
			b = newB;
			t = newT;
		}

		void Draw()
		{
			glBegin(GL_LINE_LOOP);
				glVertex2d(l, b);
				glVertex2d(l, t);
				glVertex2d(r. t);
				glVertex2d(r, b);
			glEnd();
		}

		T l, r, b, t;
	};

	using IntRect = Rect<int>;
	using FloatRect = Rect<float>;

	void Display();

	struct Canvas
	{
		static Canvas& Get()
		{
			static Canvas cvs;
			return cvs;
		}

		static void Resize(int w, int h)
		{

		}

		static void KeyInput(unsigned char key, int x, int y)
		{
			switch (key)
			{
				case 27:
					exit(0);
					break;	
			}
		}

		static void MouseHandler(int button, int state, int x, int y)
		{

		}
		
		static void MouseMoving(int x, int y)
		{

		}

		void Init(int pWidth, int pHeight, const std::string& title)
		{
			width = pWidth;
			height = pHeight;

			glutInitContextVersion(4, 3);
			glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
			glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
			glutInitWindowSize(width, height);
			glutInitWindowPosition(20, 20);
			glutCreateWindow(title.c_str());
			
			glutDisplayFunc(Display);
			glutReshapeFunc(Canvas::Resize);
			glutKeyboardFunc(Canvas::KeyInput);
			glutMouseFunc(Canvas::MouseHandler);
			glutPassiveMotionFunc(Canvas::MouseMoving);
			
			glewExperimental = GL_TRUE;
			glewInit();

			SetWindow(0.f, width, 0.f, height);
			SetViewport(0.f, width, 0.f, height);
			CP.Set(0.f, 0.f);
		}
		
		void SetWindow(float l, float r, float b, float t)
		{
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluOrtho2D(l, r, b, t);
			window.Set(l, r, b, t);
		}

		void SetViewport(int l, int r, int b, int t)
		{
			viewport.Set(l, r, b, t);
			glViewport(l, b, r - l, t - b);
		}
		
		IntRect GetViewport() const noexcept
		{
			return viewport;
		}

		FloatRect GetWindow() const noexcept
		{
			return window;
		}

		float GetWindowAspectRatio() const noexcept
		{
			const auto width = std::abs(window.r - window.l);
			const auto height = std::abs(window.t - window.b);
			
			return height == 0.f ? (0.f) : (width / height);
		}

		void ClearScreen()
		{
			glClear(GL_COLOR_BUFFER_BIT);
		}

		void SetBackgroundColor(float r, float g, float b)
		{
			glClearColor(r, g, b, 0.f);
		}

		void SetColor(float r, float g, float b)
		{
			glColor3f(r, g, b);
		}

		void LineTo(float x, float y)
		{
			glBegin(GL_LINES);
				glVertex2f(CP.X, CP.Y);
				glVertex2f(x, y);
			glEnd();

			CP.Set(x, y);

			glFlush();
		}

		void LineTo(const Point2& p)
		{
			LineTo(p.X, p.Y);
		}		

		void MoveTo(float x, float y)
		{
			CP.Set(x, y);
		}

		void MoveTo(const Point2& p)
		{
			CP = p;
		}

		void MoveRel(float dx, float dy)
		{
			CP.Set(CP.X + dx, CP.Y + dy);
		}

		void LineRel(float dx, float dy)
		{
			float X = CP.X + dx;
			float Y = CP.Y + dy;
			
			LineTo(X, Y);
		}

		float GetScreenWidth() const noexcept
		{
			return width;
		}

		float GetScreenHeight() const noexcept
		{
			return height;
		}

		void TurnTo(float angle)
		{
			CD = angle;
		}

		void Turn(float angle)
		{
			CD += angle;
		}

		void Forward(float dist, bool isVisible)
		{
			const float RadPerDeg = 0.017453393;
			float X = CP.X + dist * cos(RadPerDeg * CD);
			float Y = CP.Y + dist * sin(RadPerDeg * CD);

			if(isVisible)
			{
				LineTo(X, Y);
			}
			else
			{
				MoveTo(X, Y);
			}
		}
		
		private:
		
		Canvas()
		:
		CP{}, viewport{}, window{},
		width{}, height{}, CD{}
		{
		}

		Point2 CP;
		IntRect viewport;
		FloatRect window;
		
		float width;
		float height;
		float CD;
	};
	
	void Ngon(Canvas& cvs, int n, float cx, float cy, float radius, float rotAngle)
	{
		if(n < 3) return;

		auto angle = (rotAngle / 180) * M_PI;
		const auto angleInc = (2 * M_PI) / n;
		
		cvs.MoveTo(radius * std::cos(angle) + cx, radius * std::sin(angle) + cy);
		for(int k = 0; k < n; ++k)
		{
			angle += angleInc;
			cvs.LineTo(radius * std::cos(angle) + cx, radius * std::sin(angle) + cy);
		}
	}

	void TurtleHexagon(Canvas& cvs, float R)
	{
		const auto angle = 60;
		cvs.MoveTo(cvs.GetScreenWidth() / 2, cvs.GetScreenHeight() / 2);
		
		cvs.Forward(R, false);
		cvs.Turn(2 * angle);

		for(int i = 0; i < 6; ++i)
		{
			cvs.Forward(R, true);
			cvs.Turn(angle);
		}
	}

	void Rosette(Canvas& cvs, int N, float R, const Point2& Start)
	{
		if(N < 3) return;

		std::vector<Point2> points;
		points.reserve(N);
		
		const auto angle = 2 * M_PI / N;

		for(int i = 0; i < N; ++i)
		{
			const auto angleInc = angle + angle * i;
			points.push_back(Point2(R * std::cos(angleInc) + Start.X, R * std::sin(angleInc) + Start.Y));
		}

		for(int i = 0; i < (N - 1); ++i)
		{
			for(int j = i + 1; j < N; ++j)
			{
				cvs.MoveTo(points[i]);
				cvs.LineTo(points[j]);
			}
		}
	}

	void Display()
	{
		auto& cvs = Canvas::Get();
		cvs.ClearScreen();
		cvs.SetColor(0.f, 0.f, 0.f);
		
		Rosette(cvs, 20, 100.f, Point2(cvs.GetScreenWidth() / 2, cvs.GetScreenHeight() / 2));	
	}



	void printInteraction()
	{

	}

	
	int main(int argc, char** argv)
	{
		printInteraction();
		glutInit(&argc, argv);
		
		auto& cvs = Canvas::Get();	
		cvs.Init(640, 480, "Canvas Try");
		cvs.SetBackgroundColor(1.f, 1.f, 1.f);

		glutMainLoop();
		
		return 0;
	}

}
