#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/gtc/matrix_transform.hpp>

typedef struct {
	float x;
	float y;
}point;

point vertex[128];
int vertex_idx = 0;
int rightbuttonpressed = 0;
float windowwidth = 0;
float windowheight = 0;
bool polygon_flag = 0;  // 다각형 완성 여부 판단
bool clear_flag = 0;
bool rotate_flag = 0;
float current_x = 0;
float current_y = 0;
float move_x = 0;
float move_y = 0;
float center_x = 0;
float center_y = 0;
void timer(int value);
double angle = 5.0;
const double toRadians = 3.14f / 180.0f;

float r = 250.0f / 255.0f, g = 0.0f, b = 150.0f / 255.0f; // window color=red

void timer(int value) {
	for (int i = 0; i < vertex_idx; i++) {
		float temp_x = vertex[i].x-center_x;
		float temp_y = vertex[i].y-center_y;
		vertex[i].x = ((float)cos(angle * toRadians) * temp_x) - 
			((float)sin(angle * toRadians) * temp_y) + center_x;
		vertex[i].y = ((float)sin(angle * toRadians) * temp_x) + 
			((float)cos(angle * toRadians) * temp_y)+ center_y;

	}
	glutPostRedisplay();
	if (rotate_flag)
		glutTimerFunc(10, timer, 0);
}

void display(void) {
	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);  // color buffer reset
	glColor3f(1.0f, 1.0f, 1.0f);  // white
	glPointSize(5.0f);
	glLineWidth(0.5f);
	glEnableClientState(GL_VERTEX_ARRAY);
	// 점 그리기 //
	if (vertex_idx != 0&& clear_flag != 1) {
		for (int i = 0; i < vertex_idx; i++) {
			glBegin(GL_POINTS);
			glVertex2f(vertex[i].x, vertex[i].y);
		}
		glEnd();
	}
	// 선분 그리기 //
	if (vertex_idx > 1&& clear_flag != 1) {
		for (int i = 0; i < vertex_idx-1; i++) {
			glColor3f(0.0f, 1.0f, 0.0f);  // green
			glBegin(GL_LINES);
			glVertex2f(vertex[i].x, vertex[i].y);
			glVertex2f(vertex[i+1].x, vertex[i+1].y);
		}
		glEnd();
	}
	// 다각형 그리기 //
	if (polygon_flag == 1&&clear_flag!=1) {
		glBegin(GL_LINES);
		glVertex2f(vertex[0].x, vertex[0].y);
		glVertex2f(vertex[vertex_idx-1].x, vertex[vertex_idx-1].y);
		glEnd();
	}
	// 무게 중심 그리기 //
	if (rotate_flag == 1&&clear_flag!=1) {
		glPointSize(7.0f);
		glColor3f(0.0f, 0.0f, 1.0f); // blue
		glBegin(GL_POINTS);
		glVertex2f(center_x, center_y);
		glEnd();
	}
	glEnd();
	glFlush();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'p':
		if (vertex_idx < 3) {
			fprintf(stdout, "$$$ You need at least 3 points to create polygon\n");
			glutPostRedisplay();
		}
		else {
			polygon_flag = 1;
			//fprintf(stdout, "$$$ Polygon created\n");
			display();
		}
		break;
	case 'c':
		if (!rotate_flag) {
			//fprintf(stdout, "$$$ Erase completed\n");
			clear_flag = !clear_flag;
		}
		display();
		break;
	case 'r':
		if (polygon_flag) {
			center_x = center_y = 0;
			for (int i = 0; i < vertex_idx; i++) {
				center_x += vertex[i].x;
				center_y += vertex[i].y;
			}
			center_x /= vertex_idx;
			center_y /= vertex_idx;

			rotate_flag = !rotate_flag;
		}
		if (rotate_flag)
			glutTimerFunc(10, timer, 0);
		display();
		break;
	case 'f':
		glutLeaveMainLoop();
		break;
	}
}

