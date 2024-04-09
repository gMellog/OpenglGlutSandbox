///////////////////////////////////////////////////////////////////////////////////////          
//Example of Canvas here
/////////////////////////////////////////////////////////////////////////////////////// 

#define _USE_MATH_DEFINES 

#include <cmath>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <memory>

namespace FibonacciGoldenSin
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

		void Display()
		{
			if(seq.size() == 0) return;

			auto& cvs = Canvas::Get();
			cvs.ClearScreen();
			cvs.SetColor(0.f, 0.f, 0.f);
			cvs.MoveTo(0.f, 0.f);
			cvs.SetWindow(0.f, seq.size(), GetMinFromSeq(), GetMaxFromSeq());

			const auto screenWidth = cvs.GetScreenWidth();
			const auto screenHeight = cvs.GetScreenHeight();
			
			const auto viewportWidth = screenWidth * 0.75;
			const auto viewportHeight = screenHeight * 0.75;
			
			IntRect rect;
			rect.l = (screenWidth / 2) - (viewportWidth / 2);
			rect.r = (screenWidth / 2) + (viewportWidth / 2);
			rect.b = (screenHeight/ 2) - (viewportHeight / 2);
			rect.t = (screenHeight/ 2) + (viewportHeight / 2);

			cvs.SetViewport(rect.l, rect.r, rect.b, rect.t);			

			for(int i = 0; i < seq.size(); ++i)
			{
				const auto x = i + 1;
				const auto y = seq[i];
				cvs.LineTo(x, y);
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

	void printInteraction()
	{

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
