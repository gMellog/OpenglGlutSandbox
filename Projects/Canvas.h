//TODO
// handle scale of window,

#define _USE_MATH_DEFINES

#include <cstdlib>
#include <vector>
#include <array>
#include <map>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h> 

namespace Canvas
{
	struct Point
	{
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

		bool operator==(const Color& rhs) const noexcept
		{
			return (R == rhs.R) && (G == rhs.G) && (B == rhs.B);
		}

		bool operator!=(const Color& rhs) const noexcept
		{
			return !(*this == rhs);
		}
	};

	struct PointColor
	{
		Point p;
		Color color;
	};

	float getLowestCoord(const std::vector<Point>& v, bool x)
	{
		float r{ 10000 };

		for (const auto& i : v)
		{
			if (x)
			{
				if (i.X < r)
					r = i.X;
			}
			else
			{
				if (i.Y < r)
					r = i.Y;
			}
		}

		return r;
	}

	float getHighestCoord(const std::vector<Point>& v, bool x)
	{
		float r{ 0 };

		for (const auto& i : v)
		{
			if (x)
			{
				if (r < i.X)
					r = i.X;
			}
			else
			{
				if (r < i.Y)
					r = i.Y;
			}
		}

		return r;
	}

	template<typename T>
	std::vector<T> getFill(const std::vector<T>& elems, bool filled)
	{
		std::vector<T> r;

		for (const auto& el : elems)
		{
			if (el.fill == filled)
			{
				r.push_back(el);
			}
		}

		return r;
	}

	struct Rect {

		Rect()
			:
			fill {}
		{

		}

		Rect(const Point& pLd, const Point& pRu, const Color& pColor, bool pFill)
			:
			ld{pLd},
			ru{pRu},
			color{pColor},
			fill{pFill}
		{
			
		}

		Rect(const Point& p1, const Point& p2, const Point& p3, const Point& p4, const Color& pColor, bool pFill)
			:
			color{pColor},
			fill{ pFill }
		{
			std::vector<Point> v{
				p1, p2, p3, p4
			};

			const auto minX = getLowestCoord(v, true);
			const auto minY = getLowestCoord(v, false);
			const auto maxX = getHighestCoord(v, true);
			const auto maxY = getHighestCoord(v, false);

			ld = Point{ minX, minY };
			ru = Point{ maxX, maxY };
		}


		std::vector<Point> getFullCoords() const
		{
			return { ld, {ld.X, ru.Y, ld.Z}, ru, {ru.X, ld.Y, ld.Z} };
		}

		Point ld;
		Point ru;
		Color color;
		bool fill;
	};

	static int width = 500;
	static int height = 500;
	static const float ZPrimitive = 0.5f;
	static const float LeftColumnBackgroundZ = 0.4f;
	static const float LeftColumnActiveZ = 0.45f;
	static const Color currColor;
	static int mainMenuID;
	static int primitiveOptionsID;
	static bool colorOption = false;

	struct PrimitiveDrawer;

	PrimitiveDrawer* currDrawer = nullptr;

	struct PrimitiveDrawer
	{
		virtual ~PrimitiveDrawer() = default;

		virtual void init(const Point& pUpLeft, const Point& pDownRight)
		{
			upLeft = pUpLeft;
			downRight = pDownRight;
			clicked = false;
			firstVertexSet = secondVertexSet = false;
		}

		virtual void draw() = 0;
		virtual void click() = 0;
		virtual void addVertex(const Point& vertex) = 0;
		virtual void undo() = 0;
		virtual void mouseControl(int button, int state, int x, int y);
		virtual void mouseMotion(int x, int y);
		virtual void keyInput(unsigned char k);
		virtual void clear() = 0;
		virtual bool isFillable() const { return false; }
		virtual void setFill(bool newFill) {}
		virtual void setColor(const Color& newColor) = 0;

		bool inBounds(int x, int y) const noexcept
		{
			return (upLeft.X <= x && downRight.X >= x) && (upLeft.Y >= y && downRight.Y <= y);
		}

		bool isClicked() const noexcept
		{
			return clicked;
		}

	protected:
		
		Point upLeft;
		Point downRight;
		bool secondVertexSet;
		bool firstVertexSet;
	
		virtual bool endWorkAfterNextPoint();

	private:

		bool clicked;

		void drawSelectArea()
		{
			Point ld{
				upLeft.X,
				downRight.Y,
				LeftColumnActiveZ
			};

			Point lu{
				upLeft.X,
				upLeft.Y,
				LeftColumnActiveZ
			};

			Point ru{
				downRight.X,
				upLeft.Y,
				LeftColumnActiveZ
			};

			Point rd{
				downRight.X,
				downRight.Y,
				LeftColumnActiveZ
			};

			const std::array<Point, 4> points{ ld, lu, ru, rd };

			glColor3f(1.f, 1.f, 1.f);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glVertexPointer(3, GL_FLOAT, 0, points.data());
			glDrawArrays(GL_QUADS, 0, points.size());
		}

	};

	void PrimitiveDrawer::draw()
	{
		if (clicked)
		{
			drawSelectArea();
		}
	}

	void PrimitiveDrawer::click()
	{
		currDrawer = clicked ? nullptr : this;
		clicked = !clicked;
	}

	void PrimitiveDrawer::mouseControl(int button, int state, int x, int y)
	{
		if (secondVertexSet)
		{
			const auto proceedState = endWorkAfterNextPoint();
			undo();
			addVertex({ static_cast<float>(x), static_cast<float>(y), ZPrimitive });

			if (proceedState)
			{
				secondVertexSet = false;
				firstVertexSet = false;
			}
			else
			{
				secondVertexSet = false;
				firstVertexSet = true;
			}
		}
		else
		{
			addVertex({ static_cast<float>(x), static_cast<float>(y), ZPrimitive });
			secondVertexSet = false;
			firstVertexSet = true;
		}
	}

	void PrimitiveDrawer::mouseMotion(int x, int y)
	{
		if (firstVertexSet)
		{
			addVertex({ static_cast<float>(x), static_cast<float>(y), ZPrimitive });
			secondVertexSet = true;
			firstVertexSet = false;
		}
		else if (secondVertexSet)
		{
			undo();
			addVertex({ static_cast<float>(x), static_cast<float>(y), ZPrimitive });
		}
	}

