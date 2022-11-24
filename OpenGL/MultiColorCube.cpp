#include <stdio.h>
#include <GL/freeglut.h>


class ParentObject
{
	public:
		void init()
		{
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
		}
		void translate(float x = 0.0f, float y = 0.0f, float z = 0.0f)
		{
			glTranslatef(x,y,z);
		}
		void scale(float x = 1.0f, float y = 1.0f, float z = 1.0f)
		{
			glScalef(x,y,z);
		}
		void rotate(float angle = 0.0f, float x = 0.0f, float y = 0.0f, float z = 0.0f)
		{
			glRotatef(angle, x,y,z);
		}

};

class Triangle : public ParentObject
{
	public:
		void Draw(float tx = 0.0f,float ty = 0.0f,float tz = 0.0f,float sx = 1.0f,float sy = 1.0f,float sz = 1.0f,float angle = 0.0f,float ax = 0.0f,float ay = 0.0f,float az = 0.0f)
		{
			init();
			translate(tx,ty,tz);
			scale(sx,sy,sz);
			rotate(angle,ax,ay,az);

			glBegin(GL_TRIANGLES);
				glColor3f(1.0f, 0.0f, 0.0f);
				glVertex3f(0.0f, 0.5f, 0.0f);

				glColor3f(0.0f, 1.0f, 0.0f);
				glVertex3f(-0.5f, -0.5f, 0.0f);

				glColor3f(0.0f, 0.0f, 1.0f);
				glVertex3f(0.5f, -0.5f, 0.0f);
			glEnd();
		}
}t;

class Cube : public ParentObject
{
	public:
		void Draw(float tx = 0.0f,float ty = 0.0f,float tz = 0.0f,float sx = 1.0f,float sy = 1.0f,float sz = 1.0f,float angle = 0.0f,float ax = 0.0f,float ay = 0.0f,float az = 0.0f)
		{
			init();
			translate(tx,ty,tz);
			scale(sx,sy,sz);
			rotate(angle,ax,ay,az);

			glBegin(GL_QUADS);
				/****** Front Square ******/
				glColor3f(1.0f, 0.0f, 0.0f);
				glVertex3f(1.0f, 1.0f, 1.0f); /* Right Top */
				glVertex3f(-1.0f, 1.0f, 1.0f); /* Left Top */
				glVertex3f(-1.0f, -1.0f, 1.0f); /* Left Bottom */
				glVertex3f(1.0f, -1.0f, 1.0f); /* Right Bottom */
				
				/****** Right Square ******/
				glColor3f(0.0f, 1.0f, 0.0f);
				glVertex3f(1.0f, 1.0f, -1.0f); /* Right Top */
				glVertex3f(1.0f, 1.0f, 1.0f); /* Left Top */
				glVertex3f(1.0f, -1.0f, 1.0f); /* Left Bottom */
				glVertex3f(1.0f, -1.0f, -1.0f); /* Right Bottom */
				
				/****** Back Square ******/
				glColor3f(0.0f, 0.0f, 1.0f);
				glVertex3f(-1.0f, 1.0f, -1.0f); /* Right Top */
				glVertex3f(1.0f, 1.0f, -1.0f); /* Left Top */
				glVertex3f(1.0f, -1.0f, -1.0f); /* Left Bottom */
				glVertex3f(-1.0f, -1.0f, -1.0f); /* Right Bottom */
				
				/****** Left Square ******/
				glColor3f(0.0f, 1.0f, 1.0f);
				glVertex3f(-1.0f, 1.0f, 1.0f); /* Right Top */
				glVertex3f(-1.0f, 1.0f, -1.0f); /* Left Top */
				glVertex3f(-1.0f, -1.0f, -1.0f); /* Left Bottom */
				glVertex3f(-1.0f, -1.0f, 1.0f); /* Right Bottom */
				
				/****** Top Square ******/
				glColor3f(1.0f, 0.0f, 1.0f);
				glVertex3f(1.0f, 1.0f, 1.0f); /* Right Top */
				glVertex3f(-1.0f, 1.0f, 1.0f); /* Left Top */
				glVertex3f(-1.0f, 1.0f, -1.0f); /* Left Bottom */
				glVertex3f(1.0f, 1.0f, -1.0f); /* Right Bottom */
				
				/****** Bottom Square ******/
				glColor3f(1.0f, 1.0f, 0.0f);
				glVertex3f(1.0f, -1.0f, 1.0f); /* Right Top */
				glVertex3f(-1.0f, -1.0f, 1.0f); /* Left Top */
				glVertex3f(-1.0f, -1.0f, -1.0f); /* Left Bottom */
				glVertex3f(1.0f, -1.0f, -1.0f); /* Right Bottom */
			glEnd();
		}
}c;

/* global declarations */
//Function Declarations
void initialize(void);
void resize(int, int);
void display(void);
void keyboard(unsigned char, int, int);
void mouse(int, int, int, int);
void uninitialize(void);

/* global declarations */
bool bFullScreen = false;	// for full screen

int main(int argc, char** argv)
{
	//code
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Cube");

	initialize();

	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutCloseFunc(uninitialize);

	glutIdleFunc(display);	// for animation

	glutMainLoop();

	return(0);
}

/* function definitions */
void initialize(void)
{
	//code

	//Depth Settings
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void resize(int width, int height)
{
	//code
	if (height <= 0)
		height = 1;

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f,
					(GLfloat)width / (GLfloat)height,
					0.1f,
					100.0f);


}

void display(void)
{
	//code
	static float glfAngle = 0.0f;

	glfAngle += 1.0;
	if (glfAngle >= 360.0f)
	{
		glfAngle = 0.0f;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	t.Draw(-2.0f, 0.0f, -6.0f,0.75f, 0.75f, 0.75f);
	c.Draw(2.0f, 0.0f, -6.0f,1.0f, 1.0f, 1.0f,glfAngle, 1.0f, 1.0f, 1.0f);
	
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x_position, int y_position)
{
	//code
	switch (key)
	{
	case 27:
		glutLeaveMainLoop();
		break;

	case 'F':
	case 'f':
		if (bFullScreen == false)
		{
			glutFullScreen();
			bFullScreen = true;
		}
		else
		{
			glutLeaveFullScreen();
			bFullScreen = false;
		}
		break;

	default:
		break;
	}
}

void mouse(int button, int state, int x_position, int y_position)
{
	//code
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		break;

	case GLUT_RIGHT_BUTTON:
		break;

	default:
		break;
	}
}	

void uninitialize(void)
{
	//code
}	
