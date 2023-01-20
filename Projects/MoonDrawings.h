#pragma once
///////////////////////////////////////////////////////         
// childrenSide.h
//
// This program draws 3 pictures of the moon:
// 1. crescent moon
// 2. half-moon
// 3. three quarter-moon
// 
// Press space for switch between wireframe and fill modes
// 
///////////////////////////////////////////////////////        

#define _USE_MATH_DEFINES 

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

namespace MoonDrawings
{
	static auto N = 20;
	static auto R = 20.0;
	static auto R0 = 8.0;
	static bool isWire = false;

	void drawM(GLfloat X, GLfloat Y, GLfloat side, GLfloat lineWidth)
	{
		glLineWidth(lineWidth);

		glBegin(GL_LINE_STRIP);

		const auto offset = side / 4;

		glVertex3f(X, Y, 0.f);
		glVertex3f(X + 0.5 * side, Y + 2 * side, 0.f);
		glVertex3f(X + 1 * side, Y + 0.5 * side, 0.f);
		glVertex3f(X + 1.5 * side, Y + 2 * side, 0.f);
		glVertex3f(X + 2 * side, Y, 0.f);

		glEnd();
	}

	void drawO(GLfloat X, GLfloat Y, GLfloat side, GLfloat lineWidth)
	{
		const auto N0 = 10;

		glLineWidth(lineWidth);

		glBegin(GL_LINE_STRIP);

		float t;
		for (int i = 0; i <= N0; ++i)
		{
			t = 2 * M_PI * i / N0;
			
			glVertex3f(X + side * cos(t), Y + side * sin(t), 0.f);
		}

		glEnd();
	}

	void drawN(GLfloat X, GLfloat Y, GLfloat side, GLfloat lineWidth)
	{
		glLineWidth(lineWidth);

		glBegin(GL_LINE_STRIP);

		glVertex3f(X, Y - side / 3, 0.f);
		glVertex3f(X + 2 * side / 3, Y + side * 2, 0.f);
		glVertex3f(X + 4 * side / 3, Y, 0.f);
		glVertex3f(X + 2 * side, Y + side * 2 + side / 3, 0.f);

		glEnd();
	}


	void drawC(GLfloat X, GLfloat Y, GLfloat side, GLfloat lineWidth)
	{
		glLineWidth(lineWidth);

		glBegin(GL_LINE_STRIP);

		const auto R = side;
		const auto curvedAngle = M_PI_2;
		const auto start = curvedAngle / 2;
		const auto path = 2 * M_PI - curvedAngle;
		const auto N0 = 5;

		float t;
		for (int i = 0; i <= N0; ++i)
		{
			t = start + path * i / N0;
			glVertex3f(X + R * cos(t), Y + R * sin(t), 0.f);
		}

		glEnd();
	}


	void drawR(GLfloat X, GLfloat Y, GLfloat side, GLfloat lineWidth)
	{
		glLineWidth(lineWidth);

		glBegin(GL_LINE_STRIP);

		glVertex3f(X, Y, 0.f);
		glVertex3f(X, Y + side, 0.f);

		const auto RA = side / 4;
		const auto RB = side / 2;

		const auto start = M_PI_2;
		const auto path = M_PI;
		const auto N0 = 5;

		const auto OX = X;
		const auto OY = Y + side * 0.75;

		float t;
		for (int i = 1; i <= N0; ++i)
		{
			t = start - path * i / N0;
			glVertex3f(OX + RB * cos(t), OY + RA * sin(t), 0.f);
		}

		glVertex3f(X + side * 0.7, Y, 0.f);

		glEnd();
	}

	void drawE(GLfloat X, GLfloat Y, GLfloat side, GLfloat lineWidth)
	{
		glLineWidth(lineWidth);

		glBegin(GL_LINE_STRIP);

		glVertex3f(X + side / 2, Y + side / 2, 0.f);
		glVertex3f(X, Y + side / 2, 0.f);
		glVertex3f(X, Y, 0.f);
		glVertex3f(X + side * 0.3, Y, 0.f);
		glVertex3f(X, Y, 0.f);
		glVertex3f(X, Y - side / 2, 0.f);
		glVertex3f(X + side / 2, Y - side / 2, 0.f);

		glEnd();
	}

	void drawS(GLfloat X, GLfloat Y, GLfloat side, GLfloat lineWidth)
	{
		glLineWidth(lineWidth);
		
		const auto R = side / 3;
		const auto N0 = 7;
		const auto N1 = 4;
		const auto X0 = X + R;
		const auto Y0 = Y + 2 * side / 3;

		glBegin(GL_LINE_STRIP);

		float t;
		for (int i = 0; i <= N0; ++i)
		{
			t = M_PI * i / N0;
			glVertex3f(X0 + R * cos(t), Y0 + R / 2 * sin(t), 0.f);
		}

		const auto XT = X + side / 3;
		const auto YT = Y0;

		for (int i = 1; i <= N1; ++i)
		{
			t = M_PI + M_PI_2 * i / N1;
			glVertex3f(XT + side / 3 * cos(t), YT + side / 6 * sin(t), 0.f);
		}

		for (int i = 1; i <= N1; ++i)
		{
			t = M_PI_2 - M_PI_2 * i / N1;
			glVertex3f(XT + side / 3 * cos(t), (YT - side / 3) + side / 6 * sin(t), 0.f);
		}

		for (int i = 0; i <= N0; ++i)
		{
			t = 2 * M_PI - M_PI * i / N0;
			glVertex3f(XT + side / 3 * cos(t), (YT - side / 3) + side / 6 * sin(t), 0.f);
		}
		
		glEnd();
	}