	void PrimitiveDrawer::keyInput(unsigned char k)
	{
		switch (k)
		{
		case 27:
			exit(0);
			break;
		default:
			break;
		}
	}

	bool PrimitiveDrawer::endWorkAfterNextPoint()
	{
		return true;
	}

	struct PointDrawer : PrimitiveDrawer
	{
		PointDrawer()
			:
			color{ 0.f, 0.f, 0.f }
		{

		}

		void draw() override
		{
			PrimitiveDrawer::draw();
			drawIcon();
			drawPoints();
		}

		void setColor(const Color& newColor)
		{
			color = newColor;
			glutPostRedisplay();
		}

		void addVertex(const Point& vertex) override
		{
			points.push_back({vertex, color});
			glutPostRedisplay();
		}

		void click() override
		{
			PrimitiveDrawer::click();
		}

		void undo() override
		{
			
		}

		void clear() override
		{
			points.clear();
		}

	private:

		Color color;

		void drawIcon()
		{
			Point point{
				upLeft.X + (downRight.X - upLeft.X) / 2.f,
				downRight.Y + (upLeft.Y - downRight.Y) / 2.f,
				ZPrimitive
			};

			glColor3f(color.R, color.G, color.B);
			glPointSize(3.f);
			glVertexPointer(3, GL_FLOAT, 0, &point);

			glDrawArrays(GL_POINTS, 0, 1);
		}

		void drawPoints()
		{
			if (!points.empty())
			{
				glPointSize(3.f);

				glEnableClientState(GL_COLOR_ARRAY);

				glVertexPointer(3, GL_FLOAT, 6 * sizeof(float), (float*)points.data());
				glColorPointer(3, GL_FLOAT, 6 * sizeof(float), (float*)points.data() + 3);
				
				glDrawArrays(GL_POINTS, 0, points.size());
				
				glDisableClientState(GL_COLOR_ARRAY);
			}
		}

		std::vector<PointColor> points;
	};

	struct LineDrawer : PrimitiveDrawer
	{
		LineDrawer()
			:
			color{ 0.f, 0.f, 0.f }
		{

		}

		void draw() override
		{
			PrimitiveDrawer::draw();
			drawIcon();
			drawSelectedPoints();
			drawLines();
		}

		void addVertex(const Point& vertex) override
		{
			selectedPoints.push_back(vertex);

			if (selectedPoints.size() == 2)
			{
				points.push_back({ selectedPoints[0], color });
				points.push_back({ selectedPoints[1], color });
				
				selectedPoints.clear();
			}
			
			glutPostRedisplay();
		}

		void click() override
		{
			PrimitiveDrawer::click();
			if (!isClicked())
			{
				selectedPoints.clear();
			}
		}

		void undo() override
		{
			const auto prelast = points[points.size() - 2];
			selectedPoints.push_back(prelast.p);
			points.erase(std::end(points) - 2, std::end(points));
		}

		void clear() override
		{
			points.clear();
			selectedPoints.clear();
		}

		void setColor(const Color& newColor)
		{
			color = newColor;
			glutPostRedisplay();
		}

	private:

		std::vector<PointColor> points;
		std::vector<Point> selectedPoints;
		Color color;

		void drawIcon()
		{
			Point ld{
				upLeft.X + width * 0.1f * 0.2f,
				downRight.Y + height * 0.1f * 0.2f,
				ZPrimitive
			};

			Point ru{
				downRight.X - width * 0.1f * 0.2f,
				upLeft.Y - height * 0.1f * 0.2f,
				ZPrimitive
			};

			const std::array<Point, 2> points{ ld,ru };

			glColor3f(color.R, color.G, color.B);
			glVertexPointer(3, GL_FLOAT, 0, points.data());
			glDrawArrays(GL_LINES, 0, points.size());
		}

		void drawLines()
		{
			if (!points.empty())
			{
				glEnableClientState(GL_COLOR_ARRAY);

				glVertexPointer(3, GL_FLOAT, 6 * sizeof(float), (float*)points.data());
				glColorPointer(3, GL_FLOAT, 6 * sizeof(float), (float*)points.data() + 3);

				glDrawArrays(GL_LINES, 0, points.size());
				
				glDisableClientState(GL_COLOR_ARRAY);
			}
		}

		void drawSelectedPoints()
		{
			if (!selectedPoints.empty())
			{
				glColor3f(0.f, 0.f, 0.f);
				glVertexPointer(3, GL_FLOAT, 0, selectedPoints.data());
				glDrawArrays(GL_POINTS, 0, selectedPoints.size());
			}
		}
	};

	struct RectDrawer : PrimitiveDrawer
	{
	private:
	
		void createIcon()
		{
			Point ld{
				upLeft.X + width * 0.1f * 0.2f,
				downRight.Y + height * 0.1f * 0.3f,
				ZPrimitive
			};

			Point ru{
				downRight.X - width * 0.1f * 0.2f,
				upLeft.Y - height * 0.1f * 0.3f,
				ZPrimitive
			};

			Rect rect{ ld, ru, {0.f, 0.f, 0.f}, false };

			rects.push_back(rect);
		}

		Rect& getIcon()
		{
			return *rects.begin();
		}

	public:

		void init(const Point& pUpLeft, const Point& pDownRight)
		{
			PrimitiveDrawer::init(pUpLeft, pDownRight);
			createIcon();
		}

		void draw() override
		{
			PrimitiveDrawer::draw();
			drawIcon();
			drawRects();
			drawSelectedPoints();
		}

		void setColor(const Color& newColor)
		{
			getIcon().color = newColor;
			glutPostRedisplay();
		}

		void addVertex(const Point& vertex) override
		{
			selectedPoints.push_back(vertex);
			
			if (selectedPoints.size() == 2)
			{
				const auto p1 = selectedPoints[0];
				const auto p2 = selectedPoints[1];
				const auto p3 = Point{p1.X, p2.Y, ZPrimitive};
				const auto p4 = Point{p2.X, p1.Y, ZPrimitive };

				Rect rect{ p1,p2,p3,p4, getIcon().color, getIcon().fill };

				rects.push_back(rect);	
			}

			glutPostRedisplay();
		}

