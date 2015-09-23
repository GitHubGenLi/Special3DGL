#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GL/glut.h>

#include <GL\glext.h>
#include <GL\wglew.h>
#include <gl/gl.h>

#include "textfile.h"
#include "math.h"


GLuint v,f,f2,p;
float lpos[4] = {1,0.5,1,0};
GLint loc;

// angle of the camera
float angleCamera = 0.0f;
// rotation angle
float angleRotate = 0.0f;

//actual vector representing the camera's direction
float lx = 0, lz = -1.0;//-1.0;
//XZ position of the camera
float x = 0.0, z = -6;

//the key states. These variables will be zero when no key is being pressed
float deltaAngle = 0.0f;
float deltaMove = 0.0f;

float totalMoveUpDown = 0.0f;
float totalMoveLeft = 0.0f;
float totalMoveRight = 0.0f;
float angleStep = 0.06;

#define MAX_ITEMS 10000

class Point
{
	public: float x, y, z;
};

Point listPoint[MAX_ITEMS];

void changeSize(int w, int h) {

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;

	float ratio = 1.0* w / h;

	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45,ratio,1,1000);
	glMatrixMode(GL_MODELVIEW);
}

void drawCube() {

	float hd = 1.0;

	glColor3f(1, 0, 0);
	glBegin(GL_QUADS);
	glVertex3f(-hd, -hd, -hd);
	glVertex3f(-hd, hd, -hd);
	glVertex3f(hd, hd, -hd);
	glVertex3f(hd, -hd, -hd);
	glEnd();

	glColor3f(1, 1, 0);
	glBegin(GL_QUADS);
	glVertex3f(-hd, -hd, -hd);
	glVertex3f(hd, -hd, -hd);
	glVertex3f(hd, -hd, hd);
	glVertex3f(-hd, -hd, hd);
	glEnd();

	glColor3f(1, 0, 1);
	glBegin(GL_QUADS);
	glVertex3f(-hd, -hd, -hd);
	glVertex3f(-hd, -hd, hd);
	glVertex3f(-hd, hd, hd);
	glVertex3f(-hd, hd, -hd);
	glEnd();

	glColor3f(0, 1, 0);
	glBegin(GL_QUADS);
	glVertex3f(-hd, -hd, hd);
	glVertex3f(hd, -hd, hd);
	glVertex3f(hd, hd, hd);
	glVertex3f(-hd, hd, hd);
	glEnd();

	glColor3f(0, 0, 1);
	glBegin(GL_QUADS);
	glVertex3f(-hd, hd, -hd);
	glVertex3f(-hd, hd, hd);
	glVertex3f(hd, hd, hd);
	glVertex3f(hd, hd, -hd);
	glEnd();

	glColor3f(0, 1, 1);
	glBegin(GL_QUADS);
	glVertex3f(hd, -hd, -hd);
	glVertex3f(hd, hd, -hd);
	glVertex3f(hd, hd, hd);
	glVertex3f(hd, -hd, hd);
	glEnd();

}

void computePos(float deltaMove)
{
	x += deltaMove * lx * 0.1f;
	z += deltaMove * lz * 0.1f;
}
void computeDir(float deltaAngle)
{
	angleCamera += deltaAngle;
	lx = sin(angleCamera);
	lz = -cos(angleCamera);
}

void pressSpecialKey(int key, int xx, int yy)
{
	float fraction = 0.1f;

	switch (key)
	{
		//spin	
	//case GLUT_KEY_HOME:
			//angleCamera -= 0.01f;
			//lx = sin(angleCamera);
			//lz = -cos(angleCamera);
			
		//	break;
	//case GLUT_KEY_END:
			//angleCamera += 0.01f;
			//lx = sin(angleCamera);
			//lz = -cos(angleCamera);

			//break;
	
		//camera
		case GLUT_KEY_PAGE_UP:
			angleCamera -= 0.01f;
			lx = sin(angleCamera);
			lz = -cos(angleCamera);

			x += lx * fraction;
			z += lz * fraction;
			
			break;
		case GLUT_KEY_PAGE_DOWN:
			angleCamera += 0.01f;
			lx = sin(angleCamera);
			lz = -cos(angleCamera);

			x -= lx * fraction;
			z -= lz * fraction;
			break;
		
			//translate
		case GLUT_KEY_LEFT: 
			totalMoveLeft -= 0.05;
			break;
		case GLUT_KEY_RIGHT: 
			totalMoveLeft += 0.05;
			break;
		case GLUT_KEY_UP: deltaMove = 0.05f;
			totalMoveUpDown += deltaMove;
			break;
		case GLUT_KEY_DOWN: deltaMove = -0.05f;
			totalMoveUpDown += deltaMove;
			break;
	}
}
void processNormalKeys(unsigned char key, int x, int y) {

	if (key == 27)
		exit(0);
	else if (key == '*')
	{
		angleStep *= -1.0;
	}
	else if (key == '+')
	{
		angleStep += 0.01;
	}
	else if (key == '-')
	{
		angleStep -= 0.01;
	}
	else if (key == ' ')
	{
		lx = 0;
		lz = -1.0;
		x = 0.0;
		z = -6;
		deltaAngle = 0.0f;
		deltaMove = 0.0f;
		totalMoveUpDown = 0.0f;
		totalMoveLeft = 0.0f;
		totalMoveRight = 0.0f;
		angleStep = 0.06;
	}
}

/*
void releaseKey(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_LEFT:
	case GLUT_KEY_RIGHT: deltaAngle = 0.0f; break;
	case GLUT_KEY_UP:
	case GLUT_KEY_DOWN: deltaMove = 0; break;
	}
}
*/