	void drawT(GLfloat X, GLfloat Y, GLfloat side, GLfloat lineWidth)
	{
		glLineWidth(lineWidth);

		glBegin(GL_LINE_STRIP);

		glVertex3f(X, Y, 0.f);
		glVertex3f(X + side * 1.5, Y, 0.f);
		glVertex3f(X + side * 1.5 / 2, Y, 0.f);
		glVertex3f(X + side * 1.5 / 2, Y - side * 2, 0.f);

		glEnd();
	}

	void drawH(GLfloat X, GLfloat Y, GLfloat side, GLfloat lineWidth)
	{
		glLineWidth(lineWidth);

		glBegin(GL_LINE_STRIP);

		glVertex3f(X, Y, 0.f);
		glVertex3f(X, Y + side * 2, 0.f);
	
		glVertex3f(X, Y + side, 0.f);
		glVertex3f(X + side, Y + side, 0.f);
		glVertex3f(X + side, Y + side * 2, 0.f);
		glVertex3f(X + side, Y, 0.f);

		glEnd();
	}

	void drawA(GLfloat X, GLfloat Y, GLfloat side, GLfloat lineWidth)
	{
		glLineWidth(lineWidth);

		glBegin(GL_LINE_STRIP);
		
		glVertex3f(X, Y, 0.f);
		glVertex3f(X + side / 2, Y + side, 0.f);
		glVertex3f(X + side, Y, 0.f);
		glVertex3f(X + side * 0.7f, Y + side * 0.4f, 0.f);
		glVertex3f(X + side * 0.25f, Y + side * 0.4f, 0.f);

		glEnd();
	}

	void drawL(GLfloat X, GLfloat Y, GLfloat side, GLfloat lineWidth)
	{
		glLineWidth(lineWidth);

		glBegin(GL_LINE_STRIP);

		glVertex3f(X, Y, 0.f);
		glVertex3f(X, Y + side * 2.f, 0.f);
		glVertex3f(X, Y, 0.f);
		glVertex3f(X + side, Y, 0.f);

		glEnd();
	}

	void drawF(GLfloat X, GLfloat Y, GLfloat side, GLfloat lineWidth)
	{
		glLineWidth(lineWidth);
		
		glBegin(GL_LINE_STRIP);
		
		glVertex3f(X, Y, 0.f);
		glVertex3f(X, Y + side, 0.f);
		glVertex3f(X + side * 0.7, Y + side, 0.f);
		glVertex3f(X, Y + side, 0.f);
		glVertex3f(X, Y + side * 2, 0.f);
		glVertex3f(X + side, Y + side * 2, 0.f);

		glEnd();
	}

	void drawQ(GLfloat X, GLfloat Y, GLfloat side, GLfloat lineWidth)
	{
		glLineWidth(lineWidth);

		glBegin(GL_LINE_STRIP);

		const auto R = side;
		const auto N0 = 10;
		float t;

		glVertex3f(X + side * 0.3, Y - side * 0.3, 0.f);
		glVertex3f(X + R * cos(-M_PI_4), Y + R * sin(-M_PI_4), 0.f);

		for (int i = 0; i <= N0; ++i)
		{
			t = -M_PI_4 + 2 * M_PI * i / N0;
			glVertex3f(X + R * cos(t), Y + R * sin(t), 0.f);
		}

		glVertex3f(X + side, Y - side, 0.f);

		glEnd();
	}

	void drawU(GLfloat X, GLfloat Y, GLfloat side, GLfloat lineWidth)
	{
		glLineWidth(lineWidth);

		glBegin(GL_LINE_STRIP);

		const auto RA = side * 0.3;
		const auto RB = side * 0.75;
		const auto X0 = X;
		const auto Y0 = Y + RB;
		const auto N0 = 20;

		float t;
		for (int i = 0; i <= N0; ++i)
		{
			t = M_PI + M_PI * i / N0;
			glVertex3f(X0 + RA * cos(t), Y0 + RB * sin(t), 0.f);
		}

		glVertex3f(X0 + RA, Y - side * 0.25, 0.f);

		glEnd();
	}

	void drawCrescentWord(GLfloat X, GLfloat Y)
	{
		glColor3d(0.8, 0.0, 0.0);

		drawC(X, Y, 1.5f, 3.f);
		drawR(X + 2.5f, Y - 1.5f, 3.f, 3.f);
		drawE(X + 5.5f, Y - 0.2f, 3.f, 3.f);
		drawS(X + 7.7f, Y - 2.f, 4.f, 3.f);
		drawC(X + 13.f, Y, 1.5f, 3.f);
		drawE(X + 15.f, Y - 0.2f, 3.f, 3.f);
		drawN(X + 17.3f, Y - 1.5f, 1.5f, 3.f);
		drawT(X + 21.1f, Y + 1.5f, 1.8f, 3.f);
	}

