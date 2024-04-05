///////////////////////////////////////////////////////////////////////////////////////          
//Simple twin program
/////////////////////////////////////////////////////////////////////////////////////// 

#define _USE_MATH_DEFINES 

#include <cmath>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <sstream>

namespace TwinTry
{
	struct Vector
	{
		float X;
		float Y;

		Vector operator+(const Vector& rhs) const noexcept
		{
			return {X + rhs.X, Y + rhs.Y};
		}

		Vector operator*(float s) const noexcept
		{
			return {X * s, Y * s};
		}

		Vector operator-(const Vector& rhs) const noexcept
		{
			return (*this) + (rhs) * (-1.f);
		}

		float getLength() const
		{
			return std::sqrt(X * X + Y * Y);
		}
	};

	const int screenWidth = 640;
	const int screenHeight = 480;
	const float AnimationPeriod = 25.f;
	const float pointCloseRadius = 7.5f;

	enum class AnimState{ FORWARD, BACKWARD} animState{AnimState::BACKWARD}; //Cause if we press spacebar it will do FORWARD

	bool IsAnimating = false;
	float t = 0.f;
	float tText = 0.f;
	float tMaxTextTime = 1.f;
	
	void DrawDebugCircle(const Vector& point, int segs, float radius)
	{
		if(segs < 3) return;

		glBegin(GL_LINE_LOOP);
		const auto offset = (2 * M_PI) / segs;

		for(float r = 0.f; r < (2 * M_PI); r += offset)
		{
			const Vector drawPoint{
				point.X + radius * cos(r),
				point.Y + radius * sin(r)	
			};

			glVertex2f(drawPoint.X, drawPoint.Y);
		}

		glEnd();
	}

	template<typename T>
	T lerp(const T& a, const T& b, double t)
	{
		return a + (b - a) * t;
	}

	struct Polyline
	{

		Polyline()
		:
		points{},
		drawPrimitive{GL_LINE_STRIP},
		done{}
		{
		}
		
		void Draw() const
		{
			glBegin(drawPrimitive);
			
			for(const auto& i : points)
			{
				glVertex2f(i.X, i.Y);
			}

			glEnd();
		}
		
		void SetMustHaveVerticies(int newLength)
		{
			if(newLength < points.size()) return;

			const auto newPoints = newLength - points.size();
			AllocateNewPoints(newPoints);
		}

		void EndCollecting()
		{
			drawPrimitive = GL_LINE_LOOP;
			done = true;
		}

		bool IsDone() const noexcept
		{
			return done;
		}

		std::vector<Vector> points;
		
		private:
		
		void AllocateNewPoints(int newPoints)
		{
			if(newPoints <= 0) return;
			
			for(int i = 0; i < newPoints;)
			{
				for(auto it = std::begin(points); it != std::end(points); ++it)
				{
					const auto next = std::next(it);
					if(next != std::end(points))
					{
						const auto p = lerp(*it, *next, 0.5);
						it = points.insert(next, p);
						++i;

						if(i >= newPoints)
							break;
					}
				}

				if(i < newPoints)
				{
					const auto p = (points[0] - points[points.size() - 1]) * 0.5;
					points.push_back(p);
					++i;
				}
			}
		}

		GLenum drawPrimitive;
	
		bool done;

	};

	std::vector<Polyline> polylines;
	
	int polylineIndex = 0;
	const int maxPolylines = 2;

	void Animate(int value)
	{
		if(IsAnimating)
		{
			if(animState == AnimState::FORWARD)
			{	
				t = std::min(t + AnimationPeriod / 1000.f, 1.f);
				if(t == 1.f) IsAnimating = false;
			}
			else
			{	
				t = std::max(t - AnimationPeriod / 1000.f, 0.f);
				if(t == 0.f) IsAnimating = false;
			}	

			glutPostRedisplay();
		}

		tText = std::min(tText + AnimationPeriod / 1000.f, tMaxTextTime);
		if(tText == tMaxTextTime) glutPostRedisplay();
		glutTimerFunc(AnimationPeriod, Animate, 1);
	}
	
	void initPolylines()
	{
		polylines.resize(maxPolylines);
	}

