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

	std::vector<float> getFilledVertices(const std::vector<std::vector<Vector>>& fillVertices)
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

	std::vector<GLint> getFilledFirst(const std::vector<std::vector<Vector>>& fillVertices) 
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

	std::vector<GLsizei> getFilledCount(const std::vector<std::vector<Vector>>& fillVertices) 
	{
		std::vector<GLsizei> r;
		
		for(const auto& vertices : fillVertices)
			r.push_back(static_cast<GLsizei>(vertices.size()));
		
		return r;
	}

	void glutBitmapStr(void* font, const std::string& str)
	{
		for(const auto ch: str) glutBitmapCharacter(font, ch);
	}

	struct Car
	{	
		Car()
		:
		wireFrame{},
		where{27.f, -7.f, 0.f},
		sidePartsZOffset{-20.f}
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

			const auto leftFrontWheelPoint = Vector{
				-20.f, -6.f, 0.f
			};

			const auto rightFrontWheelPoint = Vector{
				20.f, -6.f, 0.f
			};

			draw3DWheel(leftFrontWheelPoint);
			draw3DWheel(rightFrontWheelPoint);

			drawSideParts(Vector{-27.f, -7.f, -0.f});
			drawMiddleSideParts();
			drawFrontWindshield();
			drawBackWindshield();
			drawBackLightBulbs();
			drawLabel();

			draw3DWheel(leftFrontWheelPoint + Vector{0.f, 0.f, sidePartsZOffset});
			draw3DWheel(rightFrontWheelPoint + Vector{0.f, 0.f, sidePartsZOffset});
			drawSideParts(Vector{-27.f, -7.f, sidePartsZOffset});

			glPushMatrix();
			glTranslatef(leftFrontWheelPoint.X, leftFrontWheelPoint.Y, sidePartsZOffset);
			glutSolidCylinder(0.5f, 20.f, 20, 20);
			glPopMatrix();

			glPushMatrix();
			glTranslatef(rightFrontWheelPoint.X, rightFrontWheelPoint.Y, sidePartsZOffset);
			glutSolidCylinder(0.5f, 20.f, 20, 20);
			glPopMatrix();

		}

		void toggleWireframeMode()
		{
			wireFrame = !wireFrame;
			glutPostRedisplay();
		}

		private:

		Vector getPointOnSegment(const Vector& v1, const Vector& v2, const float k) const noexcept
		{
			return (v1 + v2 * k) * (1 / (1 + k));
		}

		void drawVertices(const std::vector<Vector>& vertices) const 
		{
			glColor3f(0.f, 0.f, 0.f);
			glPointSize(5.5f);
			glBegin(GL_POINTS);
				for(const auto& v : vertices)
					glVertex3f(v.X, v.Y, v.Z);
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

		void draw3DWheel(const Vector& where) const
		{
			glPushMatrix();

			const auto ZOffset = where.Z == sidePartsZOffset ? -0.5f : -3.5f;

			glTranslatef(where.X, where.Y, where.Z + ZOffset);
			glColor3f(0.f, 0.f, 0.f);
			glutSolidCylinder(1.05, 4.f, 20, 20);
			glPopMatrix();
			
			const int N = 6;
			const auto path = 4.f;
			const auto step = path / (N - 1);
			
			for(int i{}; i < N; ++i)
			{
				const auto offset = (where.Z == sidePartsZOffset) ? step * i : -(step * i);
				drawWheel(where + Vector(0.f, 0.f, offset), 0.5f);
			}
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
				Vector{0.f, 17.5f}, //28
				Vector{5.2f, 18.5f}, //29
				Vector{0.f, 17.f}, //30
				Vector{5.2f, 15.5f}, //31
				Vector{16.f, 14.f}, //32
				Vector{18.f, 14.f}, //33
				Vector{16.f, 18.5f}, //34
				Vector{18.f, 20.f}, //35
				Vector{17.f, 27.5f}, //36
				Vector{5.2f, 20.1f}, //37
			};

			for(auto& i : r)
				i += where;

			const auto rCopyPrevPrev = r[r.size() - 2];
			const auto rCopyPrev = r[r.size() - 1];
			const auto rCopyBulbsEdge5 = Vector{ r[5].X, 14.f, r[5].Z };
			const auto rCopyBulbsEdge30 = Vector{ r[30].X, 16.f, r[5].Z };

			const auto ZOffset = Vector(0.f, 0.f, (where.Z == sidePartsZOffset ? 1.f : -1.f));
			r.push_back(rCopyPrevPrev + ZOffset); // 38
			r.push_back(rCopyPrev + ZOffset); // 39

			const auto Z = where.Z == sidePartsZOffset ? sidePartsZOffset + 5.f : -5.f;

			r.push_back(Vector{where.X, where.Y + 14.f, Z}); // 40
			r.push_back(Vector{where.X, where.Y + 16.f, Z}); // 41

			r.push_back(where + Vector{56.7f, 6.5f} + (ZOffset * 2)); //42
			r.push_back(where + Vector{56.7f, 6.5f} + (ZOffset * 5)); //43
			r.push_back(where + Vector{56.7f, 3.f} + (ZOffset * 2)); //44
			r.push_back(where + Vector{56.7f, 3.f} + (ZOffset * 5)); //45
			r.push_back(getPointOnSegment(where + Vector{56.7f, 8.f} + (ZOffset * 1.5f), 
											where + Vector{51.9f, 29.f} + (ZOffset * 1.5f), 3.f)); //46
			r.push_back(getPointOnSegment(where + Vector{51.9f, 29.f} + (ZOffset * 1.5f), 
											where + Vector{56.7f, 8.f} + (ZOffset * 1.5f), 7.f)); //47
			r.push_back(where + Vector{56.7f, 8.f} + (ZOffset * 5)); //48
			r.push_back(where + Vector{56.7f, 0.f} + (ZOffset * 5)); //49

			r.push_back(where + Vector{-2.8f, 6.f} + (ZOffset * 3.f)); // 50
			r.push_back(where + Vector{-2.8f, 3.f} + (ZOffset * 3.f)); // 51

			return r;
		}

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
				41, 40, 30, 5, 29, 31
			};

			return r;
		}

		std::vector<unsigned int> getWindshieldsPartIndices() const
		{
			std::vector<unsigned int> r;

			std::vector<unsigned int> windshieldsOnSideParts
			{
				8,26,32,34,34,35,35,33,24,23,24,24,33,33,23,14
			};

			for(const auto i : windshieldsOnSideParts)
				r.push_back(i);

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
				getBodyParts(where),
				getBodyParts({where.X, where.Y, where.Z == sidePartsZOffset ? 0.f : sidePartsZOffset})	
			};
			
			const auto indecies = getBodyPartIndices();

			const auto vertices = getFilledVertices(bodyParts);
			glVertexPointer(3, GL_FLOAT, 0, vertices.data());
			
			if(!wireFrame)
				glColor3f(1.f, 0.f, 0.f);
			
			glDrawElements(GL_TRIANGLE_STRIP, indecies.size(), GL_UNSIGNED_INT, (const void**)(indecies.data()));
		}

		void drawLabel() const
		{
			std::vector<std::vector<Vector>> bodyParts{
				getBodyParts(where),
				getBodyParts({where.X, where.Y, where.Z == sidePartsZOffset ? 0.f : sidePartsZOffset}),
			};

			const auto indecies = std::vector<unsigned int>{
				50, 51, getSecondBodyPartIndex(50), getSecondBodyPartIndex(51)
			};

			const auto vertices = getFilledVertices(bodyParts);

			glVertexPointer(3, GL_FLOAT, 0, vertices.data());

			if(!wireFrame)
				glColor3f(1.f, 1.f, 1.f);

			const auto pos = where + Vector{-2.8f, 3.5f} + Vector(0.f, 0.f, -17.f);

			glColor3f(0.f, 0.f, 0.f);
			glRasterPos3f(pos.X, pos.Y, pos.Z);
			
			glPushMatrix();
			glLineWidth(3.f);
			glTranslatef(-30.f, -3.5f, -16.4f);
			glRotatef(-90.f, 0.f, 1.f, 0.f);
			glScalef(0.018f, 0.018f, 0.018f);
			glutStrokeString(GLUT_STROKE_ROMAN, (const unsigned char*)"B382AM197");
			glLineWidth(1.f);
			glPopMatrix();

			//glutBitmapStr(GLUT_BITMAP_HELVETICA_18, "B382AM197");
			glColor3f(1.f, 1.f, 1.f);
			glDrawElements(GL_TRIANGLE_STRIP, indecies.size(), GL_UNSIGNED_INT, (const void**)(indecies.data()));
		}

		void drawLightBulbs() const
		{
			std::vector<std::vector<Vector>> bodyParts{
				getBodyParts(where),
			};

			const auto indecies = getLightBulbsPartIndices();
			const auto vertices = getFilledVertices(bodyParts);

			glVertexPointer(3, GL_FLOAT, 0, vertices.data());

			if(!wireFrame)
				glColor3f(1.f, 1.f, 0.f);

			glDrawElements(GL_TRIANGLE_STRIP, indecies.size(), GL_UNSIGNED_INT, (const void**)(indecies.data()));
		}

		void drawBackLightBulbs() const
		{
			std::vector<std::vector<Vector>> bodyParts{
				getBodyParts(where),
				getBodyParts({where.X, where.Y, where.Z == sidePartsZOffset ? 0.f : sidePartsZOffset}),
			};

			const auto indecies = std::vector<unsigned int>{
				42, 43, 44, 45, 45,
				getSecondBodyPartIndex(42), getSecondBodyPartIndex(42),
				getSecondBodyPartIndex(43), getSecondBodyPartIndex(44),
				getSecondBodyPartIndex(45)
			};

			const auto vertices = getFilledVertices(bodyParts);

			glVertexPointer(3, GL_FLOAT, 0, vertices.data());

			if(!wireFrame)
				glColor3f(1.f, 1.f, 0.f);

			glDrawElements(GL_TRIANGLE_STRIP, indecies.size(), GL_UNSIGNED_INT, (const void**)(indecies.data()));
		}

		void drawFrontWindshield() const
		{
			std::vector<std::vector<Vector>> bodyParts{
				getBodyParts(where),
				getBodyParts({where.X, where.Y, where.Z == sidePartsZOffset ? 0.f : sidePartsZOffset})
			};

			const auto indecies =  
				std::vector<unsigned int>{
					38, getSecondBodyPartIndex(38),
					39, getSecondBodyPartIndex(39), getSecondBodyPartIndex(39),
					27, 27, 26, 26, 8
			};

			const auto vertices = getFilledVertices(bodyParts);
			glVertexPointer(3, GL_FLOAT, 0, vertices.data());
			
			if(!wireFrame)
				glColor3f(0.9f, 0.9f, 0.9f);
			
			glDrawElements(GL_TRIANGLE_STRIP, indecies.size(), GL_UNSIGNED_INT, (const void**)(indecies.data()));
		}

		void drawBackWindshield() const
		{
			std::vector<std::vector<Vector>> bodyParts{
				getBodyParts(where),
				getBodyParts({where.X, where.Y, where.Z == sidePartsZOffset ? 0.f : sidePartsZOffset})
			};

			const auto indecies =  
				std::vector<unsigned int>{
					47, getSecondBodyPartIndex(47),
					46, getSecondBodyPartIndex(46)
			};

			const auto vertices = getFilledVertices(bodyParts);
			glVertexPointer(3, GL_FLOAT, 0, vertices.data());
			
			if(!wireFrame)
				glColor3f(0.9f, 0.9f, 0.9f);
			
			glDrawElements(GL_TRIANGLE_STRIP, indecies.size(), GL_UNSIGNED_INT, (const void**)(indecies.data()));
		}

		void drawWindshields() const
		{	
			std::vector<std::vector<Vector>> bodyParts{
				getBodyParts(where),
			};

			const auto indecies = getWindshieldsPartIndices();

			const auto vertices = getFilledVertices(bodyParts);
			glVertexPointer(3, GL_FLOAT, 0, vertices.data());
			
			if(!wireFrame)
				glColor3f(0.9f, 0.9f, 0.9f);
			
			glDrawElements(GL_TRIANGLE_STRIP, indecies.size(), GL_UNSIGNED_INT, (const void**)(indecies.data()));
		}

		void drawSteeringWheel() const
		{
			std::vector<std::vector<Vector>> bodyParts{
				getBodyParts(where)	
			};

			const auto indecies = getSteeringWheelPartIndices();

			const auto vertices = getFilledVertices(bodyParts);
			glVertexPointer(3, GL_FLOAT, 0, vertices.data());
			
			if(!wireFrame)
				glColor3f(0.0f, 0.0f, 0.0f);
			
			glDrawElements(GL_TRIANGLE_STRIP, indecies.size(), GL_UNSIGNED_INT, (const void**)(indecies.data()));
		}

		void drawSideParts(const Vector& pWhere) const
		{	
			where = pWhere;
			drawBodyParts();
			drawLightBulbs();
			drawWindshields();
			drawSteeringWheel();
		}

		unsigned int getSecondBodyPartIndex(int index) const
		{
			const auto size = getBodyParts(where).size();
			return index + size;
		}

		std::vector<unsigned int> getBodyMiddleIndices() const
		{
			std::vector<unsigned int> r;

			std::vector<unsigned int> hoodIndecies{
				getSecondBodyPartIndex(27), getSecondBodyPartIndex(37), 27, 37, 27, 
				27, getSecondBodyPartIndex(27), 28, getSecondBodyPartIndex(28), 28, 28, 30, 30, 28, 41, 
				getSecondBodyPartIndex(28), getSecondBodyPartIndex(41), getSecondBodyPartIndex(30),
				getSecondBodyPartIndex(41), getSecondBodyPartIndex(41), 41, getSecondBodyPartIndex(40), 40,
				getSecondBodyPartIndex(5), 5	
			};

			const std::vector<unsigned int> preFrontPartIndecies{
				5, 3, getSecondBodyPartIndex(5), getSecondBodyPartIndex(3), getSecondBodyPartIndex(3), 3, 3, 
				50, 1, 51, getSecondBodyPartIndex(1), getSecondBodyPartIndex(51), getSecondBodyPartIndex(3), 
				getSecondBodyPartIndex(50), 3, 50, 50, 3, 3, 1, 
			};

			const std::vector<unsigned int> frontPartIndecies{
				1, 0, 4, 7, 9, 10, 11, 12, 13, 15, 16, 17, 18,
			};

			const std::vector<unsigned int> backPartIndecies{
				18, 44, 20, 42, 48, 43, getSecondBodyPartIndex(48), 
				getSecondBodyPartIndex(43), getSecondBodyPartIndex(20), getSecondBodyPartIndex(42),
				getSecondBodyPartIndex(18), getSecondBodyPartIndex(44), getSecondBodyPartIndex(49),
				getSecondBodyPartIndex(45), 49, 45, 45, getSecondBodyPartIndex(45), 
				43, getSecondBodyPartIndex(43), getSecondBodyPartIndex(43), 20,
				20, 47, 21, 46, getSecondBodyPartIndex(21), getSecondBodyPartIndex(46),
				getSecondBodyPartIndex(20), getSecondBodyPartIndex(47), 20, 47, 20, 20, 21
			};

			const std::vector<unsigned int> frontPartIndeciesContinue{
				21, 22, 25, 36
			};

			std::vector<unsigned> bordersOfWindshield{
				36, 36, 38, 27, 39, 36, 36, 
				getSecondBodyPartIndex(36), getSecondBodyPartIndex(36), getSecondBodyPartIndex(38), 
				getSecondBodyPartIndex(27), getSecondBodyPartIndex(39)
			};

			r = std::move(hoodIndecies);

			r.reserve(hoodIndecies.size() + 2 * frontPartIndecies.size());

			for(const auto i : preFrontPartIndecies)
			{
				r.push_back(i);
			}

			for(const auto i : frontPartIndecies)
			{
				r.push_back(i);
				r.push_back(getSecondBodyPartIndex(i));
			}

			for(const auto i : backPartIndecies)
			{
				r.push_back(i);
			}

			for(const auto i : frontPartIndeciesContinue)
			{
				r.push_back(i);
				r.push_back(getSecondBodyPartIndex(i));
			}
			
			for(const auto i : bordersOfWindshield)
			{
				r.push_back(i);
			}

			return r;
		} 

		std::vector<Vector> getSecondBodyParts() const
		{
			return getBodyParts({where.X, where.Y, sidePartsZOffset});
		}
 
		void drawMiddleSideParts() const
		{
			std::vector<std::vector<Vector>> bodyParts{
				getBodyParts(where),
				getBodyParts({where.X, where.Y, where.Z == sidePartsZOffset ? 0.f : sidePartsZOffset}),
			};

			std::vector<std::vector<unsigned int>> partsIndices{
				getBodyMiddleIndices()
			};

			const auto vertices = getFilledVertices(bodyParts);
			glVertexPointer(3, GL_FLOAT, 0, vertices.data());
			
			const auto countIndices = getCountIndices(partsIndices);
			const auto indices = getPartsIndicesPtr(partsIndices);
			
			if(!wireFrame)
				glColor3f(0.9f, 0.0f, 0.0f);
			
			glMultiDrawElements(GL_TRIANGLE_STRIP, countIndices.data(), GL_UNSIGNED_INT, (const void**)(indices.data()), indices.size());
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

			glVertexPointer(3, GL_FLOAT, 0, vertices.data());

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
				const auto vertex = Vector(center.X + R * cos(t), center.Y + R * sin(t), center.Z);
				r.push_back(vertex);
			}

			return r;
		}

		bool wireFrame;
		mutable Vector where;
		float sidePartsZOffset;

	} car;

	template<typename T>
	T clamp(T min, T max, T val)
	{
		return min > val ? min : val > max ? max : val;
	}

	struct Camera
	{
		explicit Camera(float pR)
			:
			R{ pR },
			RStep{1.f},
			RMin{10.f},
			RMax{pR},
			center{-10.f, 0.f, 0.f},
			i{},
			j{}
		{
		}

		void tick() noexcept
		{
			const auto loc = getNextCoord(i, j);
			const auto upDir = getUpDir();

			gluLookAt(loc.X, loc.Y, loc.Z, center.X, center.Y, center.Z, upDir.X, upDir.Y, upDir.Z);

			tryToRecalcCoords();
		}

		void specialKeys(int key, int x, int y) noexcept
		{
			if (key == GLUT_KEY_LEFT)
			{
				i += 1;
				glutPostRedisplay();
			}
			else if (key == GLUT_KEY_RIGHT)
			{
				i -= 1;
				glutPostRedisplay();
			}
			else if (key == GLUT_KEY_UP)
			{
				j += 1;
				glutPostRedisplay();
			}
			else if (key == GLUT_KEY_DOWN)
			{
				if (j == 0)
				{
					j = q;
				}

				j -= 1;

				glutPostRedisplay();
			}
		}

		void mouseWheel(int dir) noexcept
		{
			R = clamp(RMin, RMax, R + RStep * dir * -1.f);
			glutPostRedisplay();
		}

	private:

		float R;
		float RStep;
		float RMin;
		float RMax;

		Vector center;

		int i;
		int j;

		const std::size_t p{30};
		const std::size_t q{30};

		Vector getNextCoord(int nextI, int nextJ) const noexcept
		{
			const float fi = 2.0 * M_PI * nextJ / q;
			const float th = 2.0 * M_PI * nextI / p;

			return { R * cos(fi) * cos(th), R * sin(fi), R * cos(fi) * sin(th) };
		}

		bool inRange(float min, float max, float v) const noexcept
		{
			return (min <= v && v <= max);
		}

		Vector getUpDir() const noexcept
		{
			const float fi = 2.0 * M_PI * j / q;
			const auto lookDown = Vector{ 0.f, -1.f, 0.f };
			const auto lookUp = Vector{ 0.f, 1.f, 0.f };

			return inRange(M_PI_2, 3 * M_PI_2, fi) ? lookDown : lookUp;
		}

		void tryToRecalcCoords()
		{
			const float fi = 2.0 * M_PI * j / q;
			const float th = 2.0 * M_PI * i / p;

			if (fi >= 2.0 * M_PI)
				j -= q;

			if (th >= 2.0 * M_PI)
				i -= p;
		}

	} camera{ 60.f };

	void drawScene(void)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLoadIdentity();
		camera.tick();

		glRotatef(90.f, 0.f, 1.f, 0.f);

		car.draw();

		glutSwapBuffers();
	}

	void setup(void)
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);
		glEnable(GL_DEPTH_TEST);
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
		case 'g':
		case 'G':
			break;
		
		default:
			break;
		}
	}

	void specialCallback(int key, int x, int y)
	{
		camera.specialKeys(key, x, y);
	}

	void mouseWheelCallback(int button, int dir, int x, int y)
	{
		if(button == 3)
		{
			camera.mouseWheel(dir > 0.f ? 1.f : 0.f);
		}
		else if(button == 4)
		{
			camera.mouseWheel(dir > 0.f ? -1.f : 0.f);
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
		glutSpecialFunc(specialCallback);
		glutMouseFunc(mouseWheelCallback);


		glewExperimental = GL_TRUE;
		glewInit();

		setup();

		glutMainLoop();
	}
}