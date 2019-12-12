#define _CRT_SECURE_NO_WARNINGS

#include <glut.h>
#include <Windows.h>
#include <wingdi.h>
#include <windef.h>
#include "LSystem.h"
#include "tree.h"
#include "grammar.h"
#include "transformation.h"

GLint		WindW = 800;
GLint		WindH = 600;
GLfloat		oldx, oldy;
GLfloat		scale = 1.0;
GLfloat		xangle = 15;
GLfloat		yangle = -15;
GLuint		textOut;
GLuint		textIn;

#define BMP_HEADER_LENGTH 54

GLfloat		Pi1 = 3.14159;
Tree		tree;
LSystem		lsrule(tree);

int LoadGLTextures(char* fileName, GLuint* ttexture, int i)
{
	FILE* file;
	BITMAPINFOHEADER header;
	BYTE* texture;
	int width, height, m, j;
	unsigned char* image;
	
	file = fopen(fileName, "rb");

	if (file) {
		fseek(file, 14, SEEK_SET);
		fread(&header, sizeof(BITMAPINFOHEADER), 1, file);
	}
	else
		return FALSE;

	width = header.biWidth;
	height = header.biHeight;

	image = (unsigned char*)malloc(width * height * 3);
	fread(image, sizeof(unsigned char), width * height * 3, file);
	texture = (BYTE*)malloc(width * height * 4);

	for (m = 0; m < width; m++)
	{
		for (j = 0; j < height; j++)
		{
			texture[m * width * 4 + j * 4] = image[m * width * 3 + j * 3];
			texture[m * width * 4 + j * 4 + 1] = image[m * width * 3 + j * 3 + 1];
			texture[m * width * 4 + j * 4 + 2] = image[m * width * 3 + j * 3 + 2];

			if (texture[m * width * 4 + j * 4] >= 200 && texture[m * width * 4 + j * 4 + 1] >= 200 && texture[m * width * 4 + j * 4 + 2] >= 200) {
				texture[m * width * 4 + j * 4 + 3] = 0;                 //?????alpha=0   
			}
			else
				texture[m * width * 4 + j * 4 + 3] = 255;           //???????alpha=255  
		}
	}

	glGenTextures(1, &ttexture[i]);
	glBindTexture(GL_TEXTURE_2D, ttexture[i]);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, texture);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glAlphaFunc(GL_GREATER, 0.5);

	free(texture);
	free(image);
	return TRUE;
}

int powerOf2(int n) {
	if (n <= 0)
		return 0;
	return (n & (n - 1)) == 0;
}

GLuint loadTexture(const char* fileName) {
	GLint width, height, totalBytes;
	GLubyte* pixels = 0;
	GLuint lastTextureID = 0, textureID = 0;

	FILE* pfile = fopen(fileName, "rb");
	if (pfile == 0)
		return 0;

	fseek(pfile, 0x0012, SEEK_SET);
	fread(&width, 4, 1, pfile);
	fread(&height, 4, 1, pfile);
	fseek(pfile, BMP_HEADER_LENGTH, SEEK_SET);

	{
		GLint lineBytes = width * 3;
		while (lineBytes % 4 != 0)
			++lineBytes;
		totalBytes = lineBytes * height;
	}

	pixels = (GLubyte*)malloc(totalBytes);
	if (!pixels) {
		fclose(pfile);
		return 0;
	}

	if (fread(pixels, totalBytes, 1, pfile) <= 0) {
		free(pixels);
		fclose(pfile);
		return 0;
	}

	{
		GLint max;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
		if (!powerOf2(width)
			|| !powerOf2(height)
			|| width > max
			|| height > max) {
			const GLint newWidth = 256;
			const GLint newHeight = 256;
			GLint		newLineBytes, newTotalBytes;
			GLubyte*		newPixels = 0;

			newLineBytes = newWidth * 3;
			while (newLineBytes % 4 != 0)
				++newLineBytes;
			newTotalBytes = newLineBytes * newHeight;

			newPixels = (GLubyte*)malloc(newTotalBytes);
			if (!newPixels) {
				free(pixels);
				fclose(pfile);
				return 0;
			}

			gluScaleImage(GL_RGB,
				width, height, GL_UNSIGNED_BYTE, pixels,
				newWidth, newHeight, GL_UNSIGNED_BYTE, newPixels);

			free(pixels);
			pixels = newPixels;
			width = newWidth;
			height = newHeight;
		}
	}

	glGenTextures(1, &textureID);
	if (!textureID) {
		free(pixels);
		fclose(pfile);
		return 0;
	}

	GLint lastTextureIDN = lastTextureID;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTextureIDN);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
		GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
	glBindTexture(GL_TEXTURE_2D, lastTextureIDN);
	free(pixels);
	return textureID;
}

