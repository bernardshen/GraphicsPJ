#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <glut.h>
#include <vector>
#include <Windows.h>
#include <iostream>
#include <fftw3.h>
#include <ctime>
#include "music.h"

using namespace std;

float** Points;

int numPoints, numSegments;

unsigned int LOD = 20;

#define OLD_FILE_PATH "16kkk.pcm"

#define NUM_SHOW_POINTS 64
#define NUM_SHOW_SEGMENTS (NUM_SHOW_POINTS)
#define PI 3.14159

vector<float> vertices;
vector<float> fftvertics;
vector<float> nvertices;

clock_t s = 0, e = 0;

void fft(int _s) {
	fftw_complex* fftIn = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * 640);
	fftw_complex* fftOut = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * 640);
	fftw_plan fftPlan = fftw_plan_dft_1d(640, fftIn, fftOut, FFTW_FORWARD, FFTW_ESTIMATE);

	for (int i = _s; i < 640; i++) {
		fftIn[i][0] = vertices[i];
		fftIn[i][1] = 0;
	}
	fftw_execute(fftPlan);
	
	for (int i = 0; i < 640; i++) {
		fftvertics.push_back(sqrt(fftOut[i][0] * fftOut[i][0] + fftOut[i][1] * fftOut[i][1]));
	}
}

void prepare() {
	short pcmIn = 0;
	int size = 0;
	FILE* fp = fopen(OLD_FILE_PATH, "rb+");

	int i = 0;
	while (!feof(fp)) {
		size = fread(&pcmIn, 2, 1, fp);
		if (size > 0) {
			vertices.push_back(((float)abs(pcmIn)) / 30000);
		}
		i++;
	}

	for (int i = 0; i < vertices.size(); i += 80) {
		float sum = 0;
		for (int j = 0; j < 80 && i + j < vertices.size(); j++) {
			sum += vertices[i + j];
		}
		nvertices.push_back(sum / 80);
	}

	numPoints = i;
	numSegments = numPoints - 3;
	fclose(fp);
	cout << numPoints << endl;
	cout << cos(60) << endl;

	for (int i = 0; i + 640 < vertices.size(); i += 640) {
		fft(i);
	}

	float* pointSpace = new float[3 * numPoints];
	Points = new float* [numPoints];

	i = 0;
	float angle = 0;
	int t = 0;
	for (vector<float>::iterator it = nvertices.begin(); it != nvertices.end(); it++) {
		Points[t++] = &pointSpace[i];
		pointSpace[i++] = (*it) * cos(angle * 10.0/NUM_SHOW_POINTS * PI)*50;
		pointSpace[i++] = (*it) * sin(angle * 10.0/NUM_SHOW_POINTS * PI)*50;
		pointSpace[i++] = 0;

		angle += 1;
		if (angle == NUM_SHOW_POINTS)
			angle = 0;
	}
}

float* GetPoint(int i) {
	if (i < 0) {
		return Points[0];
	}

	if (i < numPoints) {
		return Points[i];
	}

	return Points[numPoints - 1];
}

int stPoint = 0;

void getCurvePoint(float& _x, float& _y, float& _z, int startCv, int i) {
	float t = (float)i / LOD;
	float it = 1.0 - t;
	float b0 = it * it * it / 6.0f;
	float b1 = (3 * t * t * t - 6 * t * t + 4) / 6.0f;
	float b2 = (-3 * t * t * t + 3 * t * t + 3 * t + 1) / 6.0f;
	float b3 = t * t * t / 6.0f;

	float x = b0 * GetPoint(startCv + 0)[0] +
		b1 * GetPoint(startCv + 1)[0] +
		b2 * GetPoint(startCv + 2)[0] +
		b3 * GetPoint(startCv + 3)[0];
	float y = b0 * GetPoint(startCv + 0)[1] +
		b1 * GetPoint(startCv + 1)[1] +
		b2 * GetPoint(startCv + 2)[1] +
		b3 * GetPoint(startCv + 3)[1];
	float z = b0 * GetPoint(startCv + 0)[2] +
		b1 * GetPoint(startCv + 1)[2] +
		b2 * GetPoint(startCv + 2)[2] +
		b3 * GetPoint(startCv + 3)[2];

	_x = x;
	_y = y;
	_z = z;
}

float R = 0;

void onDraw() {
	R += 4.0/nvertices.size();
	glClear(GL_COLOR_BUFFER_BIT);

	glLoadIdentity();
	glPointSize(20);

	glColor3f(0, 1, 0);

	glLineWidth(3);
	glBegin(GL_LINE_STRIP);
	float G = 0;
	float B = 1;

	for (int startCv = stPoint, j = 0; j < NUM_SHOW_SEGMENTS; j++, startCv++) {
		for (int i = 0; i != LOD; i++) {
			glColor3f(R, G, B);
			if (j < 16) {
				B -= 1.0 / (32 * LOD);
				G += 1.0 / (32 * LOD);
			}
			else if (j < 32){
				G -= 1.0 / (32 * LOD);
				B += 1.0 / (32 * LOD);
			}
			else if (j < 48) {
				B -= 1.0 / (32 * LOD);
			}
			else {
				B -= 1.0 / (32 * LOD);
				G += 1.0 / (32 * LOD);
			}

			int tstartCv = startCv;

			if (j == NUM_SHOW_SEGMENTS - 1) {
				tstartCv = stPoint;
			}

			float x, y, z;
			getCurvePoint(x, y, z, tstartCv, i);

			glVertex2f(x, y);
		}
	}

	for (int i = 0; i != LOD; i++) {
		glColor3f(R, G, B);

		int tstartCv = stPoint;

		float x, y, z;
		getCurvePoint(x, y, z, tstartCv, i);

		glVertex2f(x, y);
	}

	glEnd();
	glutSwapBuffers();
	stPoint += NUM_SHOW_POINTS/16;
}

int rep = 0;
void myIdle(void) {
	Sleep(20 - e + s);
	s = clock();
	onDraw();
	e = clock();
}

void onInit() {
	AudioClip ac;
	ac.load("16kkk.mp3");
	ac.play();
}

void onExit() {

}

void onReshape(int w, int h) {
	if (h == 0)
		h = 1;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (w <= h)
		glOrtho(-20.0, 20.0, -20.0 * (GLfloat)h / (GLfloat)w, 20.0 * (GLfloat)h / (GLfloat)w, 0.0, 100.0);
	else
		glOrtho(-20.0, 20.0, -20.0 * (GLfloat)h / (GLfloat)w, 20.0 * (GLfloat)h / (GLfloat)w, 0.0, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	prepare();

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(640, 480);

	glutCreateWindow("Music Visualizer");

	onInit();
	glutDisplayFunc(onDraw);
	glutReshapeFunc(onReshape);
	glutIdleFunc(&myIdle);
	atexit(onExit);


	glutMainLoop();

	return 0;
}