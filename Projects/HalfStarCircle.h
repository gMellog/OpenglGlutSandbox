#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <iostream>
#include <vector>
#include <cmath>
#include <numeric>

namespace HalfStarCircle
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

	bool operator==(const Vector& rhs) const noexcept
	{
		return (X == rhs.X) && (Y == rhs.Y) && (Z == rhs.Z);
	}

	bool operator!=(const Vector& rhs) const noexcept
	{
		return !(*this == rhs);
	}

		float X;
		float Y;
		float Z;
	};


	inline float dotProduct(const Vector& lhs, const Vector& rhs)
	{
		return lhs.X * rhs.X + lhs.Y * rhs.Y + lhs.Z * rhs.Z;
	}

	inline float getAngleBetweenVectors(const Vector& lhs, const Vector& rhs)
	{
	const auto scalarProduct = dotProduct(lhs, rhs);
	const auto lengthsMult = lhs.length() * rhs.length();

	return acosf(scalarProduct / lengthsMult) / M_PI * 180;

	}


	struct HalfStarCircle
	{	
		HalfStarCircle()
		:
		wireFrame{},
		innerR{15.f},
		outerR{30.f},
		center{}
		{

		}

		void init()
		{
			glEnableClientState(GL_VERTEX_ARRAY);
		}

		void draw() const
		{
			glPolygonMode(GL_FRONT_AND_BACK, wireFrame ? GL_LINE : GL_FILL);
			drawOutlineCircles();
			drawStarParts();
			drawOutlineParts();
			
		}

		void toggleWireframeMode()
		{
			wireFrame = !wireFrame;
			glutPostRedisplay();
		}

		private:

		std::vector<std::vector<Vector>> getSectorsVertices(bool isStarsSectors) const
		{
			std::vector<std::vector<Vector>> r; 

			std::vector<Vector> innerCirclePoints;
			std::vector<Vector> outerCirclePoints;

			if(isStarsSectors)
			{
				innerCirclePoints = getStarInnerCirclePoints();
				outerCirclePoints = getStarOuterCirclePoints();
			}
			else
			{
				outerCirclePoints = getStarInnerCirclePoints();
				innerCirclePoints = getStarOuterCirclePoints();
				innerCirclePoints.insert(innerCirclePoints.begin(), innerCirclePoints.back());
				innerCirclePoints.pop_back();
			}

			r.reserve(outerCirclePoints.size());
			for(int i{}; i < outerCirclePoints.size(); ++i)
			{
				std::vector<Vector> sector;
				const auto N = 7;
				sector.reserve(N + 1);
				sector.push_back(outerCirclePoints[i]);
				const auto arcPoints = getArcPointsFromTo(
					center,
					getAngleOfVectorInRad(innerCirclePoints[i]), 
					getAngleOfVectorInRad(innerCirclePoints[i == 5 ? 0 : i + 1]) + ((i == 5) ? 2.f * M_PI : 0.f),
					isStarsSectors ? getStarR() : getOuterR(),
					N
				);

				for(const auto& i : arcPoints)
					sector.push_back(i);

				r.push_back(sector);
			}

			return r;
		}

		void drawSectors(bool isStarsSectors) const
		{
			const auto sectorsVertices = getSectorsVertices(isStarsSectors);

			const auto vertices = getFilledVertices(sectorsVertices);
			glVertexPointer(3, GL_FLOAT, 0, vertices.data());
			const auto first = getFilledFirst(sectorsVertices);
			const auto count = getFilledCount(sectorsVertices);

			if(isStarsSectors)
				glColor3f(1.f, 1.f, 0.f);
			else
				glColor3f(1.f, 0.f, 1.f);

			glMultiDrawArrays(GL_TRIANGLE_FAN, first.data(), count.data(), count.size());
		}

		void drawStarParts() const
		{	
			drawSectors(true);
		}
		
		void drawOutlineParts() const
		{
			drawSectors(false);
		}

		float getAngleOfVectorInRad(const Vector& p) const 
		{
			auto r = getAngleBetweenVectors(p, {1.f, 0.f}) * M_PI / 180.f;

			if((p.X < 0.f) && (p.Y < 0.f))
			{
				r += 2.f * (M_PI - r);
			}
			else if((p.X > 0.f) && (p.Y < 0.f))
			{
				r = 2 * M_PI - r; 
			}

			return r; 
		}

		void drawVertices(const std::vector<Vector>& vertices) const 
		{
			glColor3f(0.f, 0.f, 0.f);
			glPointSize(5.5f);
			glBegin(GL_POINTS);
				for(const auto& v : vertices)
					glVertex2f(v.X, v.Y);
			glEnd();
		}

		std::vector<float> getFilledVertices(const std::vector<std::vector<Vector>>& fillVertices) const
		{
			std::vector<float> r;

			std::vector<int> sizes;
			sizes.reserve(fillVertices.size());

			for(const auto i : fillVertices)
				sizes.push_back(i.size());
			
			const auto verticesN = std::accumulate(sizes.begin(), sizes.end(), 0);
			r.reserve(verticesN * 3);
			
			for(const auto& vertices : fillVertices)
			{
				for(const auto& v : vertices)
				{
					r.push_back(v.X);
					r.push_back(v.Y);
					r.push_back(v.Z);
				}
			}

			return r;
		}

		std::vector<GLint> getFilledFirst(const std::vector<std::vector<Vector>>& fillVertices) const 
		{
			std::vector<GLint> r;
			r.reserve(fillVertices.size());
			
			GLint nextIndex{};
			for(int i = 0; i < fillVertices.size(); ++i)
			{
				r.push_back(nextIndex);
				nextIndex += fillVertices[i].size();
			}

			return r;
		}

		std::vector<GLsizei> getFilledCount(const std::vector<std::vector<Vector>>& fillVertices) const 
		{
			std::vector<GLsizei> r;
			
			for(const auto& vertices : fillVertices)
				r.push_back(static_cast<GLsizei>(vertices.size()));
			
			return r;
		}

		std::vector<Vector> connectCircles(const std::vector<Vector>& lhs, const std::vector<Vector>& rhs) const
		{
			std::vector<Vector> r;

			r.reserve(lhs.size() + rhs.size());

			const auto N = std::max(lhs.size(), rhs.size());

			for(int i{}; i < N; ++i)
			{
				if(i < (lhs.size() - 1))
					r.push_back(lhs[i]);
				
				if(i < (rhs.size() - 1))
					r.push_back(rhs[i]);
			}

			if(!lhs.empty())
				r.push_back(lhs[0]);
			
			if(!rhs.empty())
				r.push_back(rhs[0]);

			return r;
		}

		void drawOutlineCircles() const
		{
			const auto innerN = 40;
			const auto outerN = 40;

			std::vector<std::vector<Vector>> circles{
				connectCircles(getCirclePoints(innerR - 0.4f, innerN), getCirclePoints(innerR, innerN)),
				connectCircles(getCirclePoints(outerR - 0.1f, outerN), getCirclePoints(outerR + 0.5f, outerN))
			};

			for(auto& c : circles)
				for(auto& v : c)
					v.Z = 0.5f;

			const auto vertices = getFilledVertices(circles);
			const auto first = getFilledFirst(circles);
			const auto count = getFilledCount(circles);
			
			glColor3f(0.f, 0.f, 0.f);
			glVertexPointer(3, GL_FLOAT, 0.f, vertices.data());
			glMultiDrawArrays(GL_TRIANGLE_STRIP, first.data(), count.data(), circles.size());
		}

		std::vector<Vector> getCirclePoints(float R, int N) const
		{
			std::vector<Vector> r;

			float t;
			for(int i{}; i <= N; ++i)
			{
				t = (2.f * M_PI / N * i);
				const auto vertex = Vector(center.X + R * cos(t), center.Y + R * sin(t));
				r.push_back(vertex);
			}

			return r;
		}

		float getOuterR() const noexcept
		{
			return outerR - 0.05f;
		}
		
		float getStarR() const noexcept
		{
			return innerR + 0.05f;
		}

		std::vector<Vector> getStarInnerCirclePoints() const
		{
			std::vector<Vector> r;

			const auto offsetAngleRad = 30.f * M_PI / 180.f;
			const auto R = getStarR();
			const auto pointsN = 6;

			for(int i{}; i < pointsN; ++i)
			{
				const float t = (offsetAngleRad + (2.f * offsetAngleRad * i));
				
				const auto point = Vector{
					center.X + R * cos(t), 
					center.Y + R * sin(t)
				};
				
				r.push_back(point);
			}

			return r;
		}

		std::vector<Vector> getStarOuterCirclePoints() const
		{
			std::vector<Vector> r;

			const auto offsetAngleRad = 60.f * M_PI / 180.f;
			const auto R = getOuterR();
			const auto pointsN = 6;

			for(int i{}; i < pointsN; ++i)
			{
				const float t = (offsetAngleRad + (offsetAngleRad * i));
				
				const auto point = Vector{
					center.X + R * cos(t), 
					center.Y + R * sin(t)
				};
				
				r.push_back(point);
			}

			return r;
		}

		std::vector<Vector> getArcPointsFromTo(const Vector& centerPoint, float startAngle, float endAngle, float R, int N) const
		{
			std::vector<Vector> r;
			r.reserve(N);

			float offset = startAngle;
			float t;
			float path = endAngle - startAngle;

			for(int i{}; i <= N; ++i)
			{
				t = offset + (path / N) * i;

				Vector point{
					centerPoint.X + R * cos(t),
					centerPoint.Y + R * sin(t)
				};

				r.push_back(point);
			}

			return r;
		}

		bool wireFrame;
	
		float innerR;
		float outerR;

		Vector center;

	} halfStarCircle;

	void drawScene(void)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		halfStarCircle.draw();

		glFlush();
	}

	void setup(void)
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);
		halfStarCircle.init();
		glEnable(GL_DEPTH_TEST);
	}

	void resize(int w, int h)
	{
		glViewport(0, 0, w, h);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-50.0, 50.0, -50, 50.0, -1.0, 1.0);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	void keyInput(unsigned char key, int x, int y)
	{
		switch (key)
		{
		case 27:
			exit(0);
			break;
		case ' ':
			halfStarCircle.toggleWireframeMode();
			break;
		default:
			break;
		}
	}

	int main(int argc, char **argv)
	{
		glutInit(&argc, argv);

		glutInitContextVersion(4, 3);
		glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
		
		glutInitWindowSize(500, 500);
		glutInitWindowPosition(100, 100);
		
		glutCreateWindow("HalfStarCircle");
		
		glutDisplayFunc(drawScene);
		glutReshapeFunc(resize);
		glutKeyboardFunc(keyInput);

		glewExperimental = GL_TRUE;
		glewInit();

		setup();

		glutMainLoop();
	}
}