void init() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-25.f, 25.f, -25.f, 25.f, -25.f, 25.f);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	lsrule.initGrammar();
	lsrule.generateFractal();
}

void drawCone(double r, double h) {
	glBindTexture(GL_TEXTURE_2D, textIn);
	for (int i = 0; i < 360; i += 50) {
		float temp = i * Pi1 / 180;
		float temp1 = (i + 50) * Pi1 / 180;
		glBegin(GL_QUADS);
		glTexCoord2f((temp * r * tree.trunk.radiusShrink) / (2 * Pi1), 0.0f);
		glVertex3f(r * cos(temp) * tree.trunk.radiusShrink, 0, r * sin(temp) * tree.trunk.radiusShrink);
		glTexCoord2f((temp * r) / (2 * Pi1), 1.0f);
		glVertex3f(r * cos(temp), h, r * sin(temp));
		glTexCoord2f((temp1 * r) / (2 * Pi1), 1.0f);
		glVertex3f(r * cos(temp1), h, r * sin(temp1));
		glTexCoord2f((temp1 * r * tree.trunk.radiusShrink) / (2 * Pi1), 0.0f);
		glVertex3f(r * cos(temp1) * tree.trunk.radiusShrink, 0.0f, r * sin(temp1) * tree.trunk.radiusShrink);
		glEnd();
	}
}

void drawChannel(Node A, Node B, double r) {
	GLfloat		dx = B.x - A.x;
	GLfloat		dy = B.y - A.y;
	GLfloat		dz = B.z - A.z;

	GLfloat		distance = sqrt(dx * dx + dy * dy + dz * dz);

	GLfloat		px = A.x;
	GLfloat		py = A.y - 1;
	GLfloat		pz = A.z;

	GLfloat		bx = px;
	GLfloat		by = (1 - distance) + py;
	GLfloat		bz = pz;

	GLfloat		sx = bx - A.x;
	GLfloat		sy = by - A.y;
	GLfloat		sz = bz - A.z;

	GLfloat		fx = sy * dz - dy * sz;
	GLfloat		fy = sz * dx - sx * dz;
	GLfloat		fz = sx * dy - dx * sy;

	GLfloat		ax = fabs(B.x - bx);
	GLfloat		ay = fabs(B.y - by);
	GLfloat		az = fabs(B.z - bz);
	GLfloat		length = sqrt(ax * ax + ay * ay + az * az);

	GLfloat		angle = acos((distance * distance * 2 - length * length) / (2 * distance * distance)) * 180.0f / 3.14159;

	glPushMatrix();

	glTranslatef(A.x, A.y, A.z);
	glRotatef(angle, fx, fy, fz);
	glTranslatef(0, -distance, 0);
	drawCone(r, distance);
	glPopMatrix();
}

void drawSquare(double r) {
	glRotated(180, 0, 0, 1);
	glBindTexture(GL_TEXTURE_2D, textOut);
	glBegin(GL_QUADS);
	glTexCoord2f(0., 0.); glVertex3f(r / 2, 0, 0);
	glTexCoord2f(0., 1.); glVertex3f(r / 2, r, 0);
	glTexCoord2f(1., 1.); glVertex3f(-r / 2, r, 0);
	glTexCoord2f(1., 0.); glVertex3f(-r / 2, 0, 0);

	glEnd();
}

