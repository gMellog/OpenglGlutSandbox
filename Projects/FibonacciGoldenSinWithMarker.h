///////////////////////////////////////////////////////////////////////////////////////          
//Example of some draw graphs
/////////////////////////////////////////////////////////////////////////////////////// 

#define _USE_MATH_DEFINES 

#include <cmath>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

namespace FibonacciGoldenSinWithMarker
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

		Point2 operator*(const float s) const noexcept
		{
			return Point2{X * s, Y * s};
		}

		Point2 operator+(const Point2& point) const noexcept
		{
			return Point2{X + point.X, Y + point.Y};
		}
	
		Point2 operator-(const Point2& point) const noexcept
		{
			return (*this) + (point * -1.f);
		}

		float Length() const noexcept
		{
			return std::sqrt(X * X + Y * Y);
		}

		float X;
		float Y;
	};

	Point2 Norm(const Point2& point)
	{
		const auto length = point.Length();
		return Point2{point.X / length, point.Y / length};
	}

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

	template<typename T>
	std::ostream& operator<<(std::ostream& os, const Rect<T>& rect)
	{
		os << "l: " << rect.l << " r: " << rect.r << " b: " << rect.b << " t: " << rect.t << '\n';
		return os;
	}

	std::vector<float> GetFibonacci(const int N)
	{
		std::vector<float> r;
		
		if(N >= 1)
		{
			r.reserve(N);
			if(N == 1)
			{
				r = {1};
			}
			else if(N == 2)
			{
				r = {1, 1};
			}
			else if(N > 2)
			{
				r = {1, 1};
				const auto leftMembers = N - 2;
				
				for(int i = 0; i < leftMembers; ++i)
				{
					auto last = r[r.size() - 1];
					auto prevLast = r[r.size() - 2];
					
					r.push_back(last + prevLast);
				}
			}
		}

		return r;
	}

	std::vector<float> GetGoldenRatioSeq(const std::vector<float>& fiboSeq)
	{
		std::vector<float> r;
		r.reserve(fiboSeq.size() / 2);

		for(int i = 0; i < fiboSeq.size(); i += 2)
		{
			const int j = i + 1;
			const auto y_i = fiboSeq[i];
			const auto y_j = fiboSeq[j];

			r.push_back(y_j / y_i);
		}

		return r;
	}

	std::vector<float> GetSinSeq(const int N)
	{
		std::vector<float> r;
	
		if(N > 0)
		{
			r.reserve(N);
			r = {1};
			const auto a = 2 * cos(2 * M_PI / N);
			const auto leftMembers = N - r.size();

			for(int i = 0; i < leftMembers; ++i)
			{
				int y_k_2;
				int y_k_1;

				if(r.size() == 1)
				{
					y_k_2 = 0;
					y_k_1 = 1;
				}
				else
				{
					y_k_2 = r[r.size() - 2];
					y_k_1 = r[r.size() - 1];
				}
				
				r.push_back( a * y_k_1 - y_k_2 );
			}	
		}

		return r;
	}
	
	void Display();
	void MouseHandler(int button, int state, int x, int y); 
	void MouseMoving(int x, int y);

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

		static void Animate(int val)
		{
			Display();
			glutSwapBuffers();

			glutTimerFunc(animateDelay, Animate, 0);
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
			glutMouseFunc(MouseHandler);
			glutMotionFunc(MouseMoving);
			glutTimerFunc(animateDelay, Animate, 0);	

			glewExperimental = GL_TRUE;
			glewInit();

			SetWindow(0.f, width, 0.f, height);
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
			glViewport(l, b, r - l, t - b);
			viewport.Set(l, r, b, t);
		}
		
		IntRect GetViewport() const noexcept
		{
			return viewport;
		}

		FloatRect GetWindow() const noexcept
		{
			return window;
		}
		
		void SetWindowAspectRatio(float NewWindowR)
		{
			window.l *= NewWindowR;
			window.r *= NewWindowR;
			window.b *= NewWindowR;
			window.t *= NewWindowR;
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

		void CountMinMaxCoords(float x, float y)
		{
			minX = std::min(minX, x);
			minY = std::min(minY, y);
			
			maxX = std::max(maxX, x);
			maxY = std::max(maxY, y);
		}

		void LineTo(float x, float y)
		{
			glBegin(GL_LINES);
				glVertex2f(CP.X, CP.Y);
				glVertex2f(x, y);
			glEnd();

			CP.Set(x, y);
			
			CountMinMaxCoords(x, y);
		}

		void LineTo(const Point2& p)
		{
			LineTo(p.X, p.Y);
		}		

		void MoveTo(float x, float y)
		{
			CP.Set(x, y);
			CountMinMaxCoords(x, y);
		}

		void MoveTo(const Point2& p)
		{
			MoveTo(p.X, p.Y);
		}

		void MoveRel(float dx, float dy)
		{
			MoveTo(CP.X + dx, CP.Y + dy);
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

		Point2 GetCP() const noexcept
		{
			return CP;
		}
		
		void FitWorldWindowToBounds()
		{
			const auto R = width / height;
		
			window.r = maxX;
			window.l = minX;
			window.t = maxY;
			window.b = minY;
			
			const auto W = window.r - window.l;
			const auto H = window.t - window.b;

			if( R > (W / H) )
			{
				window.l = minX;
				window.r = H * R;
				window.b = minY;
				window.t = maxY;
			}
			else
			{
				window.l = minX;
				window.r = maxX;
				window.b = minY;
				window.t = W / R;
			}
		}

		void SetShouldFitAgain(bool NewFitAgain)
		{
			fitAgain = NewFitAgain;
		}

		bool IsShouldFitAgain() const noexcept
		{
			return fitAgain;
		}

		void SetClickedPoint(const Point2& newClickedPoint)
		{
			clickedPoint = newClickedPoint;
		}

		Point2 GetClickedPoint() const noexcept
		{
			return clickedPoint;
		}

		void SetLeftButtonClicked(bool newClickedState)
		{
			leftButtonClicked = newClickedState;
		}

		bool IsLeftButtonClicked() const noexcept
		{
			return leftButtonClicked;
		}

		private:
		
		Canvas()
		:
		CP{}, viewport{}, window{},
		width{}, height{}, 
		minY{1000.f}, minX{1000.f},
		maxX{}, maxY{},
		clickedPoint{},
		fitAgain{ true },
		leftButtonClicked{ false }
		{
		}

		Point2 CP;
		IntRect viewport;
		FloatRect window;
		
		float width;
		float height;

		float minY;
		float maxY;
		float minX;
		float maxX;

		Point2 clickedPoint;

		bool fitAgain;
		bool leftButtonClicked;
		
		static unsigned int animateDelay;
	};

	unsigned int Canvas::animateDelay{20};
	
	enum class GRAPH_ALGO{ FIBO_RABBITS, GOLDEN_RATIO, SIN_SEQ };
	struct GraphImpl
	{
		static GraphImpl& Get()
		{
			static GraphImpl graphImpl;
			return graphImpl;
		}
	
		void SetGraphAlgo(const GRAPH_ALGO NewGraphAlgo, const int N)
		{	
			if(NewGraphAlgo == GRAPH_ALGO::GOLDEN_RATIO) seq = GetGoldenRatioSeq(GetFibonacci(2 * N));
			else if(NewGraphAlgo == GRAPH_ALGO::FIBO_RABBITS) seq = GetFibonacci(N);
			else if(NewGraphAlgo == GRAPH_ALGO::SIN_SEQ) seq = GetSinSeq(N);
		}	

		void Star(float lowerXDist, float upXDist, float lowerYDist, float upYDist)
		{
			auto& cvs = Canvas::Get();

			const auto upOffset = 
				(lowerYDist * lowerYDist - upYDist * upYDist - lowerXDist * lowerXDist * 0.25) 
				/ (2 * (upYDist + lowerYDist));
		
			cvs.MoveRel(0, (upYDist + upOffset));
			cvs.LineRel( -lowerXDist / 2, -(lowerYDist + upYDist) );
			cvs.LineRel( lowerXDist + (upXDist - lowerXDist) / 2, lowerYDist );
			cvs.LineRel( -upXDist, 0 );
			cvs.LineRel( lowerXDist + (upXDist - lowerXDist) / 2, -lowerYDist );
			cvs.LineRel( -lowerXDist / 2, (lowerYDist + upYDist) );
			cvs.MoveRel(0, -(upYDist + upOffset));
		}

		void Display()
		{
			if(seq.size() == 0) return;

			auto& cvs = Canvas::Get();
			cvs.ClearScreen();
			cvs.SetColor(0.f, 0.f, 0.f);
			cvs.MoveTo(0.f, 0.f);
			
			const auto window = cvs.GetWindow();
			
			cvs.SetWindow(window.l, window.r, window.b, window.t);
			
			const auto screenWidth = cvs.GetScreenWidth();
			const auto screenHeight = cvs.GetScreenHeight();
			const auto R = cvs.GetWindowAspectRatio();			
			int viewportWidth;
			int viewportHeight;
			
			if( R > (screenWidth / screenHeight) )
			{
				viewportWidth  = screenWidth;
				viewportHeight = screenWidth / R;
			}
			else
			{
				viewportWidth  = screenHeight * R; 
				viewportHeight = screenHeight;
			}
			
			IntRect rect;

			rect.l = (screenWidth / 2) - (viewportWidth / 2);
			rect.r = (screenWidth / 2) + (viewportWidth / 2) ;
			rect.b = 0;
			rect.t = viewportHeight;		
			
			cvs.SetViewport(rect.l, rect.r, rect.b, rect.t);							
			
			Star(0.2f, 0.3f, 0.2f, 0.1f);

			for(int i = 0; i < seq.size(); ++i)
			{
				const auto x = i + 1;
				const auto y = seq[i];
				cvs.LineTo(x, y);
				Star(0.2f, 0.3f, 0.2f, 0.1f);
			}
			
			if(cvs.IsShouldFitAgain())
			{
				cvs.SetShouldFitAgain(false);
				cvs.FitWorldWindowToBounds();
			}
		}

		private:
		
		float GetMinFromSeq()
		{
			float r{};
			
			for(const auto i : seq)
			{
				r = std::min(i, r);
			}

			return r;
		}

		float GetMaxFromSeq()
		{
			float r{};
			
			for(const auto i : seq)
			{
				r = std::max(i, r);
			}

			return r;
		}

		GraphImpl() = default;
		std::vector<float> seq;
	};

	void Display()
        {
		GraphImpl::Get().Display();
        }

	enum MouseEvents{SCROLL_UP = 3, SCROLL_DOWN = 4};

	std::pair<float, float> GetNewWidthNewHeight(const FloatRect& window)
	{
		const auto W = window.r - window.l;
		const auto H = window.t - window.b;

		return std::make_pair(W * 0.005f, H * 0.005f);
	}

	void MouseHandler(int button, int state, int x, int y)
	{
		auto& cvs = Canvas::Get();
		const auto windowAspectRatio = cvs.GetWindowAspectRatio();
		
		if(button == GLUT_LEFT_BUTTON)
		{
			if(state == GLUT_DOWN)
			{
				cvs.SetLeftButtonClicked(true);
				cvs.SetClickedPoint({x, cvs.GetScreenHeight() - y});				
			}
			else if(state == GLUT_UP)
			{
				cvs.SetLeftButtonClicked(false);
			}
		}
		
		const auto window = cvs.GetWindow();

		if(button == MouseEvents::SCROLL_UP)
		{
			const auto NewWidthNewHeight = GetNewWidthNewHeight(window);					
			const auto newWidth = NewWidthNewHeight.first;
			const auto newHeight = NewWidthNewHeight.second;

			cvs.SetWindow(window.l + newWidth, window.r - newWidth,	window.b + newHeight, window.t - newHeight);
		}
		else if(button == MouseEvents::SCROLL_DOWN)
		{	
			const auto NewWidthNewHeight = GetNewWidthNewHeight(window);					
			const auto newWidth = NewWidthNewHeight.first;
			const auto newHeight = NewWidthNewHeight.second;

			cvs.SetWindow(window.l - newWidth, window.r + newWidth,	window.b - newHeight, window.t + newHeight);
		}

	}
	
	void MouseMoving(int x, int y)
	{
		auto& cvs = Canvas::Get();
		
		if(cvs.IsLeftButtonClicked())
		{
			const auto clickedPoint = cvs.GetClickedPoint();
			const Point2 pointNow{x, cvs.GetScreenHeight() - y};

			const auto offsetVector = clickedPoint - pointNow;
			
			auto window = cvs.GetWindow();
			
			const auto W = window.r - window.l;
			const auto H = window.t - window.b;

			const Point2 oneSizeVector{ offsetVector.X * W * 0.001f, offsetVector.Y * H * 0.001f };	
			
			window.l += oneSizeVector.X;
			window.b += oneSizeVector.Y;
			window.r += oneSizeVector.X;
			window.t += oneSizeVector.Y;

			cvs.SetWindow(window.l, window.r, window.b, window.t);
			
			cvs.SetClickedPoint(pointNow);	
		}

	}

	void printInteraction()
	{
		std::cout << "In this program you can view graph on the screen\n";
		std::cout << "and move around with left mouse button!\n";
		std::cout << "You can also use zooming with your mouse wheel\n";
		std::cout << "And of course quit with (ESC) button\n";
	}

	int main(int argc, char** argv)
	{
		if(argc != 3)
		{
			std::cerr << "Example of usage: ./(programExe) (FIBO/GOLDEN/SIN_SEQ) (AMOUNT_OF_VERTICIES)\n";
			return -1;
		}

		GRAPH_ALGO algoType;
		const std::string algoName = argv[1];
		
		if(algoName == "FIBO")
		{
			algoType = GRAPH_ALGO::FIBO_RABBITS;
		}
		else if(algoName == "GOLDEN")
		{
			algoType = GRAPH_ALGO::GOLDEN_RATIO;
		}
		else if(algoName == "SIN_SEQ")
		{
			algoType = GRAPH_ALGO::SIN_SEQ;
		}
		else
		{
			std::cerr << "This algo isn't in the list! Add this and then try again!\n";
			return -1;
		}
		
		int N;
		
		try
		{
			N = std::stoi(argv[2]);
		}
		catch(const std::invalid_argument& ex)
		{
			std::cerr << "Invalid N, try again!\n";
			return -1;
		}

		if(N <= 0)
		{
			return -1;
		}

		printInteraction();
		glutInit(&argc, argv);
		
		auto& cvs = Canvas::Get();	
		cvs.Init(640, 480, "FiboGoldenTask");
		cvs.SetBackgroundColor(1.f, 1.f, 1.f);
			
		GraphImpl::Get().SetGraphAlgo(algoType, N);

		glutMainLoop();
		
		return 0;
	}

}