		void click() override
		{
			PrimitiveDrawer::click();

			if (!isClicked())
			{
				selectedPoints.clear();
			}
		}

		void undo() override
		{
			rects.erase(std::end(rects) - 1);
			selectedPoints.erase(std::end(selectedPoints) - 1);
		}

		void clear() override
		{
			if(rects.size() > 1)
				rects.erase(std::begin(rects) + 1, std::end(rects));
			
			selectedPoints.clear();
		}

		bool isFillable() const override { return true; }
		
		void setFill(bool newFill) override
		{
			getIcon().fill = newFill;
			glutPostRedisplay();
		}

		void mouseControl(int button, int state, int x, int y) override
		{
			PrimitiveDrawer::mouseControl(button, state, x, y);

			if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
			{
				if (selectedPoints.size() == 2)
				{
					selectedPoints.clear();
				}
			}
		}

	protected:

		std::vector<Rect> rects;
		std::vector<Point> selectedPoints;

		virtual void drawIcon() {}

		std::vector<Rect> getRectsFill(bool filled)
		{
			return getFill<Rect>(rects, filled);
		}

		void drawRects(bool filled)
		{
			auto rects = getRectsFill(filled);

			if (rects.empty()) return;

			std::vector<float> attribs;
			attribs.reserve(24 * rects.size());

			for (const auto& rect : rects)
			{
				auto fullCords = rect.getFullCoords();
				
				for (const auto& coord : fullCords)
				{
					attribs.push_back(coord.X);
					attribs.push_back(coord.Y);
					attribs.push_back(coord.Z);

					attribs.push_back(rect.color.R);
					attribs.push_back(rect.color.G);
					attribs.push_back(rect.color.B);
				}
			}
			
			glPolygonMode(GL_FRONT_AND_BACK, filled ? GL_FILL : GL_LINE);
			glVertexPointer(3, GL_FLOAT, 6 * sizeof(float), (float*)attribs.data());
			glColorPointer(3, GL_FLOAT, 6 * sizeof(float), (float*)attribs.data() + 3);

			glDrawArrays(GL_QUADS, 0, attribs.size() / 2);
		}

		virtual void drawRects()
		{
			if (!rects.empty())
			{
				glEnableClientState(GL_COLOR_ARRAY);
				
				drawRects(true);
				drawRects(false);

				glDisableClientState(GL_COLOR_ARRAY);
			}
		}

		virtual void drawSelectedPoints()
		{
			if (!selectedPoints.empty())
			{
				glColor3f(0.f, 0.f, 0.f);
				glVertexPointer(3, GL_FLOAT, 0, selectedPoints.data());
				glDrawArrays(GL_POINTS, 0, selectedPoints.size());
			}
		}
	};

	struct MultiSegment : PrimitiveDrawer
	{
		MultiSegment()
			:
			color{0.f, 0.f, 0.f}
		{

		}

		void setColor(const Color& newColor)
		{
			color = newColor;
			glutPostRedisplay();
		}

		void draw() override
		{
			PrimitiveDrawer::draw();
			drawIcon();
			drawLinesStrip();
			drawSelectedPoints();
		}

		void addVertex(const Point& vertex) override
		{
			selectedPoints.push_back(vertex);

			if (usingPoints.empty())
			{
				if (selectedPoints.size() == 2)
				{
					usingPoints.push_back({ selectedPoints[0], color });
					usingPoints.push_back({ selectedPoints[1], color });
					
					selectedPoints.erase(std::begin(selectedPoints));
				}
			}
			else
			{
				usingPoints.push_back({ vertex, color });
				selectedPoints.erase(std::begin(selectedPoints));
			}

			glutPostRedisplay();
		}

		void click() override
		{
			PrimitiveDrawer::click();

			if (!isClicked())
			{
				if (!usingPoints.empty())
				{
					stripLines.push_back(usingPoints);
					usingPoints.clear();
				}

				selectedPoints.clear();
			}
		}
		
		void endLine()
		{
			//return to initial state
			firstVertexSet = false;
			secondVertexSet = false;

			undo();

			stripLines.push_back(std::move(usingPoints));
			usingPoints.clear();
			selectedPoints.clear();

			glutPostRedisplay();
		}

		void undo() override
		{
			if (usingPoints.size() == 2)
			{
				selectedPoints.clear();
				selectedPoints.push_back(usingPoints[0].p);
				usingPoints.clear();
			}
			else
			{
				usingPoints.erase(std::end(usingPoints) - 1);
				selectedPoints.clear();
				const auto prelast = usingPoints[usingPoints.size() - 2];
				const auto last = usingPoints[usingPoints.size() - 1];
				selectedPoints.push_back(prelast.p);
				selectedPoints.push_back(last.p);
			}
		}

		bool endWorkAfterNextPoint() override
		{
			return false;
		}

		void mouseControl(int button, int state, int x, int y) override 
		{
			PrimitiveDrawer::mouseControl(button, state, x, y);

			if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
			{
				endLine();
			}
		}

		void clear() override
		{
			selectedPoints.clear();
			usingPoints.clear();
			stripLines.clear();
		}

	private:

		std::vector<Point> selectedPoints;
		std::vector<PointColor> usingPoints;
		std::vector<std::vector<PointColor>> stripLines;
		Color color;

		void drawIcon()
		{
			Point p1{
				upLeft.X + width * 0.1f * 0.15f,
				downRight.Y + height * 0.1f * 0.25f,
				ZPrimitive
			};

			Point p2{
				upLeft.X + width * 0.1f * 0.35f,
				downRight.Y + height * 0.1f * 0.65f,
				ZPrimitive
			};

			Point p3{
				upLeft.X + width * 0.1f * 0.7f,
				downRight.Y + height * 0.1f * 0.4f,
				ZPrimitive
			};

			Point p4{
				upLeft.X + width * 0.1f * 0.9f,
				downRight.Y + height * 0.1f * 0.8f,
				ZPrimitive
			};

			const std::array<Point, 4> points{ p1, p2, p3, p4 };

			glColor3f(color.R, color.G, color.B);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glVertexPointer(3, GL_FLOAT, 0, points.data());
			glDrawArrays(GL_LINE_STRIP, 0, points.size());
		}

