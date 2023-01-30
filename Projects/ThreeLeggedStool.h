
#define _USE_MATH_DEFINES

#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <vector>

namespace ThreeLeggedStool
{
	static unsigned int base;
	static float angleX;
	static float angleY;
	static float angleZ;
	static float zoom = -50.f;

	void drawSeat()
	{
		const auto N = 30;
		const auto R = 10.5f;
		
		std::vector<float> circleV;
		circleV.reserve(N + 1);

		circleV.push_back(0.0);
		circleV.push_back(0.0);
		circleV.push_back(0.0);

		float t;
		for (int i = 0; i <= N; ++i)
		{
			t = 2 * M_PI * i / N;
			circleV.push_back(R * cos(t));
			circleV.push_back(0.f);
			circleV.push_back(R * sin(t));
		}

		glVertexPointer(3, GL_FLOAT, 0, circleV.data());
		
		const auto yUp = 10.0;
		const auto offset = 2.0;

		glPushMatrix();
		glPolygonMode(GL_BACK, GL_FILL);
		glColor3d(169.0 / 255.0, 64 / 255.0, 7 / 255.0);
		glTranslated(0.0, yUp, 0);
		glDrawArrays(GL_TRIANGLE_FAN, 0, circleV.size() / 3);
		glPopMatrix();

		glPushMatrix();
		glColor3f(1.0, 170 / 255.0, 51 / 255.0);
		glTranslated(0.0, yUp + offset, 0);
		glDrawArrays(GL_TRIANGLE_FAN, 0, circleV.size() / 3);
		glPopMatrix();

		std::vector<float> aroundCirclesV;
		circleV.erase(std::begin(circleV), std::begin(circleV) + 3);
		aroundCirclesV.reserve(circleV.size() * 2);

		for (int i = 0; i < circleV.size(); i += 3)
		{
			const auto X = circleV[i];
			const auto Y = circleV[i + 1];
			const auto Z = circleV[i + 2];

			aroundCirclesV.push_back(X);
			aroundCirclesV.push_back(Y);
			aroundCirclesV.push_back(Z);

			aroundCirclesV.push_back(X);
			aroundCirclesV.push_back(Y + offset);
			aroundCirclesV.push_back(Z);
		}

		glPushMatrix();
		glColor3f(1.0, 170 / 255.0, 51 / 255.0);
		glTranslated(0.0, yUp, 0.0);
		glVertexPointer(3, GL_FLOAT, 0, aroundCirclesV.data());
		glDrawArrays(GL_TRIANGLE_STRIP, 0, aroundCirclesV.size() / 3);
		glPopMatrix();	
	}

	void drawScene(void)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glLoadIdentity();
		glTranslatef(0.0, 0.0, zoom);
		glRotated(angleX, 1.0, 0.0, 0.0);
		glRotated(angleY, 0.0, 1.0, 0.0);
		glRotated(angleZ, 0.0, 0.0, 1.0);

		glListBase(base);

		glPushMatrix();
		glRotated(180.0, 0.0, 1.0, 0.0);
		glTranslated(0.0, 0.0, 10.0);
		glCallList(1);
		glPopMatrix();

		glPushMatrix();
		glRotated(60.0, 0.0, 1.0, 0.0);
		glTranslated(0.0, 0.0, 7.0);
		glCallList(1);
		glPopMatrix();

		glPushMatrix();
		glRotated(60.0, 0.0, -1.0, 0.0);
		glTranslated(0.0, 0.0, 7.0);
		glCallList(1);
		glPopMatrix();

		drawSeat();

		glFlush();
	}

	void setup(void)
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);
		
		glEnableClientState(GL_VERTEX_ARRAY);

		base = glGenLists(1);
		glNewList(base, GL_COMPILE);

		glColor3d(169.0 / 255.0, 64 / 255.0, 7 / 255.0);

		const std::vector<float> rectV{
			 -1.5f, 10.f, -15.f,
			 -1.5f, 10.f, -12.f,
				1.5f, 10.f, -15.f,
				1.5f, 10.f, -12.f,
			-1.f, -20.f, -30.f,
			-1.f, -20.f, -28.5f,
				1.f, -20.f, -30.f,
				1.f, -20.f, -28.5f
		};

		const std::vector<unsigned int> indices{
			0, 1, 4, 5, 6, 7, 7, 5, 3, 1, 2, 0, 6, 4, 6, 2, 7, 3
		};

		glVertexPointer(3, GL_FLOAT, 0, rectV.data());

		glDrawElements(GL_TRIANGLE_STRIP, indices.size(), GL_UNSIGNED_INT, indices.data());
		
		glEndList();
	}
	
	void resize(int w, int h)
	{
		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);

		glMatrixMode(GL_MODELVIEW);
	}

	void keyInput(unsigned char key, int x, int y)
	{
		switch (key)
		{
		case 27:
			exit(0);
			break;
		case 'x':
			angleX += 5.0;
			if (angleX > 360.0) angleX -= 360.0;
			glutPostRedisplay();
			break;
		case 'X':
			angleX -= 5.0;
			if (angleX < 0.0) angleX += 360.0;
			glutPostRedisplay();
			break;
		case 'y':
			angleY += 5.0;
			if (angleY > 360.0) angleY -= 360.0;
			glutPostRedisplay();
			break;
		case 'Y':
			angleY -= 5.0;
			if (angleY < 0.0) angleY += 360.0;
			glutPostRedisplay();
			break;
		case 'z':
			angleZ += 5.0;
			if (angleZ > 360.0) angleZ -= 360.0;
			glutPostRedisplay();
			break;
		case 'Z':
			angleZ -= 5.0;
			if (angleZ < 0.0) angleZ += 360.0;
			glutPostRedisplay();
			break;
		case 'q':
			if(zoom > -100.0)
				zoom += -1.0;
			glutPostRedisplay();
			break;
		
		case 'Q':
			if (zoom < -50.0)
				zoom += 1.0;
			glutPostRedisplay();

			break;
	
		default:
			break;
		}
	}

	int main(int argc, char** argv)
	{
		glutInit(&argc, argv);

		glutInitContextVersion(4, 3);
		glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
		glutInitWindowSize(500, 500);
		glutInitWindowPosition(100, 100);
		glutCreateWindow("ThreeLeggedStool");
		glutDisplayFunc(drawScene);
		glutReshapeFunc(resize);
		glutKeyboardFunc(keyInput);

		glewExperimental = GL_TRUE;
		glewInit();

		setup();

		glutMainLoop();

		return 0;
	}
}