void drawLeaf(Node A, Node B, double r) {
	GLfloat		dx = B.x - A.x;
	GLfloat		dy = B.y - A.y;
	GLfloat		dz = B.z - A.z;

	GLfloat		distance = sqrt(dx * dx + dy * dy + dz * dz);

	GLfloat  px = A.x;
	GLfloat  py = A.y - 1;
	GLfloat  pz = A.z;
  
	GLfloat  bx = px;
	GLfloat  by = (1 - distance) + py;
	GLfloat  bz = pz;
  
	GLfloat  sx = bx - A.x;
	GLfloat  sy = by - A.y;
	GLfloat  sz = bz - A.z;
  
	GLfloat fx = sy * dz - dy * sz;
	GLfloat fy = sz * dx - sx * dz;
	GLfloat fz = sx * dy - dx * sy;
  
	GLfloat ax = fabs(B.x - bx);
	GLfloat ay = fabs(B.y - by);
	GLfloat az = fabs(B.z - bz);
	GLfloat length = sqrt(ax * ax + ay * ay + az * az);
  
	GLfloat angle = acos((distance * distance * 2 - length * length) / (2 * distance * distance)) * 180.0f / 3.14159;
	glPushMatrix();

	glTranslatef(A.x, A.y, A.z);
	glRotatef(angle, fx, fy, fz);
	glTranslatef(0, -distance, 0);
	drawSquare(r);
	glPopMatrix();
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);
	glLoadIdentity();
	glColor4f(1, 1, 1, 1);

	glRotatef(xangle, 1, 0, 0);
	glRotatef(yangle, 0, 1, 0);

	glScalef(scale, scale, scale);
	glTranslatef(0, -20, 0);

	for (int i = 0; i < lsrule.trunks.size(); i++) {
		drawChannel(lsrule.trunks[i].pos[0], lsrule.trunks[i].pos[1], lsrule.trunks[i].radius);
	}
	for (int i = 0; i < lsrule.leaves.size(); i++) {
		// cout << lsrule.leaves[i].radius << endl;
		drawLeaf(lsrule.leaves[i].pos[0], lsrule.leaves[i].pos[1], lsrule.leaves[i].radius);
	}

	glFlush();
	glutSwapBuffers();
}

void reshape(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	GLfloat f = 25.0f;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h)
		glOrtho(-f, f, -f * (GLfloat)h / (GLfloat)w,
			f * (GLfloat)h / (GLfloat)w, -f, f);
	else
		glOrtho(-f * (GLfloat)w / (GLfloat)h,
			f * (GLfloat)w / (GLfloat)h, -f, f, -f, f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y) {
	char b[] = "res//Maple1.bmp";
	char c[] = "res//starLeaf.bmp";
	switch (key) {
	case 'w':
	case 'W':
		scale += 0.25;
		break;
	case 's':
	case 'S':
		if (scale <= 0)
			return;
		scale -= 0.25;
	}
	glutPostRedisplay();
}

void mouse(int btn, int state, int x, int y) {
	if ((btn == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
		oldx = x;
		oldy = y;
	}
}

void motion(int x, int y) {
	GLint deltax = oldx - x;
	GLint deltay = oldy - y;

	yangle += 360 * (GLfloat)deltax / (GLfloat)WindW;
	xangle += 360 * (GLfloat)deltay / (GLfloat)WindH;
	oldx = x;
	oldy = y;
	glutPostRedisplay();
}


int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(200, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("tree");

	char b[] = "res/starLeaf.bmp";
	LoadGLTextures(b, &textOut, 0);
	textIn = loadTexture("res/bark1.bmp");

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutKeyboardFunc(keyboard);
	init();
	glutMainLoop();
	return 0;
}