		void drawLinesStrip()
		{
			glEnableClientState(GL_COLOR_ARRAY);

			if (!usingPoints.empty())
			{
				glVertexPointer(3, GL_FLOAT, 6 * sizeof(float), (float*)usingPoints.data());
				glColorPointer(3, GL_FLOAT, 6 * sizeof(float), (float*)usingPoints.data() + 3);
				glDrawArrays(GL_LINE_STRIP, 0, usingPoints.size());
			}
			
			if (!stripLines.empty())
			{
				std::vector<int> first;
				first.reserve(stripLines.size());
				
				int i, j;
				for (i = j = 0; i < stripLines.size(); ++i)
				{
					first.push_back(j);
					j = j + stripLines[i].size();
				}

				std::vector<int> count;
				count.reserve(stripLines.size());
				for (const auto& i : stripLines)
					count.push_back(i.size());

				std::vector<PointColor> vertices;
				
				for (const auto& stripLinePoints : stripLines)
				{
					for (const auto& point : stripLinePoints)
					{
						vertices.push_back(point);
					}
				}

				glVertexPointer(3, GL_FLOAT, 6 * sizeof(float), (float*)vertices.data());
				glColorPointer(3, GL_FLOAT, 6 * sizeof(float), (float*)vertices.data() + 3);
				
				glMultiDrawArrays(GL_LINE_STRIP, first.data(), count.data(), stripLines.size());
			}
			
			glDisableClientState(GL_COLOR_ARRAY);
		}

		void drawSelectedPoints()
		{
			if (!selectedPoints.empty())
			{
				glVertexPointer(3, GL_FLOAT, 0, selectedPoints.data());
				glDrawArrays(GL_POINTS, 0, selectedPoints.size());
			}
		}
	};

	struct CircleDrawer : PrimitiveDrawer
	{
		CircleDrawer(int pIconN = 10, int pDrawN = 40)
			:
			iconN{pIconN},
			drawN{pDrawN},
			color{0.f, 0.f, 0.f},
			fill{}
		{

		}

		void setFill(bool newFill)
		{
			fill = newFill;
			glutPostRedisplay();
		}

		void setColor(const Color& newColor)
		{
			color = newColor;
			glutPostRedisplay();
		}

		void draw() override
		{
			PrimitiveDrawer::draw();
			drawIcon();
			drawCircles();
			drawSelectedPoints();
		}

		void addVertex(const Point& vertex) override
		{
			if (selectedPoints.size() == 1)
			{
				const auto center = selectedPoints[0];
				const auto circlePoint = vertex;

				const auto ROffset = Point{
					circlePoint.X - center.X,
					circlePoint.Y - center.Y
				};

				const auto R = sqrt(ROffset.X * ROffset.X + ROffset.Y * ROffset.Y);

				const auto RX = center.X - R;

				if (RX < width * 0.1f)
				{
					selectedPoints.push_back({ width * 0.1f, center.Y,ZPrimitive});
				}
				else
				{
					selectedPoints.push_back(vertex);
				}
			}
			
			if (selectedPoints.size() == 2)
			{
					circles.push_back({ selectedPoints[0], selectedPoints[1], color, fill });
			}
			else
			{
				selectedPoints.push_back(vertex);
			}

			glutPostRedisplay();
		}

		void click() override
		{
			PrimitiveDrawer::click();
			
			if (!isClicked())
			{
				selectedPoints.clear();
			}
		}

		void undo() override
		{
			circles.erase(std::end(circles) - 1);
			selectedPoints.erase(std::end(selectedPoints) - 1);
		}

		void clear() override
		{
			//if (circles.size() > 1)
			//	circles.erase(std::begin(circles) + 1, std::end(circles));
			circles.clear();
			selectedPoints.clear();
		}

		bool isFillable() const override { return true; }

		void mouseControl(int button, int state, int x, int y) override
		{
			PrimitiveDrawer::mouseControl(button, state, x, y);

			if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
			{
				if (selectedPoints.size() == 2)
				{
					selectedPoints.clear();
				}
			}
		}

	private:

		struct Circle
		{
			Point center;
			Point circlePoint;
			Color color;
			bool fill;
		};

		std::vector<Point> selectedPoints;
		std::vector<Circle> circles;

		int iconN;
		int drawN;
		Color color;
		bool fill;

		void drawIcon()
		{
			Point center{
				upLeft.X + (downRight.X - upLeft.X) / 2,
				downRight.Y + (upLeft.Y - downRight.Y) / 2,
				ZPrimitive
			};

			const auto R = width * 0.1f * 0.25f;
			const auto N = iconN;

			std::vector<Point> points;
			
			if (fill)
			{
				points.push_back(center);
			}
			
			points.resize((N + 1) + 1 * fill);

			int i;
			float t;
			for (i = 0; i <= N; ++i)
			{
				t = 2 * M_PI * i / N;
				points[i] = {center.X + R * cos(t), center.Y + R * sin(t), ZPrimitive};
			}

			glColor3f(color.R, color.G, color.B);
			glPolygonMode(GL_FRONT_AND_BACK, fill ? GL_FILL : GL_LINE);
			glVertexPointer(3, GL_FLOAT, 0, points.data());
			
			glDrawArrays(fill ? GL_TRIANGLE_FAN : GL_LINE_STRIP, 0, points.size());
		}