void special(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_LEFT:
		if (polygon_flag) {
			for (int i = 0; i < vertex_idx; i++) {
				//fprintf(stdout, "$$$ move left\n");
				vertex[i].x -= 0.05f;
			}
			display();
		}
		break;
	case GLUT_KEY_RIGHT:
		if (polygon_flag) {
			for (int i = 0; i < vertex_idx; i++) {
				//fprintf(stdout, "$$$ move right\n");
				vertex[i].x += 0.05f;
			}
			display();
		}
		break;
	case GLUT_KEY_DOWN:
		if (polygon_flag) {
			for (int i = 0; i < vertex_idx; i++) {
				//fprintf(stdout, "$$$ move down\n");
				vertex[i].y -= 0.05f;
			}
			display();
		}
		break;
	case GLUT_KEY_UP:
		if (polygon_flag) {
			for (int i = 0; i < vertex_idx; i++) {
				//fprintf(stdout, "$$$ move up\n");
				vertex[i].y += 0.05f;
			}
			display();
		}
		break;
	}
}

void mousepress(int button, int state, int x, int y) {
	int mode = glutGetModifiers();
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN) && (mode == GLUT_ACTIVE_SHIFT)&& polygon_flag == 0) {
		//fprintf(stdout, "*** The left mouse button was pressed at (%d, %d).\n", x, y);
		vertex[vertex_idx].x = (x - (windowwidth / 2)) / (windowwidth / 2);
		vertex[vertex_idx++].y = ((windowheight / 2) - y) / (windowheight / 2);
		display();
	}
	else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN) && polygon_flag == 1) {
		current_x = (x - (windowwidth / 2)) / (windowwidth / 2);
		current_y = ((windowheight / 2) - y) / (windowheight / 2);
		rightbuttonpressed = 1;
	}
	else if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_UP)) {
		rightbuttonpressed = 0;
		for (int i = 0; i < vertex_idx; i++) {
			vertex[i].x += move_x;
			vertex[i].y += move_y;
		}
	}
		
}

void mousemove(int x, int y) {
	if (rightbuttonpressed&&!rotate_flag) {
		//fprintf(stdout, "$$$ The right mouse button is now at (%d, %d).\n", x, y);   // 현재 포인터 위치
		move_x = ((x - (windowwidth / 2)) / (windowwidth / 2)) - current_x;
		move_y = (((windowheight / 2) - y) / (windowheight / 2)) - current_y;

		for (int i = 0; i < vertex_idx; i++) {
			vertex[i].x += move_x;
			vertex[i].y += move_y;
		}
		display();
		for (int i = 0; i < vertex_idx; i++) {
			vertex[i].x -= move_x;
			vertex[i].y -= move_y;
		}
	}
}

void reshape(int width, int height) {
	//fprintf(stdout, "### The new window size is %dx%d.\n", width, height);
	windowwidth = (float)width;
	windowheight = (float)height;
	glViewport(0, 0, width, height);
}

void close(void) {
	fprintf(stdout, "\n^^^ The control is at the close callback function now.\n\n");
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutMouseFunc(mousepress);
	glutMotionFunc(mousemove);
	glutReshapeFunc(reshape);
	glutCloseFunc(close);
}

void initialize_renderer(void) {
	register_callbacks();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = TRUE;
	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void greetings(char* program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 4
int main(int argc, char* argv[]) {
	char program_name[64] = "Sogang CSE4170 SimplefreeGLUTcode";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'p', 'r', 'c', 'f'",
		"    - Special keys used: LEFT, RIGHT, UP, DOWN",
		"    - Mouse used: SHIFT + L-click, R-click and move",
		"    - Other operations: window size change"
	};

	glutInit(&argc, argv);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE); // <-- Be sure to use this profile for this example code!
 //	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_RGBA);

	glutInitWindowSize(500, 500);
	glutInitWindowPosition(500, 200);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	// glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_EXIT); // default
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutMainLoop();
	fprintf(stdout, "^^^ The control is at the end of main function now.\n\n");

	return 0;

}