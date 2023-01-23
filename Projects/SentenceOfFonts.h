//////////////////////////////////////////////////////////////////////////    
// This program displays all fonts available through the GLUT library.
//////////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h> 
#include <vector>

namespace SentenceOfFonts
{
	std::vector<std::pair<void*, std::string>> fonts;
	int fontIndex = 0;

	void initFonts()
	{
		fonts.push_back({ GLUT_BITMAP_8_BY_13, "GLUT_BITMAP_8_BY_13" });
		fonts.push_back({ GLUT_BITMAP_9_BY_15, "GLUT_BITMAP_9_BY_15" });
		fonts.push_back({ GLUT_BITMAP_TIMES_ROMAN_10, "GLUT_BITMAP_TIMES_ROMAN_10" });
		fonts.push_back({ GLUT_BITMAP_TIMES_ROMAN_24, "GLUT_BITMAP_TIMES_ROMAN_24" });
		fonts.push_back({ GLUT_BITMAP_HELVETICA_10, "GLUT_BITMAP_HELVETICA_10" });
		fonts.push_back({ GLUT_BITMAP_HELVETICA_12, "GLUT_BITMAP_HELVETICA_12" });
		fonts.push_back({ GLUT_BITMAP_HELVETICA_18, "GLUT_BITMAP_HELVETICA_18" });
		fonts.push_back({ GLUT_STROKE_ROMAN, "GLUT_STROKE_ROMAN" });
		fonts.push_back({ GLUT_STROKE_MONO_ROMAN, "GLUT_STROKE_MONO_ROMAN" });
	}

	void writeBitmapString(void* font, const std::string& str)
	{
		for (const auto ch : str) glutBitmapCharacter(font, ch);
	}

	void writeStrokeString(void* font, const std::string& str)
	{
		for (const auto ch : str) glutStrokeCharacter(font, ch);
	}

	void drawScene(void)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		const auto bitmapStr = "BITMAP";
		const auto upStr = "I'm having so much fun with OpenGL,";
		const auto downStr = "it can't be legal!";

		const auto fontAddress = fonts[fontIndex].first;
		const auto fontName = fonts[fontIndex].second;

		glColor3f(0.f, 0.f, 0.f);
		glLineWidth(3.0);

		glPushMatrix();
		glTranslated(2.5, 75.0, 0.0);
		glScaled(0.04, 0.04, 0.04);
		writeStrokeString(GLUT_STROKE_ROMAN, "FONT: ");
		glPopMatrix();
	
		glPushMatrix();
		glTranslated(18.5, 75.0, 0.0);
		if (fontName.find("TIMES") == std::string::npos)
		{
			glScaled(0.04, 0.04, 0.04);
		}
		else
		{
			glScaled(0.035, 0.035, 0.035);
		}
		writeStrokeString(GLUT_STROKE_ROMAN, fontName);
		glPopMatrix();

		if (fontName.find(bitmapStr) != std::string::npos)
		{
			glColor3f(0.0, 0.0, 0.0);
			glRasterPos2d(25.0, 55.0);
			writeBitmapString(fontAddress, upStr);

			glColor3f(1.f, 0.f, 0.f);
			glRasterPos2d(37.0, 50.0);
			writeBitmapString(fontAddress, downStr);
		}
		else
		{
			glColor3f(0.f, 0.f, 0.f);
			glLineWidth(1.5);
			glPushMatrix();
			glRasterPos2d(50.0, 50.0);
			glTranslatef(19.0, 55.0, 0.0);
			glScalef(0.025, 0.025, 0.025);
			writeStrokeString(GLUT_STROKE_ROMAN, upStr);
			glPopMatrix();

			glColor3f(1.f, 0.f, 0.f);
			glPushMatrix();
			glTranslatef(27.5, 47.5, 0.0);
			glScalef(0.04, 0.04, 0.04);
			writeStrokeString(GLUT_STROKE_ROMAN, downStr);
			glPopMatrix();
		}

		glFlush();
	}

	void setup(void)
	{
		initFonts();
		glClearColor(1.0, 1.0, 1.0, 0.0);
	}

	void resize(int w, int h)
	{
		glViewport(0, 0, w, h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0, 100.0, 0.0, 100.0, -1.0, 1.0);
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
		default:
			break;
		}
	}
	
	void specialFunc(int key, int x, int y)
	{
		switch (key)
		{
		case GLUT_KEY_LEFT:

			if (fontIndex > 0)
			{
				--fontIndex;
				glutPostRedisplay();
			}

			break;

		case GLUT_KEY_RIGHT:
		{
			const auto lastIndex = fonts.size() - 1;
			if (fontIndex < lastIndex)
			{
				++fontIndex;
				glutPostRedisplay();
			}

			break;
		}
		default:
			break;
		}
	}

	int main(int argc, char** argv)
	{
		glutInit(&argc, argv);

		glutInitContextVersion(4, 3);
		glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
		glutInitWindowSize(500, 500);
		glutInitWindowPosition(100, 100);
		glutCreateWindow("fonts.cpp");
		glutDisplayFunc(drawScene);
		glutReshapeFunc(resize);
		glutKeyboardFunc(keyInput);
		glutSpecialFunc(specialFunc);

		glewExperimental = GL_TRUE;
		glewInit();

		setup();

		glutMainLoop();

		return 0;
	}
}