		void drawCircles(bool filled)
		{
			auto filterCircles = getFill(circles, filled);

			if (filterCircles.empty()) return;

			const auto N = drawN;
			std::vector<int> first;
			first.reserve(filterCircles.size() + 1 * filled);

			int i, j;
			for (i = j = 0; i < filterCircles.size(); ++i)
			{
				first.push_back(j);
				j = j + (N + 1 + 1 * filled);
			}

			std::vector<int> count;
			count.reserve(first.size());
			for (const auto& i : first)
				count.push_back(N + 1 + 1 * filled);

			std::vector<float> attribs;
			attribs.reserve(filterCircles.size() * (N + 1 + 1 * filled) * 2);

			for (int i = 0; i < filterCircles.size(); ++i)
			{
				const auto center = filterCircles[i].center;
				const auto circlePoint = filterCircles[i].circlePoint;

				const auto ROffset = Point{
					circlePoint.X - center.X,
					circlePoint.Y - center.Y
				};

				const auto R = sqrt(ROffset.X * ROffset.X + ROffset.Y * ROffset.Y);

				const auto pColor = filterCircles[i].color;

				if (filled)
				{
					attribs.push_back(center.X);
					attribs.push_back(center.Y);
					attribs.push_back(center.Z);

					attribs.push_back(pColor.R);
					attribs.push_back(pColor.G);
					attribs.push_back(pColor.B);
				}

				float t;
				for (int j = 0; j <= N; ++j)
				{
					t = 2 * M_PI * j / N;

					const Point p{
						center.X + R * cos(t),
						center.Y + R * sin(t),
						ZPrimitive
					};

					attribs.push_back(p.X);
					attribs.push_back(p.Y);
					attribs.push_back(p.Z);

					attribs.push_back(pColor.R);
					attribs.push_back(pColor.G);
					attribs.push_back(pColor.B);
				}
			}

			glPolygonMode(GL_FRONT_AND_BACK, filled ? GL_FILL : GL_LINE);
			glVertexPointer(3, GL_FLOAT, 6 * sizeof(float), (float*)attribs.data());
			glColorPointer(3, GL_FLOAT, 6 * sizeof(float), (float*)attribs.data() + 3);

			glMultiDrawArrays(filled ? GL_TRIANGLE_FAN : GL_LINE_STRIP, first.data(), count.data(), circles.size());
		}

		void drawCircles()
		{
			if (!circles.empty())
			{
				glEnableClientState(GL_COLOR_ARRAY);
				drawCircles(true);
				drawCircles(false);
				glDisableClientState(GL_COLOR_ARRAY);
			}
		}

		void drawSelectedPoints()
		{
			if (!selectedPoints.empty())
			{
				glVertexPointer(3, GL_FLOAT, 0, selectedPoints.data());
				glDrawArrays(GL_POINTS, 0, selectedPoints.size());
			}
		}
	};

	struct HexagonDrawer : CircleDrawer
	{
		HexagonDrawer() : CircleDrawer(6,6) {}
	};

	void writeStrokeString(void* font, const std::string& str)
	{
		for (const auto ch : str) glutStrokeCharacter(font, ch);
	}

	void drawVerticalLinePeriodically(int val);

	std::vector<std::string> split(const std::string& str, char deli)
	{
		std::vector<std::string> r;

		auto from = 0u;
		auto nextSpace = str.find(deli);

		while (nextSpace != std::string::npos)
		{
			auto part = str.substr(from, nextSpace - from);
			r.push_back(std::move(part));

			from = nextSpace + 1;
			nextSpace = str.find(' ', from);
		}

		if (from <= (str.size() - 1))
			r.push_back(str.substr(from));

		return r;
	}

	/** TODO
	* add less space with ' ' symbol
	* add possibility to move around with | symbol with help of arrow keys
	*/
	struct TextField
	{
		explicit TextField(const Rect& pRect)
			:
			rectField{pRect},
			rectText{}
		{

		}

		Rect rectField;
		std::string rectText;
		Color color;
	};

	struct TextDrawer : PrimitiveDrawer
	{
		TextDrawer()
			:
			showCursor{},
			updateCursor{},
			typicalWaitTime{500},
			backspaceCursorTime{250},
			cursorWaitTime{typicalWaitTime},
			noActionsTime{},
			selectedField{},
			leftBottomDownTimes{0},
			color{0.f, 0.f, 0.f}
		{
		}

		void click() override
		{
			PrimitiveDrawer::click();

			if (!isClicked())
			{
				deselectField();
			}
		}

		Rect getSelectedRect() const noexcept
		{
			Rect r;
			
			if (selectedField)
				r = selectedField->rectField;

			return r;
		}

		void setRectText(const std::string& newText)
		{
			if(selectedField)
				selectedField->rectText = newText;
		}

		std::string getRectText() const noexcept
		{
			return selectedField ? selectedField->rectText : "";
		}

		void drawRect(const TextField& textField)
		{
			const auto rectCoords = textField.rectField.getFullCoords();
			
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glColor3f(0.f, 0.f, 0.f);
			glVertexPointer(3, GL_FLOAT, 0, rectCoords.data());
			glDrawArrays(GL_QUADS, 0, rectCoords.size());
		}

		void draw() override
		{
			PrimitiveDrawer::draw();

			drawIcon();

			if (selectedField && selectedField->color != color)
			{
				selectedField->color = color;
			}

			if (noActionsTime >= typicalWaitTime && (cursorWaitTime == backspaceCursorTime))
				cursorWaitTime = typicalWaitTime;

			for (auto& textField : textFields)
			{
				const auto& rect = textField.rectField;
				const auto ld = rect.ld;
				const auto ru = rect.ru;

				drawRect(textField);

				if (selectedField && updateCursor)
				{
					auto& rectText = selectedField->rectText;

					if (showCursor)
					{
						rectText.push_back('|');
					}
					else
					{
						if (!rectText.empty() && rectText.back() == '|')
							rectText.erase(std::end(rectText) - 1);
					}

					updateCursor = false;
				}

				glColor3f(0.f, 0.f, 0.f);
				glLineWidth(2.f);
			
				glPushMatrix();
				glTranslatef(ld.X + (ru.X - ld.X) * 0.001f, ld.Y + (ru.Y - ld.Y) * 0.25f, ZPrimitive);

				// 0.3 of screen is 1.f of scalef
				glScalef(0.2f, (ru.Y - ld.Y) / (0.1 * height) * 0.3f, 0.125f);

				for (const auto ch : textField.rectText)
				{
					if (ch == '|')
					{
						glColor3f(0.f, 0.f, 0.f);
					}
					else
					{
						glColor3f(textField.color.R, textField.color.G, textField.color.B);
					}

					glutStrokeCharacter(GLUT_STROKE_ROMAN, ch);
				}

				glPopMatrix();
				glLineWidth(1.f);
				
			}
		}