	void setup()
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glPointSize(4.0);
		Animate(1);
		initPolylines();
	}

	
	void WriteText(void* Font, const std::string& str)
	{
		for(auto i : str)
		{
			glutBitmapCharacter(Font, i);
		}
	}
	
	enum class AppState
	{
		POLYGON_SELECTING,
		TWINING
	} appState{AppState::POLYGON_SELECTING};
	
	void selectPolyline()
	{
		if(tText < tMaxTextTime)
		{
			glColor3f(1.0, 0.0, 0.0);
			glRasterPos2f(240.f, 400.f);
			
			std::stringstream os;
			os << "Choose " << polylineIndex + 1 << " polyline!";
			WriteText(GLUT_BITMAP_TIMES_ROMAN_24, os.str());
		}

		const auto& sample = polylines[polylineIndex];

		glColor3f(0.f, 1.f, 0.f);
		sample.Draw();
		
		if((sample.points.size() > 1) && (!sample.IsDone()))
		{
			glColor3f(1.f, 0.f, 0.f);
			DrawDebugCircle(sample.points[0], 20, pointCloseRadius);
		}
	}

	void showPolylines()
	{
		for(const auto& line : polylines)
		{
			std::cout << "Polyline:\n";
			for(const auto& i : line.points)
			{
				std::cout << "(" << i.X << "," << i.Y << ")" << '\n';
			}
		}
	}

	void twining()
	{
		if( (polylines.size() < 2) && (polylines.size() > 2) ) return;

		if(tText < tMaxTextTime)
		{
			glColor3f(1.0, 0.0, 0.0);
			glRasterPos2f(270.f, 400.f);
			WriteText(GLUT_BITMAP_TIMES_ROMAN_24, "Twin time!");
		}
		const auto polylineLength = polylines[0].points.size();
		const auto& fPL = polylines[0];
	       	const auto& sPL = polylines[1];	
		
		glColor3f(0.f, 0.f, 0.f);
		glBegin(GL_LINE_LOOP);
		for(int i = 0; i < polylineLength; ++i)
		{
			const auto bP = lerp(sPL.points[i], fPL.points[i],t);	
			glVertex2f(bP.X, bP.Y);
		}
		glEnd();
	}
	
	void drawScene(void)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		
		if(appState == AppState::POLYGON_SELECTING)
		{
			selectPolyline();
		}
		else if(appState == AppState::TWINING)
		{
			twining();
		}
			
		glFlush();
	}

	void resize(int w, int h)
	{
		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0.0, screenWidth, 0.0, screenHeight);
	}

	void reset()
	{
		polylineIndex = 0;
		tText = 0.f;
		polylines.clear();
		initPolylines();
		appState = AppState::POLYGON_SELECTING;
		glutPostRedisplay();
	}

	void keyInput(unsigned char key, int x, int y)
	{
		switch (key)
		{
			case 27:
				exit(0);
			break;
			
			case ' ':
			{
				IsAnimating = !IsAnimating;
				
				if(IsAnimating) animState = (animState == AnimState::FORWARD) ? AnimState::BACKWARD : AnimState::FORWARD;
			}
			break;
			
			case 'R':
			case 'r':
				reset();
			break;
		}
	}

	void printInteraction(void)
	{
		std::cout << "Simple twin program here\n";
		std::cout << "Press ESC to quit\n";
		std::cout << "R to restart\n";
		std::cout << "Space bar to stop animation\n";
		std::cout << "Click with left mouse button when it's possible!\n";
		std::cout << "And that's all!!!\n";
	}

		
	bool isCloseToPoint(const Vector& To, const Vector& SelectedPoint, const double radius)
	{
		const auto dist = (To - SelectedPoint).getLength();
		return dist <= radius;
	}

	int getPolylineMaxLengths()
	{
		int max = 0;

		for(const auto& i : polylines)
		{
			const int size = i.points.size();
			max = std::max(max, size);
		}

		return max;
	}

	void setMaxLengths(int mustHaveSize)
	{
		for(auto& i : polylines)
		{
			i.SetMustHaveVerticies(mustHaveSize);
		}
	}

	void mouseCallBack(int button, int state, int x, int y)
	{
		if(appState == AppState::TWINING) return;

		if(button == GLUT_LEFT_BUTTON)
		{
			if(state == GLUT_DOWN)
			{
				auto& sample = polylines[polylineIndex];
				if(!sample.IsDone())
				{
					const Vector choosePoint{x, screenHeight - y};
					auto& polylinePoints = sample.points;

					if(polylinePoints.size() > 3)
					{
						if(isCloseToPoint(polylinePoints[0], choosePoint, pointCloseRadius))
						{
							sample.EndCollecting();
							
							tText = 0.f;
							if( polylineIndex != (maxPolylines - 1) ) polylineIndex = std::min(maxPolylines - 1, polylineIndex + 1);				
							else 
							{
								appState = AppState::TWINING; 
								const auto maxLength = getPolylineMaxLengths();
								setMaxLengths(maxLength);
							}

							glutPostRedisplay();
							return;
						}
					}
					
					polylinePoints.push_back(choosePoint);
					if(polylinePoints.size() > 1)
						glutPostRedisplay();
				}
			}
		}
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
		glutCreateWindow("Twin stuff");
		glutDisplayFunc(drawScene);
		glutReshapeFunc(resize);
		glutKeyboardFunc(keyInput);
		glutMouseFunc(mouseCallBack);

		glewExperimental = GL_TRUE;
		glewInit();

		setup();

		glutMainLoop();
		
		return 0;
	}

}