	void drawHalfWord(GLfloat X, GLfloat Y)
	{
		drawH(X, Y, 2.f, 3.f);
		drawA(X + 3.f, Y, 3.f, 3.f);
		drawL(X + 7.2f, Y + 0.3f, 1.75f, 3.f);
		drawF(X + 10.f, Y, 1.7f, 3.f); 
	}

	void drawThreeQuarterWord(GLfloat X, GLfloat Y)
	{
		drawT(X, Y, 1.5f, 3.f);
		drawH(X + 3.f, Y - 3.f, 1.5f, 3.f);
		drawR(X + 6.f, Y - 3.f, 3.f, 3.f);
		drawE(X + 9.f, Y - 1.5f, 3.f, 3.f);
		drawE(X + 11.5f, Y - 1.5f,3.f ,3.f );

		drawQ(X + 16.75f, Y - 1.6f, 1.7f, 3.f);
		drawU(X + 20.5f, Y - 3.f, 4.f, 3.f);
		drawA(X + 22.75f, Y - 3.f, 3.f, 3.f);
		drawR(X + 26.5f, Y - 3.f, 3.f, 3.f);
		drawT(X + 29.f, Y, 1.5f, 3.f);
		drawE(X + 32.f, Y - 1.5f, 3.f, 3.f);
		drawR(X + 35.f, Y - 3.f, 3.f, 3.f);
	}

	void drawMoonWord(GLfloat X, GLfloat Y)
	{
		glColor3i(0, 0, 0);
		drawM(X, Y, 3.f, 4.f);
		drawO(X + 10.f, Y + 3.f, 2.25f, 4.f);
		drawO(X + 16.f, Y + 3.f, 2.25f, 4.f);
		drawN(X + 20.f, Y + 0.5f, 2.5f, 4.f);
	}

	void drawCrescentMoon(GLfloat X, GLfloat Y)
	{
		glBegin(GL_TRIANGLE_STRIP);

		const auto Z = 0.f;
		float t = M_PI_2;

		for (int i = 1; i < N; ++i)
		{
			t = M_PI_2 + M_PI * i / N;
			
			glVertex3f(X + R * cos(t), Y + R * sin(t), Z);
			glVertex3f(X + R0 * cos(t), Y + R * sin(t), Z);
		}

		t = M_PI + M_PI_2;

		glEnd();
	}

	void drawHalfMoon(GLfloat X, GLfloat Y)
	{
		glBegin(GL_TRIANGLE_FAN);

		const auto Z = 0.f;
		float t;

		glVertex3f(X, Y, Z);

		for (int i = 0; i <= N; ++i)
		{
			t = M_PI_2 + M_PI * i / N;
			glVertex3f(X + R * cos(t), Y + R * sin(t), Z);
		}

		glEnd();
	}

	void drawThreeQuarterMoon(GLfloat X, GLfloat Y)
	{
		glBegin(GL_TRIANGLE_STRIP);

		const auto Z = 0.f;
		float t = M_PI_2;

		for (int i = 0; i <= N; ++i)
		{
			t = M_PI_2 + M_PI * i / N;

			glVertex3f(X + R * cos(t), Y + R * sin(t), Z);
			glVertex3f(X + -R0 * cos(t), Y + R * sin(t), Z);
		}

		t = M_PI + M_PI_2;

		glEnd();
	}

	// Drawing routine.
	void drawScene(void)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, isWire ? GL_LINE : GL_FILL);
		
		if (isWire)
			glColor3f(0.f, 0.f, 0.f);
		else
			glColor3f(0.f, 0.f, 1.f);

		drawCrescentMoon(26.f, 50.f);
		drawHalfMoon(53.0f, 50.f);
		drawThreeQuarterMoon(86.0f, 50.f);

		drawMoonWord(35.f, 14.f);
		drawCrescentWord(4.f, 73.25f);
		drawHalfWord(40.f, 71.5f);
		drawThreeQuarterWord(60.f, 75.f);

		glFlush();
	}

	// Initialization routine.
	void setup(void)
	{
		glClearColor(1.0, 1.0, 1.0, 0.0);
	}

	// OpenGL window reshape routine.
	void resize(int w, int h)
	{
		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0, 100.0, 0.0, 100.0, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	// Keyboard input processing routine.
	void keyInput(unsigned char key, int x, int y)
	{
		switch (key)
		{
		case 27:
			exit(0);
			break;
		case ' ':
			isWire = !isWire;
			glutPostRedisplay();
			break;
		default:
			break;
		}
	}

	// Main routine.
	int main(int argc, char** argv)
	{
		glutInit(&argc, argv);

		glutInitContextVersion(4, 3);
		glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
		glutInitWindowSize(500, 500);
		glutInitWindowPosition(500, 200);
		glutCreateWindow("moonDrawings");
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