		void setColor(const Color& newColor) override
		{
			color = newColor;

			glutPostRedisplay();
		}

		void addVertex(const Point& point) override
		{
			selectedPoints.push_back(point);

			if (selectedPoints.size() == 2)
			{
				const auto p1 = selectedPoints[0];
				const auto p2 = selectedPoints[1];
				const auto p3 = Point{ p1.X, p2.Y, ZPrimitive };
				const auto p4 = Point{ p2.X, p1.Y, ZPrimitive };

				const auto rect = Rect{ p1,p2,p3,p4, {0.f, 0.f, 0.f}, false };

				textFields.back().rectField.ld = rect.ld;
				textFields.back().rectField.ru = rect.ru;

				glutPostRedisplay();
			}
		}

		void undo() override
		{
			selectedPoints.erase(std::end(selectedPoints) - 1);
		}

		void keyInput(unsigned char k) override
		{
			if (!selectedField) return;

			auto& rectText = selectedField->rectText;
			PrimitiveDrawer::keyInput(k);
			noActionsTime = 0;
			cursorWaitTime = backspaceCursorTime;

			const auto BACKSPACE = 8;

			if (!rectText.empty())
			{
				const auto lastChar = rectText.back();

				if(lastChar == '|')
					rectText.erase(std::end(rectText) - 1);
			}
			
			if ((k == BACKSPACE) && !(rectText.empty()))
			{
				rectText.erase(std::end(rectText) - 1);
			}
			else
			{
				rectText += k;
			}

			glutPostRedisplay();
		}

		void mouseControl(int button, int state, int x, int y)
		{
			const auto mousePos = Point{ x * 1.f, y * 1.f, ZPrimitive };
			if (!selectedField)
			{
				if (leftBottomDownTimes == 0)
				{
					for (auto& i : textFields)
					{
						if (isVertexInsideRect(i.rectField.ld, i.rectField.ru, mousePos))
						{
							selectedField = &i;
							glutTimerFunc(0, drawVerticalLinePeriodically, 0);
							secondVertexSet = false;
							firstVertexSet = false;
							return;
						}
					}

					textFields.push_back(TextField({}));
					textFields.back().color = color;
				}

				PrimitiveDrawer::mouseControl(button, state, x, y);
				if (leftBottomDownTimes == 1)
					selectedPoints.clear();

				leftBottomDownTimes = leftBottomDownTimes == 1 ? 0 : leftBottomDownTimes + 1;
			}
			else
			{
				if (!isVertexInsideRect(selectedField->rectField.ld, selectedField->rectField.ru, mousePos))
				{
					deselectField();
				}
			}
		}

		void mouseMotion(int x, int y)
		{
			if (!selectedField)
			{
				PrimitiveDrawer::mouseMotion(x, y);
			}
		}

		void drawIcon()
		{
			glLineWidth(1.5f);
			glPushMatrix();
			glTranslatef((downRight.X - upLeft.X) / 4, downRight.Y + (upLeft.Y - downRight.Y) * 0.2f, ZPrimitive);
			glScalef(0.3f, 0.3f, 0.3f);
			glColor3f(color.R, color.G, color.B);
			writeStrokeString(GLUT_STROKE_ROMAN, "T");
			glPopMatrix();

			glLineWidth(1.f);
		}

		void setShowCursor(bool newShowCursor) noexcept
		{
			showCursor = newShowCursor;
			updateCursor = true;
			noActionsTime += cursorWaitTime;
		}

		int getUpdateCursorWaitTime() const noexcept
		{
			return cursorWaitTime;
		}

		TextField* getSelectedField() const noexcept
		{
			return selectedField;
		}

		void clear() override
		{
			textFields.clear();
			selectedPoints.clear();
			selectedField = nullptr;
			showCursor = false;
			updateCursor = false;
			leftBottomDownTimes = 0;
		}

		bool isFillable() const override { return false; }

	private:

		bool showCursor;
		bool updateCursor;

		int typicalWaitTime;
		int backspaceCursorTime;
		int cursorWaitTime;

		int noActionsTime;

		std::vector<TextField> textFields;
		TextField* selectedField;

		int leftBottomDownTimes;
		std::vector<Point> selectedPoints;
		Color color;

		bool isVertexInsideRect(const Point& ld, const Point& ru, const Point& vertex) const noexcept
		{
			return (ld.X <= vertex.X) && (vertex.X <= ru.X) && (ld.Y <= vertex.Y) && (vertex.Y <= ru.Y);
		}
		
		void deselectField()
		{
			if (!selectedField) return;
			
			auto& rectText = selectedField->rectText;
			const auto pos = rectText.find('|');

			if (pos != std::string::npos)
			{
				std::string newRectText;
				newRectText.reserve(rectText.size() - 1);

				for (int i = 0; i < rectText.size(); ++i)
				{
					if (pos != i)
					{
						newRectText.push_back(rectText[i]);
					}
				}

				newRectText.shrink_to_fit();
				rectText = std::move(newRectText);
			}

			selectedField = nullptr;
		}
	};

	void drawVerticalLinePeriodically(int val)
	{
		if (currDrawer)
		{
			auto textDrawer = dynamic_cast<TextDrawer*>(currDrawer);
			if (textDrawer && textDrawer->getSelectedField())
			{
				auto selectedField = textDrawer->getSelectedField();
				auto rectText = selectedField->rectText;
				textDrawer->setShowCursor(val == 0);

				textDrawer->setRectText(rectText);
				glutTimerFunc(textDrawer->getUpdateCursorWaitTime(), drawVerticalLinePeriodically, val == 0 ? 1 : 0);
				glutPostRedisplay();
			}
		}
	}

	struct ElemsColumnDrawer
	{
		ElemsColumnDrawer()
			:
			gridCoefficient{0.1f},
			downHeightCoefficient{},
			primsDrawers{},
			grid{true},
			gridZ{-0.1f}
		{

		}

