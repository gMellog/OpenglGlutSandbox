#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <iostream>
#include <vector>
#include <cmath>
#include <numeric>

namespace Car3D
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


	struct Car
	{	
		Car()
		:
		wireFrame{},
		where{}
		{

		}

		void init()
		{
			glEnableClientState(GL_VERTEX_ARRAY);
		}

		void draw() const
		{
			glPolygonMode(GL_FRONT_AND_BACK, wireFrame ? GL_LINE : GL_FILL);

			if(wireFrame)
			{
				glColor3f(0.f, 0.f, 0.f);
			}

			drawWheel(Vector{-20.f, -6.f, 0.f}, 0.5f);
			drawWheel(Vector{20.f, -6.f, 0.f}, 0.5f);
			drawSideParts(Vector{-27.f, -7.f, 0.f});
		}

		void toggleWireframeMode()
		{
			wireFrame = !wireFrame;
			glutPostRedisplay();
		}

		private:

		void drawVertices(const std::vector<Vector>& vertices) const 
		{
			glColor3f(0.f, 0.f, 0.f);
			glPointSize(5.5f);
			glBegin(GL_POINTS);
				for(const auto& v : vertices)
					glVertex2f(v.X, v.Y);
			glEnd();
		}

		std::vector<Vector> drawCircleWithTriangleStrip(const Vector& center, const std::vector<Vector>& circlePoints) const
		{
			std::vector<Vector> r;
			
			for(const auto& i : circlePoints)
			{
				r.push_back(i);
				r.push_back(center);
			}

			return r;
		}

		std::vector<Vector> getBodyParts(const Vector& where) const
		{	
			std::vector<Vector> r{
				Vector{}, // 0
				Vector{-2.8, 0.f}, // 1
				Vector{0.f, 2.8f}, // 2
				Vector{-2.8f, 8.f}, // 3
				Vector{2.2f, 8.f}, // 4
				Vector{0.f, 13.f}, // 5
				Vector{2.2f,14.f}, // 6
				Vector{12.f,8.f}, // 7
				Vector{14.f,14.f}, // 8
				Vector{14.f, 2.8f}, // 9
				Vector{14.f, 0.f}, // 10
				Vector{40.f, 0.f}, // 11
				Vector{40.f, 2.8f}, //12
				Vector{42.2f, 8.f}, //13
				Vector{40.f, 14.f}, //14
				Vector{51.9f, 8.f}, //15
				Vector{53.9f, 2.8f}, //16
				Vector{53.9f, 0.f}, //17
				Vector{56.7f, 0.f}, //18
				Vector{56.7f, 2.8f}, //19
				Vector{56.7f, 8.f}, //20
				Vector{51.9f, 29.f}, //21
				Vector{40.f, 29.f}, //22
				Vector{40.f, 23.f}, //23
				Vector{22.f, 23.f}, //24
				Vector{19.f, 29.f}, //25
				Vector{14.f, 17.f}, //26
				Vector{5.2f, 20.f}, //27
				Vector{0.f, 18.5f}, //28
				Vector{5.2f, 18.5f}, //29
				Vector{0.f, 17.f}, //30
				Vector{5.2f, 15.5f}, //31
				Vector{16.f, 14.f}, //32
				Vector{18.f, 14.f}, //33
				Vector{16.f, 18.5f}, //34
				Vector{18.f, 20.f}, //35

			};

			for(auto& i : r)
				i += where;

			return r;
		}

		//TODO There should be two points in class which will set where

		std::vector<unsigned int> getBodyPartIndices() const
		{
			std::vector<unsigned int> r{
				0,1,2,3,4,5,6,4,7,6,8,7,9,8,8,10,14,11,12,14,13,15,15,
				16,19,17,18,18,19,20,15,21,14,22,23,22,24,25,26,27,26,
				31,8,6,8,8,26,26,27,27,28,29,30
			};

			return r;
		}

		std::vector<unsigned int> getLightBulbsPartIndices() const
		{
			std::vector<unsigned int> r{
				5,31,30,29
			};

			return r;
		}

		std::vector<unsigned int> getWindshieldsPartIndices() const
		{
			std::vector<unsigned int> r{
				8,26,32,34,34,35,35,33,24,23,24,24,33,33,23,14
			};

			return r;
		}

		std::vector<unsigned int> getSteeringWheelPartIndices() const
		{
			std::vector<unsigned int> r{
				32,33,34,35
			};

			return r;
		}

		std::vector<unsigned int*> getPartsIndicesPtr(std::vector<std::vector<unsigned int>>& partsIndices) const
		{
			std::vector<unsigned int*> r;

			r.resize(partsIndices.size());

			for(int i{}; i < partsIndices.size(); ++i)
				r[i] = partsIndices[i].data();

			return r;
		}

		void drawBodyParts() const
		{
			std::vector<std::vector<Vector>> bodyParts{
				getBodyParts(where)	
			};

			std::vector<std::vector<unsigned int>> partsIndices{
				getBodyPartIndices()
			};

			const auto vertices = getFilledVertices(bodyParts);
			glVertexPointer(3, GL_FLOAT, 0, vertices.data());
			
			const auto countIndices = getCountIndices(partsIndices);
			const auto indices = getPartsIndicesPtr(partsIndices);
			
			if(!wireFrame)
				glColor3f(1.f, 0.f, 0.f);
			
			glMultiDrawElements(GL_TRIANGLE_STRIP, countIndices.data(), GL_UNSIGNED_INT, (const void**)(indices.data()), indices.size());
		}

		void drawLightBulbs() const
		{
			std::vector<std::vector<Vector>> bodyParts{
				getBodyParts(where)	
			};

			std::vector<std::vector<unsigned int>> partsIndices{
				getLightBulbsPartIndices()
			};

			const auto vertices = getFilledVertices(bodyParts);
			glVertexPointer(3, GL_FLOAT, 0, vertices.data());
			
			const auto countIndices = getCountIndices(partsIndices);
			const auto indices = getPartsIndicesPtr(partsIndices);
			
			if(!wireFrame)
				glColor3f(1.f, 1.f, 0.f);
			
			glMultiDrawElements(GL_TRIANGLE_STRIP, countIndices.data(), GL_UNSIGNED_INT, (const void**)(indices.data()), indices.size());
		}

		void drawWindshields() const
		{
			std::vector<std::vector<Vector>> bodyParts{
				getBodyParts(where)	
			};

			std::vector<std::vector<unsigned int>> partsIndices{
				getWindshieldsPartIndices()
			};

			const auto vertices = getFilledVertices(bodyParts);
			glVertexPointer(3, GL_FLOAT, 0, vertices.data());
			
			const auto countIndices = getCountIndices(partsIndices);
			const auto indices = getPartsIndicesPtr(partsIndices);
			
			if(!wireFrame)
				glColor3f(0.9f, 0.9f, 0.9f);
			
			glMultiDrawElements(GL_TRIANGLE_STRIP, countIndices.data(), GL_UNSIGNED_INT, (const void**)(indices.data()), indices.size());
		}

		void drawSteeringWheel() const
		{
			std::vector<std::vector<Vector>> bodyParts{
				getBodyParts(where)	
			};

			std::vector<std::vector<unsigned int>> partsIndices{
				getSteeringWheelPartIndices()
			};

			const auto vertices = getFilledVertices(bodyParts);
			glVertexPointer(3, GL_FLOAT, 0, vertices.data());
			
			const auto countIndices = getCountIndices(partsIndices);
			const auto indices = getPartsIndicesPtr(partsIndices);
			
			if(!wireFrame)
				glColor3f(0.0f, 0.0f, 0.0f);
			
			glMultiDrawElements(GL_TRIANGLE_STRIP, countIndices.data(), GL_UNSIGNED_INT, (const void**)(indices.data()), indices.size());
		}

		void drawSideParts(const Vector& pWhere) const
		{	
			where = pWhere;
			drawBodyParts();
			drawLightBulbs();
			drawWindshields();
			drawSteeringWheel();
		}

		//TODO
		void drawMiddleSideParts() const
		{

		}

		//TODO
		void drawOffsetUpLightbulbs() const 
		{

		}


		void drawWheel(const Vector& where, float scale) const
		{	
			const auto innerN = 20;
			const auto innerR = 7.f * scale;

			const auto outerN = 40;
			const auto outerR = 10.f * scale;

			const auto middleWholeCircleN = 10;
			const auto middleWholeCircleR = 2.1f * scale;

			std::vector<std::vector<Vector>> circles{
				connectCircles(getCirclePoints(where, innerR - 0.1f, innerN), getCirclePoints(where, innerR + 0.5f, innerN)),
				connectCircles(getCirclePoints(where, outerR - 0.1f, outerN), getCirclePoints(where, outerR + 0.5f, outerN)),
				drawCircleWithTriangleStrip(where, getCirclePoints(where, middleWholeCircleR, middleWholeCircleN))
			};

			const auto vertices = getFilledVertices(circles);
			const auto first = getFilledFirst(circles);
			const auto count = getFilledCount(circles);
			
			if(!wireFrame)
				glColor3f(0.f, 0.f, 0.f);
			
			glVertexPointer(3, GL_FLOAT, 0.f, vertices.data());
			glMultiDrawArrays(GL_TRIANGLE_STRIP, first.data(), count.data(), circles.size());
		}
		
		std::vector<int> getCountIndices(const std::vector<std::vector<unsigned int>>& partsIndices) const
		{
			std::vector<int> r;
			r.reserve(partsIndices.size());

			for(const auto& i : partsIndices)
				r.push_back(i.size());

			return r;
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

		std::vector<Vector> getCirclePoints(const Vector& center, float R, int N) const
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


		bool wireFrame;
		mutable Vector where;

	} car;

	void drawScene(void)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLoadIdentity();
		glTranslatef(0.f, 0.f, -40.f);

		car.draw();

		glutSwapBuffers();
	}

	void setup(void)
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);
		car.init();
	}

	void resize(int w, int h)
	{
		glViewport(0, 0, w, h);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-5.f, 5.f, -5.f, 5.f, 5.f, 100.f);

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
			car.toggleWireframeMode();
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

		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
		
		glutInitWindowSize(500, 500);
		glutInitWindowPosition(100, 100);
		
		glutCreateWindow("Car3D");
		
		glutDisplayFunc(drawScene);
		glutReshapeFunc(resize);
		glutKeyboardFunc(keyInput);

		glewExperimental = GL_TRUE;
		glewInit();

		setup();

		glutMainLoop();
	}
}