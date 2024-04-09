///////////////////////////////////////////////////////////////////////////////////////          
//Example of Canvas here
/////////////////////////////////////////////////////////////////////////////////////// 

#define _USE_MATH_DEFINES 

#include <cmath>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

namespace Canvas
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
		
		private:
		
		Canvas() = default;

		Point2 CP;
		IntRect viewport;
		FloatRect window;
		
		float width;
		float height;

	};
	
	void Arrow(float f, float h, float t, float w)
	{
		auto& cvs = Canvas::Get();
		cvs.LineRel(-w - t/2, -f);

		cvs.LineRel(w, 0);
		cvs.LineRel(0, -h);
		cvs.LineRel(t, 0);

		cvs.LineRel(0, h);

		cvs.LineRel(w, 0);
		cvs.LineRel(-w - t / 2, f);
	}

	void Star(float lowerXDist, float upXDist, float lowerYDist, float upYDist)
	{
		auto& cvs = Canvas::Get();

		cvs.LineRel( -lowerXDist / 2, -(lowerYDist + upYDist) );
		cvs.LineRel( lowerXDist + (upXDist - lowerXDist) / 2, lowerYDist);
		cvs.LineRel( -upXDist, 0);
		cvs.LineRel( lowerXDist + (upXDist - lowerXDist) / 2, -lowerYDist);
		cvs.LineRel( -lowerXDist / 2, (lowerYDist + upYDist) );
	}

	void Display()
	{
		auto& cvs = Canvas::Get();
		cvs.ClearScreen();
		cvs.SetColor(0.f, 0.f, 0.f);
		cvs.MoveTo(cvs.GetScreenWidth() / 2, cvs.GetScreenHeight() / 2);
		//Arrow(40.f, 20.f, 15.f, 15.f);
		Star(20.f, 30.f, 20.f, 10.f);
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