		void init()
		{
			initPrimDrawers();
		}

		void draw()
		{
			drawLeftSelectColumn();
			drawGrid();
			drawPrimDrawers();
		}

		void drawLeftSelectColumn()
		{
			Point p1{ 0.f, height * downHeightCoefficient, LeftColumnBackgroundZ };
			Point p2{ 0.f, height * 1.f, LeftColumnBackgroundZ };
			Point p3{ 0.1f * width, height * 1.f, LeftColumnBackgroundZ };
			Point p4{ 0.1f * width, height * downHeightCoefficient, LeftColumnBackgroundZ };

			std::array<Point, 4> arr{ p1, p2, p3, p4 };

			glColor3f(0.8f, 0.8f, 0.8f);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glVertexPointer(3, GL_FLOAT, 0, arr.data());
			glDrawArrays(GL_QUADS, 0, arr.size());

			for (auto& i : arr)
				i.Z = ZPrimitive;

			glColor3f(0.f, 0.f, 0.f);
			glVertexPointer(3, GL_FLOAT, 0, arr.data());
			glDrawArrays(GL_LINE_STRIP, 0, arr.size());
		}

		void drawGrid()
		{
			if (grid)
			{
				glEnable(GL_LINE_STIPPLE);
				glLineStipple(1, 0x5555);
				drawVerticalLines();
				drawHorizontalLines();
				glDisable(GL_LINE_STIPPLE);
			}
		}

		void drawPrimDrawers()
		{
			Point upLeft{0.f, height, ZPrimitive};
			Point downRight{width * 0.1f, height - height * 0.1f, ZPrimitive};

			for (int i = 0; i < primsDrawers.size(); ++i)
			{
				primsDrawers[i]->draw();
				drawHorizontalLine({ upLeft.X, downRight.Y, ZPrimitive }, downRight);

				const auto heightOffset = height * 0.1f;
				upLeft.Y = upLeft.Y - heightOffset;
				downRight.Y = downRight.Y - heightOffset;
			}
		}

		void tryClick(int x, int y)
		{
			const auto yScreen = height - y;

			if (currDrawer && currDrawer->inBounds(x, yScreen))
			{
				currDrawer->click();
			}
			else
			{
				for (auto& i : primsDrawers)
				{
					if (i->inBounds(x, yScreen))
					{
						if (currDrawer)
							currDrawer->click();

						i->click();

						glutPostRedisplay();
					}
				}
			}
		}

		void setGrid(bool newGrid)
		{
			grid = newGrid;
		}

		void clearAll()
		{
			for (auto& i : primsDrawers)
			{
				i->clear();
			}
		}

		void setGridCoefficient(float newGridCoefficient)
		{
			gridCoefficient = newGridCoefficient;
		}

	private:

		float gridCoefficient;
		float downHeightCoefficient;
		std::vector<std::unique_ptr<PrimitiveDrawer>> primsDrawers;
		bool grid;
		float gridZ;

		void initPrimDrawers()
		{
			primsDrawers.push_back(std::make_unique<PointDrawer>());
			primsDrawers.push_back(std::make_unique<LineDrawer>());
			primsDrawers.push_back(std::make_unique<RectDrawer>());
			primsDrawers.push_back(std::make_unique<MultiSegment>());
			primsDrawers.push_back(std::make_unique<CircleDrawer>());
			primsDrawers.push_back(std::make_unique<HexagonDrawer>());
			primsDrawers.push_back(std::make_unique<TextDrawer>());

			Point upLeft{ 0.f, height, ZPrimitive };
			Point downRight{ width * 0.1f, height - height * 0.1f, ZPrimitive };

			for (int i = 0; i < primsDrawers.size(); ++i)
			{
				primsDrawers[i]->init(upLeft, downRight);

				const auto heightOffset = height * 0.1f;
				upLeft.Y = upLeft.Y - heightOffset;
				downRight.Y = downRight.Y - heightOffset;
			}
		}

		void drawVerticalLines()
		{
			std::vector<Point> points;

			const auto N = 1.f / gridCoefficient;

			for (int i = 1; i < N; ++i)
			{
				const auto widthOff = width * 0.1f + width * i * gridCoefficient;

				points.push_back({ widthOff, static_cast<float>(height), gridZ });
				points.push_back({ widthOff, 0.f, gridZ });
			}

			points.shrink_to_fit();

			glVertexPointer(3, GL_FLOAT, 0, points.data());
			glDrawArrays(GL_LINES, 0, points.size());
		}

		void drawHorizontalLines()
		{
			std::vector<Point> points;

			const auto N = 1.f / gridCoefficient;

			for (int i = 0; i < N; ++i)
			{
				const auto heightOff = height * gridCoefficient + height * i * gridCoefficient;

				points.push_back({ width * 0.1f, heightOff, gridZ });
				points.push_back({ static_cast<float>(width), heightOff, gridZ });
			}

			points.shrink_to_fit();

			glVertexPointer(3, GL_FLOAT, 0, points.data());
			glDrawArrays(GL_LINES, 0, points.size());
		}

		void drawHorizontalLine(const Point& start, const Point& end)
		{
			std::vector<Point> points{ start, end };
			
			glColor3f(0.f, 0.f, 0.f);
			glVertexPointer(3, GL_FLOAT, 0, points.data());
			glDrawArrays(GL_LINES, 0, points.size());
		}

	} elemsColumnDrawer;

	void drawScene()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		elemsColumnDrawer.draw();
		glDisable(GL_DEPTH_TEST);