void renderScene(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	/*if (deltaMove)
		computePos(deltaMove);

	if (deltaAngle)
		computeDir(deltaAngle);*/

	//set the camera
	/*gluLookAt(x, 10.0f, z,
		x + lx, 0.0f, z + lz,
		0.0f, 1.0f, 0.0f);
		*/
	/*gluLookAt(0.0, 5.0, -10,
			0.0, 0.0, -1.0,
			0.0f, 1.0f, 0.0f);*/

	/*gluLookAt(0.0, 5.0, -2.0, 
		0.0, 0.0, 0.0, 
		0.0, 1.0, 0.0);
	*/

	gluLookAt(x + lx, 5.0f, z + lz,
		0, 0.0f, 0,
		0.0f, 1.0f, 0.0f);

	glLightfv(GL_LIGHT0, GL_POSITION, lpos);

	glTranslatef(totalMoveLeft, totalMoveRight, totalMoveUpDown);

	glRotatef(angleRotate, 0, 1, 0);

	drawCube();

	//glPointSize(3.0);
	//glColor3f(1, 0, 1);
	//glBegin(GL_POINTS); //starts drawing of points
	//glVertex3f(1.0f, 1.0f, 0.0f);//upper-right corner
	//glVertex3f(-1.0f, -1.0f, 0.0f);//lower-left corner
	//glVertex3f(0.0f, 0.0f, 0.0f);
	//glEnd();//end drawing of points

	angleRotate += angleStep;

	glutSwapBuffers();
}

#define printOpenGLError() printOglError(__FILE__, __LINE__)

int printOglError(char *file, int line)
{
    //
    // Returns 1 if an OpenGL error occurred, 0 otherwise.
    //
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
        retCode = 1;
        glErr = glGetError();
    }
    return retCode;
}

void printShaderInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
        free(infoLog);
    }
}

void printProgramInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
        free(infoLog);
    }
}

void setShaders() {

	char *vs = NULL,*fs = NULL,*fs2 = NULL;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);
	f2 = glCreateShader(GL_FRAGMENT_SHADER);
	
	vs = textFileRead("height.vert");
	fs = textFileRead("height.frag");

	const char * vv = vs;
	const char * ff = fs;

	glShaderSource(v, 1, &vv,NULL);
	glShaderSource(f, 1, &ff,NULL);

	free(vs);
	free(fs);

	glCompileShader(v);
	glCompileShader(f);

	printShaderInfoLog(v);
	printShaderInfoLog(f);
	printShaderInfoLog(f2);

	p = glCreateProgram();
	glAttachShader(p,v);
	glAttachShader(p,f);

	glLinkProgram(p);
	printProgramInfoLog(p);

	glUseProgram(p);
	
	loc = glGetAttribLocation(p,"height");
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(640, 480);
	int openW = glutCreateWindow("My OpenGL");
	
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glOrtho(-10, 10, -10, 10, -10, 10);
	//gluPerspective((float)30, 640.0f / 480.0f, 0.001f, -100);
	//glMatrixMode(GL_MODELVIEW);

	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(pressSpecialKey);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0, 0.0, 0.0, 0.0);

	//glEnable(GL_CULL_FACE);

	glewInit();

	if (glewIsSupported("GL_VERSION_2_0"))
		printf("Ready for OpenGL 2.0\n");
	else {
		printf("OpenGL 2.0 not supported\n");
		exit(1);
	}

	//setShaders();

	/*
	PIXELFORMATDESCRIPTOR pfd;
	int   pf;
	HDC   hDC;
	HGLRC hRC;
	HWND  hWnd;

	//hWnd = CreateWindow(...);

	//hDC = GetDC(hWnd);

	hDC = GetDC((HWND)openW);

	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;

	// Note, for ease of code reading no error checking is done.
	pf = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, pf, &pfd);
	DescribePixelFormat(hDC, pf, sizeof(PIXELFORMATDESCRIPTOR),
		&pfd);

	hRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hRC);
	*/
	
//#define MAX_GPU 2
//
//	PIXELFORMATDESCRIPTOR pfd;
//	int    pf, gpuIndex = 0;
//	HGPUNV hGPU[MAX_GPU];
//	HGPUNV GpuMask[MAX_GPU];
//	HDC    affDC;
//	HGLRC  affRC;
//
//	// Get a list of the first MAX_GPU GPUs in the system
//	while ((gpuIndex < MAX_GPU) && wglEnumGpusNV(gpuIndex,
//		&hGPU[gpuIndex])) {
//		gpuIndex++;
//	}
//
//
//	// Create an affinity-DC associated with the second/first GPU
//	GpuMask[0] = NULL; //hGPU[0]
//	GpuMask[1] = hGPU[1]; //NULL
//
//	affDC = wglCreateAffinityDCNV(GpuMask);
//	//affDC = wglCreateAffinityDCNV(hGPU);
//
//	// Set a pixelformat on the affinity-DC
//	pf = ChoosePixelFormat(affDC, &pfd);
//	SetPixelFormat(affDC, pf, &pfd);
//	DescribePixelFormat(affDC, pf, sizeof(PIXELFORMATDESCRIPTOR),
//		&pfd);
//
//	affRC = wglCreateContext(affDC);
//	wglMakeCurrent(affDC, affRC);
//
//	// Make a previously created FBO current so we have something
//	// to render into. Since there's no window, the default system
//	// created FBO is incomplete.
//	//glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fb);

	

	glutMainLoop();

	return 0;
}

