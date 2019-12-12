from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *
import  random
import math

from threading import Thread
import pyaudio, wave
import numpy as np
from scipy.fftpack import fft
import  time
import argparse

test_dt = [0]
files_seconds = 1
LOD = 15
Points = []
NUM_SHOW_POINS = 64
PI = math.pi
R = 0.

class Music(Thread):
    def __init__(self,filepath):
        self.filepath=filepath
        Thread.__init__(self)
        self.daemon = True
        self.start()

    def run(self):
        global files_seconds
        CHUNK = 1024 * 4
        FORMAT = pyaudio.paInt16
        CHANNELS = 1
        RATE = 44100
        wf = wave.open(self.filepath, 'rb')
        p = pyaudio.PyAudio()
        stream = p.open(format=p.get_format_from_width(wf.getsampwidth()),
                        channels=wf.getnchannels(), rate=wf.getframerate(), input=True,
                        output=True, frames_per_buffer=1024)
        files_seconds = wf.getnframes() / RATE
        print(wf.getnchannels())
        x = np.arange(0, 16 * CHUNK, 8)
        x_fft = np.linspace(0, RATE, CHUNK)
        frame_count = 0
        start_time = time.time()
        global Frequencies, test_dt
        while True:
            data = wf.readframes(CHUNK)
            data_int = np.fromstring(data, dtype=np.int16)
            test_dt = data_int
            preparePoints()
            try:
                stream.write(data)
                frame_count = frame_count + 1
            except:
                frame_rate = frame_count / (time.time() - start_time)
                print("stream stopped")
                print('average frame rate = {:.0f} FPS'.format(frame_rate))
                break


def preparePoints():
    global Points
    angle = 0
    tmpPt = []
    tmpDt = []
    for i in range(0, len(test_dt), 128):
        sum = 0
        for j in range(64):
            if j + i >= len(test_dt):
                break
            sum += abs(test_dt[j + i]) / 30000
        sum /= 128.
        tmpDt.append(sum)

    for i in range(len(tmpDt)):
        x = tmpDt[i] * math.cos(angle * 8. / NUM_SHOW_POINS * PI) * 2
        y = tmpDt[i] * math.sin(angle * 8. / NUM_SHOW_POINS * PI) * 2
        z = 0
        tmpPt.append((x, y, z))
        angle += 1
        if angle == NUM_SHOW_POINS:
            angle = 0

    Points = tmpPt


def initGL():
    glClearColor(1.0,1.0,1.0,1.0)
    glMatrixMode(GL_PROJECTION)
    gluOrtho2D(-250,250,-250,250)


def getPoint(startCv):
    if len(Points):
        startCv = startCv % len(Points)
        return Points[startCv]
    return 0,0,0


def getCurvePoint(startCv, _i):
    t = _i / LOD
    it = 1.0 - t
    b0 = it * it * it / 6.0
    b1 = (3 * t * t * t - 6 * t * t + 4) / 6.0
    b2 = (-3 * t * t * t + 3 * t * t + 3 * t + 1) / 6.0
    b3 = t * t * t / 6.0

    pt = []
    for i in range(4):
        pt.append(getPoint(startCv + i))

    x = b0 * pt[0][0] + \
        b1 * pt[1][0] + \
        b2 * pt[2][0] + \
        b3 * pt[3][0]
    y = b0 * pt[0][1] + \
        b1 * pt[1][1] + \
        b2 * pt[2][1] + \
        b3 * pt[3][1]
    z = b0 * pt[0][2] + \
        b1 * pt[1][2] + \
        b2 * pt[2][2] + \
        b3 * pt[3][2]

    return x, y, z


def display():
    global Frequencies, R, files_seconds

    for t in range(0, 64, 1):
        glClear(GL_COLOR_BUFFER_BIT)
        glLineWidth(3)
        glBegin(GL_LINE_STRIP)
        G = 0.
        B = 1.
        if R < 1:
            R += 4. / (10000)
        else:
            R -= 4. / (10000)
        for i in range(NUM_SHOW_POINS):
            for j in range(LOD):
                tstartCv = i + t
                glColor((R, G, B))
                fac = 32 * LOD
                if i < 16:
                    B -= (1. / fac)
                    G += (1. / fac)
                elif i < 32:
                    G -= (1. / fac)
                    B += (1. / fac)
                elif i < 48:
                    B -= (1. / fac)
                else:
                    B -= (1. / fac)
                    G += (1. / fac)
                if i == NUM_SHOW_POINS - 1:
                    tstartCv = t
                x, y, _ = getCurvePoint(tstartCv, j)
                glVertex2f(x, y)
        glEnd()
        glFlush()


def mouseClick(button,state,x,y):
    return


def mouseMotion(x,y):
    return

if __name__ == '__main__':
    
    parser = argparse.ArgumentParser(description='')
    parser.add_argument('fname', type=str, default=None)
    args = parser.parse_args()
    fname = args.fname

    glutInit()
    glutInitWindowSize(750,750)
    glutCreateWindow(b"Music Visualizer")
    glutDisplayFunc(display)
    glutIdleFunc(display)
    glutMouseFunc(mouseClick)
    glutMotionFunc(mouseMotion)
    Music(fname)
    glutMainLoop()