		glFlush();
	}

	bool isInDrawArea(int x, int y)
	{
		const auto startX = width * 0.1f;
		const auto endX = width;
		const auto startY = 0;
		const auto endY = height;

		return (startX < x) && (x < endX) && (startY < y) && (y < endY);
	}

	// OpenGL window reshape routine.
	void resize(int w, int h)
	{
		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		// Set viewing box dimensions equal to window dimensions.
		glOrtho(0.0, w, 0.0, h, -1.0, 1.0);

		// Pass the size of the OpenGL window to globals.
		width = w;
		height = h;

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	// Keyboard input processing routine.
	void keyInput(unsigned char key, int x, int y)
	{
		if (currDrawer)
			currDrawer->keyInput(key);
	}

	void colorMenu(int id)
	{
		enum EColor{
			BLACK,
			RED,
			BLUE,
			GOLDEN,
			GREEN,
			MAGENTA
		};

		std::map<EColor, Color> colorMap {
			{BLACK, {0.f, 0.f, 0.f}},
			{RED, {1.f, 0.f, 0.f}},
			{BLUE, {0.f, 0.f, 1.f}},
			{GOLDEN, {1.f, 215.f / 255.f, 0.f}},
			{GREEN, {0.f, 1.f, 0.f}},
			{MAGENTA, {1.f, 0.f, 1.f}}
		};

		auto it = colorMap.find((EColor)id);
		
		if (it != std::end(colorMap) && currDrawer)
		{
			currDrawer->setColor(it->second);
		}
	}

	void gridMenuSize(int id)
	{
		if (id == 0)
		{
			elemsColumnDrawer.setGridCoefficient(0.025f);
		}
		else if (id == 1)
		{
			elemsColumnDrawer.setGridCoefficient(0.05f);
		}
		else if (id == 2)
		{
			elemsColumnDrawer.setGridCoefficient(0.1f);
		}

		glutPostRedisplay();
	}

	void mainMenu(int id)
	{
		if (id == 1)
		{
			elemsColumnDrawer.clearAll();
			glutPostRedisplay();
		}
		else if (id == 2) exit(0);
	}

	void gridMenu(int id)
	{
		if (id == 3) elemsColumnDrawer.setGrid(true);
		else if (id == 4) elemsColumnDrawer.setGrid(false);

		glutPostRedisplay();
	}

	int createColorMenu()
	{
		int colorMenuID = glutCreateMenu(colorMenu);
		glutAddMenuEntry("Black", 0);
		glutAddMenuEntry("Red", 1);
		glutAddMenuEntry("Blue", 2);
		glutAddMenuEntry("Golden", 3);
		glutAddMenuEntry("Green", 4);
		glutAddMenuEntry("Magenta", 5);

		return colorMenuID;
	}

	void makeMenu()
	{
		int gridSizeMenuID = glutCreateMenu(gridMenuSize);
		glutAddMenuEntry("Small", 0);
		glutAddMenuEntry("Medium", 1);
		glutAddMenuEntry("Large", 2);

		int gridMenuID = glutCreateMenu(gridMenu);
		glutAddSubMenu("Size", gridSizeMenuID);
		glutAddMenuEntry("On", 3);
		glutAddMenuEntry("Off", 4);

		mainMenuID = glutCreateMenu(mainMenu);
		glutAddSubMenu("Grid", gridMenuID);
		glutAddMenuEntry("Clear", 1);
		glutAddMenuEntry("Quit", 2);

		glutAttachMenu(GLUT_RIGHT_BUTTON);	
	}

	void mouseControl(int button, int state, int x, int y)
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			elemsColumnDrawer.tryClick(x, y);

			const auto yScreen = height - y;

			if (currDrawer && isInDrawArea(x, yScreen))
			{
				currDrawer->mouseControl(button, state, x, yScreen);
			}
		}
		else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
		{
			if (currDrawer)
			{
				const auto yScreen = height - y;
				currDrawer->mouseControl(button, state, x, yScreen);
			}
		}
	}

	void fillOutlineSubMenu(int id)
	{
		if (!currDrawer) return;

		if (id == 0)
		{
			currDrawer->setFill(true);
		}
		else if (id == 1)
		{
			currDrawer->setFill(false);
		}
	}

	void primitiveOptions(int id)
	{

	}

	void mouseMotion(int x, int y)
	{
		const auto yScreen = height - y;

		if (currDrawer)
		{
			currDrawer->mouseMotion(x, yScreen);
			
			if (currDrawer->inBounds(x, yScreen))
			{
				if (!colorOption)
				{
					glutDestroyMenu(mainMenuID);

					int fillOutlineSubMenuID = -1;

					if (currDrawer->isFillable())
					{
						fillOutlineSubMenuID = glutCreateMenu(fillOutlineSubMenu);
						glutAddMenuEntry("Fill", 0);
						glutAddMenuEntry("Outline", 1);
					}

					const int colorMenuID = createColorMenu();
					primitiveOptionsID = glutCreateMenu(primitiveOptions);
					glutAddSubMenu("Color", colorMenuID);

					if (fillOutlineSubMenuID != -1)
						glutAddSubMenu("Draw type", fillOutlineSubMenuID);

					glutAttachMenu(GLUT_RIGHT_BUTTON);
					colorOption = true;
				}
			}
			else if(colorOption)
			{
				glutDestroyMenu(primitiveOptionsID);
				makeMenu();
				colorOption = false;
			}
		}
		else
		{
			if (colorOption)
			{
				glutDestroyMenu(primitiveOptionsID);
				makeMenu();
				colorOption = false;
			}
		}
	}

	// Initialization routine.
	void setup(void)
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glEnableClientState(GL_VERTEX_ARRAY);

		elemsColumnDrawer.init();
		makeMenu();
	}

	// Routine to output interaction instructions to the C++ window.
	void printInteraction(void)
	{
		std::cout << "Interaction:" << '\n';
		std::cout << "Left click on a box on the left to select a primitive." << '\n'
			<< "Then left click on the drawing area: once for point, twice for line, rectangle, circle, hexagon and text" << '\n'
			<< "Right click for menu options." << '\n';
	}

	// Main routine.
	int main(int argc, char** argv)
	{
		printInteraction();
		glutInit(&argc, argv);

		glutInitContextVersion(4, 3);
		glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
		glutInitWindowSize(width, height);
		glutInitWindowPosition(100, 100);
		glutCreateWindow("canvas");
		glutDisplayFunc(drawScene);
		glutReshapeFunc(resize);
		glutKeyboardFunc(keyInput);
		glutMouseFunc(mouseControl);
		glutPassiveMotionFunc(mouseMotion);

		glewExperimental = GL_TRUE;
		glewInit();

		setup();

		glutMainLoop();

		return 0;
	}
}