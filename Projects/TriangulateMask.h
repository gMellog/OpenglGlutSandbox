#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <iostream>
#include <vector>
#include <cmath>
#include <numeric>

namespace TriangulateMask
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


	struct Mask
	{	
		Mask()
		:
		wireFrame{},
		faceAroundCircleLines{40.f},
		center{},
		faceR{30.f},
		foreheadStartAngle{ 2.f * (2.f * M_PI / faceAroundCircleLines) },
		earR{7.f},
		left{getLeftEyeLeftUp(), 10.f, getStartEarLeftPoint().Y - getEndEarLeftPoint().Y},
		right{getRightEyeLeftUp(), 10.f, getStartEarRightPoint().Y - getEndEarRightPoint().Y}
		{

		}

		void init()
		{
			glEnableClientState(GL_VERTEX_ARRAY);
		}

		void drawVertices(const std::vector<Vector>& vertices)
		{
			glColor3f(0.f, 0.f, 0.f);
			glPointSize(5.5f);
			glBegin(GL_POINTS);
				for(const auto& v : vertices)
					glVertex2f(v.X, v.Y);
			glEnd();
		}

		std::vector<float> getFilledTriangleFanVertices(const std::vector<std::vector<Vector>>& fillVertices)
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

		std::vector<GLint> getFilledTriangleFanFirst(const std::vector<std::vector<Vector>>& fillVertices)
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

		std::vector<GLsizei> getFilledTriangleFanCount(const std::vector<std::vector<Vector>>& fillVertices)
		{
			std::vector<GLsizei> r;
			
			for(const auto& vertices : fillVertices)
				r.push_back(static_cast<GLsizei>(vertices.size()));
			
			return r;
		}

		void draw()
		{
			glPolygonMode(GL_FRONT_AND_BACK, wireFrame ? GL_LINE : GL_FILL);
			
			std::vector<std::vector<Vector>> fillVertices{
				fillForeheadVertices(),
				fillLeftEarVertices(),
				fillRightEarVertices(),
				fillMiddleVertices()
			};

			const auto vertices = getFilledTriangleFanVertices(fillVertices);
			
			const auto first = getFilledTriangleFanFirst(fillVertices);
			const auto count = getFilledTriangleFanCount(fillVertices);
			
			glColor3f(0.f, 0.f, 0.f);
			glVertexPointer(3, GL_FLOAT, 0.f, vertices.data());
			glMultiDrawArrays(GL_TRIANGLE_FAN, first.data(), count.data(), fillVertices.size());

			drawChinVertices();
		}

		void toggleWireframeMode()
		{
			wireFrame = !wireFrame;
			glutPostRedisplay();
		}

	private:

		std::vector<Vector> getArcPointsFromTo(const Vector& centerPoint, float startAngle, float endAngle, float R, int N) const
		{
			std::vector<Vector> r;
			r.reserve(N);

			float offset = startAngle;
			float t;
			float path = std::abs(endAngle - startAngle);

			for(int i{}; i <= N; ++i)
			{
				t = offset - (path / N) * i;

				Vector point{
					centerPoint.X + R * cos(t),
					centerPoint.Y + R * sin(t)
				};

				r.push_back(point);
			}

			return r;
		}

		std::vector<Vector> getLeftCheekArcPoints() const 
		{
			return getArcPointsFromTo(center, getLeftChinAngle(), getLeftAngleEnd(), faceR, 7);
		}

		std::vector<Vector> getRightCheekArcPoints() const 
		{
			return getArcPointsFromTo(center, getRightAngleEnd(), getRightChinAngle(), faceR, 7);
		}

		void fillWithArcPoints(std::vector<Vector>& vertices, int N, float R)
		{
			const auto points = getArcPointsFromTo(center, getRightMouthAngle(), getLeftMouthAngle(), R, N);

			for(const auto& v : points)
				vertices.push_back(v);
		}

		std::vector<Vector> fillChinVertices()
		{	
			std::vector<Vector> r;

			const auto chinN = 20;
			const auto chinVertices = getArcPointsFromTo(center, getRightChinAngle() + 2 * M_PI, getLeftChinAngle(), faceR, chinN);
			const auto centerPoint = center + Vector(0.f, -8.f);
			const auto diff = getRightMouthPoint() - centerPoint;
			const auto R = diff.length();
			const auto offsetAngleRad = getAngleBetweenVectors(diff,{1.f, 0.f}) * M_PI / 180.f;
			
			const auto startAngle = 2.f * M_PI - offsetAngleRad;
			const auto endAngle = M_PI + offsetAngleRad;
			
			const auto mouthVertices = getArcPointsFromTo(centerPoint, startAngle, endAngle, R, chinN);
			
			for(int i{}; i < chinVertices.size(); ++i)
			{	
				r.push_back(chinVertices[i]);
				r.push_back(mouthVertices[i]);
			}

			return r;
		}

		void drawChinVertices()
		{
			const auto chinVertices = fillChinVertices();
			
			glColor3f(0.f, 0.f, 0.f);
			glVertexPointer(3, GL_FLOAT, 0, chinVertices.data());
			glDrawArrays(GL_TRIANGLE_STRIP, 0, chinVertices.size());
		}

		std::vector<Vector>  fillMiddleVertices()
		{	
			std::vector<Vector> r;
			const auto offsetCenter = Vector{ center.X, center.Y - 5.f };
			r.push_back(offsetCenter);
			r.push_back(getLeftMouthPoint());
			
			const auto leftCheekArcPoints = getLeftCheekArcPoints();
			for(const auto& v : leftCheekArcPoints)
				r.push_back(v);

			r.push_back(left.getRightDown());
			r.push_back(left.getRightUp());
			r.push_back(right.getLeftUp());
			r.push_back(right.getLeftDown());

			const auto rightCheekArcPoints = getRightCheekArcPoints();
			for(const auto& v : rightCheekArcPoints)
				r.push_back(v);

			r.push_back(getRightMouthPoint());	

			const auto mouthUpR = (center - getRightMouthPoint()).length();
			fillWithArcPoints(r, 20, mouthUpR);

			return r;
		}

		std::vector<Vector> fillForeheadVertices()
		{
			std::vector<Vector> r;

			const auto N = 20;
			float t;

			Vector drawCenter{center.X, center.Y + faceR * sin(foreheadStartAngle)};
			r.push_back(drawCenter);
			for(int i{}; i <= N; ++i)
			{
				t = foreheadStartAngle + (M_PI - 2 * foreheadStartAngle) / N * i;
				const auto vertex = Vector(center.X + faceR * cos(t), center.Y + faceR * sin(t));
				r.push_back(vertex);
			}

			return r;
		}

		std::vector<Vector> fillLeftEarVertices()
		{
			std::vector<Vector> r;

			const auto leftAngleStart = getLeftAngleStart();
			
			const auto circleLineAngle = getCircleLineAngle();
			const auto radiusMoveAngle = leftAngleStart + circleLineAngle;
			const auto leftAngleEnd = leftAngleStart + 2.f * circleLineAngle;

			const auto offsetCenter = sqrtf(earR * earR - faceR * faceR * sin(circleLineAngle) * sin(circleLineAngle)) - 1.f;
			const auto centerOfEarLength = faceR + offsetCenter;
			
			const auto earCenter = Vector{
				center.X + centerOfEarLength * cos(radiusMoveAngle),
				center.Y + centerOfEarLength * sin(radiusMoveAngle)
			};

			const auto startEarCirclePoint = getStartEarLeftPoint();
			const auto endEarCirclePoint = getEndEarLeftPoint();

			const auto onCircleDrawPoint = Vector{
				center.X + faceR * cos(radiusMoveAngle),
				center.Y + faceR * sin(radiusMoveAngle)
			};

			const auto rightEarCenter = Vector{
				earCenter.X + earR, earCenter.Y
			};

			r.push_back(onCircleDrawPoint);
			
			const float offset = getAngleBetweenVectors(startEarCirclePoint - earCenter, rightEarCenter - earCenter) * M_PI / 180.f;
			const float until = ( 360.f - getAngleBetweenVectors(endEarCirclePoint - earCenter, rightEarCenter - earCenter)) * M_PI / 180.f;

			float t;
			const auto N = 10;
			for(int i{}; i <= N; ++i)
			{
				t = offset + (until - offset) / N * i;
				const auto vertex = Vector(earCenter.X + earR * cos(t), earCenter.Y + earR * sin(t));
				r.push_back(vertex);
			}

			r.push_back(left.getLeftDown());
			r.push_back(left.getLeftUp());		
			r.push_back(startEarCirclePoint);

			return r;
		}

		std::vector<Vector> fillRightEarVertices()
		{
			std::vector<Vector> r;

			const auto rightAngleStart = getRightAngleStart();
			const auto circleLineAngle = getCircleLineAngle();

			const auto radiusMoveAngle = rightAngleStart - circleLineAngle;
			const auto rightAngleEnd = rightAngleStart - 2.f * circleLineAngle;

			const auto offsetCenter = sqrtf(earR * earR - faceR * faceR * sin(circleLineAngle) * sin(circleLineAngle)) - 1.f;
			const auto centerOfEarLength = faceR + offsetCenter;
			
			const auto earCenter = Vector{
				center.X + centerOfEarLength * cos(radiusMoveAngle),
				center.Y + centerOfEarLength * sin(radiusMoveAngle)
			};

			const auto startEarCirclePoint =  getStartEarRightPoint();
			const auto endEarCirclePoint = getEndEarRightPoint();

			const auto onCircleDrawPoint = Vector{
				center.X + faceR * cos(radiusMoveAngle),
				center.Y + faceR * sin(radiusMoveAngle)
			};

			const auto rightEarCenter = Vector{
				earCenter.X - earR, earCenter.Y
			};

			r.push_back(onCircleDrawPoint);
			
			const float offset = (180.f - getAngleBetweenVectors(startEarCirclePoint - earCenter, rightEarCenter - earCenter)) * M_PI / 180.f;
			const float until = (getAngleBetweenVectors(endEarCirclePoint - earCenter, rightEarCenter - earCenter) + 180.f) * M_PI / 180.f;

			float t;
			const auto N = 10;
			for(int i{}; i <= N; ++i)
			{
				t = offset - (2 * M_PI - (until - offset)) / N * i;
				const auto vertex = Vector(earCenter.X + earR * cos(t), earCenter.Y + earR * sin(t));
				r.push_back(vertex);
			}

			r.push_back(right.getRightDown());
			r.push_back(right.getRightUp());		
			r.push_back(startEarCirclePoint);
		
			return r;
		}
		
		float getLeftAngleStart() const noexcept
		{
			return M_PI -  foreheadStartAngle;
		}

		float getLeftAngleEnd() const noexcept
		{
			return getLeftAngleStart() + 2.f * getCircleLineAngle();
		}

		float getCircleLineAngle() const noexcept
		{
			return 2.f * M_PI / faceAroundCircleLines;
		}

		Vector getStartEarLeftPoint() const noexcept
		{
			return Vector{
				center.X + faceR * cos(getLeftAngleStart()),
				center.Y + faceR * sin(getLeftAngleStart())
			};
		}

		Vector getEndEarLeftPoint() const noexcept
		{
			return Vector{
				center.X + faceR * cos(getLeftAngleEnd()),
				center.Y + faceR * sin(getLeftAngleEnd())
			};
		}

		float getRightAngleStart() const noexcept
		{
			return foreheadStartAngle;
		}

		float getRightAngleEnd() const noexcept
		{
			return getRightAngleStart() - 2.f * getCircleLineAngle();
		}

		Vector getStartEarRightPoint() const noexcept
		{
			return Vector{
				center.X + faceR * cos(getRightAngleStart()),
				center.Y + faceR * sin(getRightAngleStart())
			};
		}

		Vector getEndEarRightPoint() const noexcept
		{
			return Vector{
				center.X + faceR * cos(getRightAngleEnd()),
				center.Y + faceR * sin(getRightAngleEnd())
			};
		}

		float getLeftChinAngle() const noexcept
		{
			return getLeftAngleEnd() + (M_PI_2 / 3.0);
		}

		float getRightChinAngle() const noexcept
		{
			return getRightAngleEnd() - (M_PI_2 / 3.0);
		}

		Vector getLeftChinPoint() const noexcept
		{
			Vector r;

			const auto angleAt = getLeftAngleEnd() + (M_PI_2 / 3.0);

			r.X = center.X + faceR * cos(angleAt);
			r.Y = center.Y + faceR * sin(angleAt);

			return r;
		}

		Vector getRightChinPoint() const noexcept
		{
			Vector r;

			const auto angleAt = getRightAngleEnd() - (M_PI_2 / 3.0);

			r.X = center.X + faceR * cos(angleAt);
			r.Y = center.Y + faceR * sin(angleAt);

			return r;
		}

		Vector getLeftEyeLeftUp() const noexcept
		{
			return Vector{-17.5f, getStartEarLeftPoint().Y};
		}

		Vector getRightEyeLeftUp() const noexcept
		{
			return Vector{7.5f, getStartEarRightPoint().Y};
		}

		float getLeftMouthAngle() const noexcept
		{
			return 2.0 * M_PI - getAngleBetweenVectors(getLeftMouthPoint(),{1.f, 0.f}) * M_PI / 180.f;
		}

		float getRightMouthAngle() const noexcept
		{
			return 2.0 * M_PI - getAngleBetweenVectors(getRightMouthPoint(),{1.f, 0.f}) * M_PI / 180.f;
		}

		Vector getLeftMouthPoint() const noexcept
		{
			return Vector(-12.f, -12.f);
		}

		Vector getRightMouthPoint() const noexcept
		{
			return Vector(12.f, -12.f);
		}

		bool wireFrame;
		float faceAroundCircleLines;
		Vector center;
		float faceR;
		float foreheadStartAngle;
		float earR;
		
		struct RectangleEye
		{
			RectangleEye(const Vector& pLeftUp, float pWidth, float pHeight)
			:
			leftUp{pLeftUp},
			width{pWidth},
			height{pHeight}
			{

			}

			Vector getLeftUp() const noexcept
			{
				return leftUp;
			}
			
			Vector getRightUp() const noexcept
			{
				return leftUp + Vector(width);
			}

			Vector getLeftDown() const noexcept
			{
				return leftUp - Vector(0.f, height);			
			}

			Vector getRightDown() const noexcept
			{
				return getRightUp() - Vector(0.f, height);
			}
		
		private:

			Vector leftUp;
			float width;
			float height;

		} left, right;

	} mask;

	void drawScene(void)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		mask.draw();

		glFlush();
	}

	void setup(void)
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);
		mask.init();
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
			mask.toggleWireframeMode();
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

		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
		
		glutInitWindowSize(500, 500);
		glutInitWindowPosition(100, 100);
		
		glutCreateWindow("TriangulateMask");
		
		glutDisplayFunc(drawScene);
		glutReshapeFunc(resize);
		glutKeyboardFunc(keyInput);

		glewExperimental = GL_TRUE;
		glewInit();

		setup();

		glutMainLoop();
	}
}