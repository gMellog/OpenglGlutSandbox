///////////////////////////////////////////////////////////////////////////////////////          
//Example of turtle graphics
/////////////////////////////////////////////////////////////////////////////////////// 

#define _USE_MATH_DEFINES 

#include <cmath>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

namespace TurtleCanvas
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
		
		Canvas() = default;

		Point2 CP;
		IntRect viewport;
		FloatRect window;
		
		float width;
		float height;
		float CD;
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
	
	void Hook(Canvas& cvs, float L)
	{
		cvs.Forward(3 * L, true);
		cvs.Turn(90);
		cvs.Forward(L, true);
		cvs.Turn(90);
		cvs.Forward(L, true);
		cvs.Turn(90);
	}
	
	void HookDraw(Canvas& cvs)
	{
		cvs.MoveTo(cvs.GetScreenWidth() / 2 - cvs.GetScreenWidth() * 0.1, cvs.GetScreenHeight() / 2 + cvs.GetScreenHeight() * 0.05);
		Hook(cvs, 40.f);
		Hook(cvs, 40.f);
		Hook(cvs, 40.f);
		Hook(cvs, 40.f);
	}

	void Polyspiral(Canvas& cvs, float length, float angle, float incr, int num)
	{
		cvs.MoveTo(cvs.GetScreenWidth() / 2, cvs.GetScreenHeight() / 2);
		for(int i = 0; i < num; ++i)
		{
			cvs.Forward(length, true);
			cvs.Turn(angle);
			length += incr;
		}
	}

	//Add MoveTo to some place before call recursive version
	void PolyspiralRecursive(Canvas& cvs, float length, float angle, float incr, int num)
	{
		if(num == 0)
		{
			return;
		}
		
		cvs.Forward(length, true);
		cvs.Turn(angle);

		PolyspiralRecursive(cvs, length + incr, angle, incr, num - 1);
	}
	
	void DrawPolyspiralA(Canvas& cvs)
	{
		Polyspiral(cvs, 2.f, 60.f, 0.5f, 200);
	}

	void DrawPolyspiralARecursive(Canvas& cvs)
	{
		cvs.MoveTo(cvs.GetScreenWidth() / 2, cvs.GetScreenHeight() / 2);
		
		PolyspiralRecursive(cvs, 2.f, 60.f, 0.5f, 200);
	}

	void DrawPolyspiralB(Canvas& cvs)
	{
		Polyspiral(cvs, 2.f, 89.5f, 1.f, 200);
	}

	void DrawPolyspiralC(Canvas& cvs)
	{
		Polyspiral(cvs, 5.f, -144.f, 2.f, 200);
	}

	void DrawPolyspiralD(Canvas& cvs)
	{
		Polyspiral(cvs, 5.f, 170.f, 2.f, 200);
	}

	void SquareInSquares(Canvas& cvs, float length = 40.f)
	{
		cvs.MoveTo(cvs.GetScreenWidth() / 2 - cvs.GetScreenWidth() * 0.08f, cvs.GetScreenHeight() / 2 - cvs.GetScreenHeight() * 0.1f);
		
		for(int i = 0; i < 4; ++i)
		{
			const auto moveLength = length * (i + 1);
			cvs.Forward(length, true);
			cvs.Turn(90);
			cvs.Forward(moveLength, true);
			cvs.Turn(90);
			cvs.Forward(moveLength, true);
			cvs.Turn(90);
			cvs.Forward(length, true);
			cvs.Forward(length * i, false);
			cvs.Turn(90);
			cvs.Forward(moveLength, false);
		}
	}

	void SimplePolyspiral(Canvas& cvs)
	{
		Polyspiral(cvs, 30.f, -90.f, 10.f, 31);
	}

	void TriangleInTriangles(Canvas& cvs, float length = 80.f)
	{
		cvs.MoveTo(cvs.GetScreenWidth() / 2 - cvs.GetScreenWidth() * 0.1f, cvs.GetScreenHeight() / 2 - cvs.GetScreenHeight() * 0.1f);
		cvs.Forward(2 * length, true);
		cvs.Turn(120);
		cvs.Forward(length, true);
		cvs.Turn(60);
		cvs.Forward(length, true);
		cvs.Turn(120);
		cvs.Forward(length, true);
		cvs.Turn(120);
		cvs.Forward(length, true);
		cvs.Turn(60);
		cvs.Forward(length, true);
		cvs.Turn(120);
		cvs.Forward(2 * length, true);	
	}

	void FamousLogo(Canvas& cvs, float length = 50.f)
	{
		cvs.MoveTo(cvs.GetScreenWidth() / 2, cvs.GetScreenHeight() / 2);
		
		for(int i = 0; i < 3; ++i)
		{
			cvs.Turn(60);
			cvs.Forward(length, true);
			cvs.Turn(60);
			cvs.Forward(length, true);
			cvs.Turn(120);
			cvs.Forward(length, true);
			cvs.Turn(60);
			cvs.Forward(length, true);
			cvs.Turn(60);
			cvs.Turn(120);
		}
	}

	void TurtleDrawWithString(Canvas& cvs, float length = 60.f, const std::string& str = "FLFLFLFRFLFLFLFRFLFLFLFR")
	{
		cvs.MoveTo(cvs.GetScreenWidth() / 2, cvs.GetScreenHeight() / 2);

		for(const auto i : str)
		{
			if(i == 'F')
			{
				cvs.Forward(length, true);
			}
			else if(i == 'L')
			{
				cvs.Turn(60);
			}
			else if(i == 'R')
			{
				cvs.Turn(-60);
			}
		}
	}

	void SimpleMotifMeandr(Canvas& cvs, float length)
	{
		cvs.Forward(length, true);
		
		cvs.Turn(90);
		cvs.Forward(length * 0.5, true);
		
		cvs.Turn(90);
		cvs.Forward(length * 0.45, true);
		
		cvs.Turn(90);
		cvs.Forward(length * 0.25, true);
		
		cvs.Turn(-90);
		cvs.Forward(length * 0.4, true);
		
		cvs.Turn(-90);
		cvs.Forward(length * 0.5, true);
		
		cvs.Turn(-90);
		cvs.Forward(length * 1.05, true);
		
		cvs.Turn(-90);
		cvs.Forward( length * 0.75, true);
		
		cvs.Turn(90);
	}

	void SimpleMeandr(Canvas& cvs, int num = 10, float length = 40.f)
	{
		cvs.MoveTo(cvs.GetScreenWidth() / 2 - cvs.GetScreenWidth() * 0.38, cvs.GetScreenHeight() / 2);

		for(int i = 0; i < num; ++i)
		{
			SimpleMotifMeandr(cvs, length);
		}
	}

	void TMotifMeandr(Canvas& cvs, float length)
	{	
		cvs.Forward(length * 0.4, true);
		
		cvs.Turn(90);
		cvs.Forward(length * 0.4, true);
		
		cvs.Turn(90);
		cvs.Forward(length * 0.25, true);
		
		cvs.Turn(-90);
		cvs.Forward(length * 0.3, true);

		cvs.Turn(-90);
		cvs.Forward(length * 0.8, true);
			
		cvs.Turn(-90);
		cvs.Forward(length * 0.3, true);
	
		cvs.Turn(-90);
		cvs.Forward(length * 0.25, true);
		
		cvs.Turn(90);
		cvs.Forward(length * 0.4, true);
		
		cvs.Turn(90);
		cvs.Forward(length * 0.4, true);	
	}

	void TMeandr(Canvas& cvs, int num = 10, float length = 55.f)
	{
		cvs.MoveTo(cvs.GetScreenWidth() / 2 - cvs.GetScreenWidth() * 0.475, cvs.GetScreenHeight() / 2);

		for(int i = 0; i < num; ++i)
		{
			TMotifMeandr(cvs, length);
		}
	}

	void YMotifMeandr(Canvas& cvs, float length)
	{
		cvs.Forward(length * 0.5, true);

		cvs.Turn(90);
		cvs.Forward(length * 0.25, true);
		
		cvs.Turn(90);
		cvs.Forward(length * 0.5 * 0.75, true);

		cvs.Turn(-90);
		cvs.Forward(length * 0.3, true);

		cvs.Turn(-90);
		cvs.Forward(length * 0.5 * 0.75, true);

		cvs.Turn(-90);
		cvs.Forward(length * 0.1, true);
		
		cvs.Turn(90);
		cvs.Forward(length * 0.2, true);

		cvs.Turn(90);
		cvs.Forward(length * 0.1, true);
		
		cvs.Turn(-90);
		cvs.Forward(length * 0.5 * 0.75, true);

		cvs.Turn(-90);
		cvs.Forward(length * 0.3, true);

		cvs.Turn(-90);
		cvs.Forward(length * 0.5 * 0.75, true);
		
		cvs.Turn(90);
		cvs.Forward(length * 0.25, true);

		cvs.Turn(90);
		cvs.Forward(length * 0.5, true);

	}

	void YMeandr(Canvas& cvs, int num = 6, float length = 80.f)
	{
		cvs.MoveTo(cvs.GetScreenWidth() / 2 - cvs.GetScreenWidth() * 0.45, cvs.GetScreenHeight() / 2);

		for(int i = 0; i < num; ++i)
		{
			YMotifMeandr(cvs, length);
		}
	}

	void ToughMotifMeandr1(Canvas& cvs, float length)
	{
		cvs.Forward(length, true);
		
		cvs.Turn(90);
		cvs.Forward(length * 0.75, true);
		
		cvs.Turn(90);
		cvs.Forward(length * 0.3, true);

		cvs.Turn(90);
		cvs.Forward(length * 0.4, true);

		cvs.Turn(-90);
		cvs.Forward(length * 0.3, true);
		
		cvs.Turn(-90);
		cvs.Forward(length * 0.6, true);
		
		cvs.Turn(-90);
		cvs.Forward(length * 0.825, true);

		cvs.Turn(-90);
		cvs.Forward(length * 0.95, true);
		
		cvs.Turn(90);
	}

	void ToughMeandr1(Canvas& cvs, int num = 5, float length = 60.f)
	{
		cvs.MoveTo(cvs.GetScreenWidth() / 2 - cvs.GetScreenWidth() * 0.3, cvs.GetScreenHeight() / 2);

		for(int i = 0; i < num; ++i)
		{
			ToughMotifMeandr1(cvs, length);
		}
	}

	void ToughMotifMeandr2(Canvas& cvs, float length)
	{
		cvs.Forward(length * 0.95, true);
		
		cvs.Turn(90);
		cvs.Forward(length * 0.75, true);
		
		cvs.Turn(90);
		cvs.Forward(length * 0.5, true);

		cvs.Turn(90);
		cvs.Forward(length * 0.25, true);
		
		cvs.Turn(90);
		cvs.Forward(length * 0.2, true);
		
		cvs.Turn(-90);
		cvs.Forward(length * 0.15, true);
		
		cvs.Turn(-90);
		cvs.Forward(length * 0.4, true);
		
		cvs.Turn(-90);
		cvs.Forward(length * 0.55, true);

		cvs.Turn(-90);
		cvs.Forward(length * 0.9, true);
			
		cvs.Turn(-90);
		cvs.Forward(length * 0.9, true);

		cvs.Turn(90);
	}

	void ToughMeandr2(Canvas& cvs, int num = 5, float length = 60.f)
	{
		cvs.MoveTo(cvs.GetScreenWidth() / 2 - cvs.GetScreenWidth() * 0.275, cvs.GetScreenHeight() / 2);

		for(int i = 0; i < num; ++i)
		{
			ToughMotifMeandr2(cvs, length);
		}
	}

	void ToughMotifMeandr3(Canvas& cvs, float length)
	{
		cvs.Forward(length, true);
		
		cvs.Turn(90);
		cvs.Forward(length * 0.75, true);
		
		cvs.Turn(90);
		cvs.Forward(length * 0.6, true);
		
		cvs.Turn(90);
		cvs.Forward(length * 0.35, true);
		
		cvs.Turn(90);
		cvs.Forward(length * 0.2, true);
		
		cvs.Turn(90);
		cvs.Forward(length * 0.15, true);
		
		cvs.Turn(-90);
		cvs.Forward(length * 0.2, true);
		
		cvs.Turn(-90);
		cvs.Forward(length * 0.325, true);
		
		cvs.Turn(-90);
		cvs.Forward(length * 0.6, true);

		cvs.Turn(-90);
		cvs.Forward(length * 0.7, true);

		cvs.Turn(-90);
		cvs.Forward(length, true);
		
		cvs.Turn(-90);
		cvs.Forward(length * 0.925, true);
		
		cvs.Turn(90);

	}

	void ToughMeandr3(Canvas& cvs, int num = 8, float length = 60.f)
	{
		cvs.MoveTo(cvs.GetScreenWidth() / 2 - cvs.GetScreenWidth() * 0.45, cvs.GetScreenHeight() / 2);

		for(int i = 0; i < num; ++i)
		{
			ToughMotifMeandr3(cvs, length);
		}
	}
	
	void Display()
	{
		auto& cvs = Canvas::Get();
		cvs.ClearScreen();
		cvs.SetColor(0.f, 0.f, 0.f);
		
		DrawPolyspiralARecursive(cvs);	
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
