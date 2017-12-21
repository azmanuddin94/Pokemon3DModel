//Pokemon 3D project
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <GL/glut.h>

// ---------------------------------------------struct of the camera-------
struct Camera
{
	static const int max_fov = 50;

	float field_of_view; //Field of view angle, in degrees. This is changed when zooming in/out.
	float rotate[2]; //Yaw and Pitch angles to orbit around the point of interest.
	int mouse_button[2];
	int mouse_xy[2];

	Camera()
	{
		field_of_view = 30;
		rotate[0] = 1;
		rotate[1] = -100;
		mouse_button[0] = 0;
		mouse_button[1] = 0;
		mouse_xy[0] = 0;
		mouse_xy[1] = 0;
	}

	//--------------------------------------------setup projection---------
	void projection_setup()
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		float aspect = (float)glutGet(GLUT_WINDOW_WIDTH)/glutGet(GLUT_WINDOW_HEIGHT);
		gluPerspective(field_of_view, aspect, 0.1f, 100);
	}

	//----------------------------------------------setup model view----------
	void model_view()
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0,0,-10);
		glRotatef(rotate[0],1,0,0);
		glRotatef(rotate[1],0,1,0);
	}

	//------------------------------------------------------mouse click---------------
	void mouse_click(int button, int state, int x, int y)
	{
		if(button==3 || button==4) //--------------------------mouse wheel------------------
		{
			if(state == GLUT_DOWN)
			{
				const float ZoomFactor = 0.9f;
				field_of_view *= (button==3) ? ZoomFactor : 1.f/ZoomFactor; //----------------------------------camera zoom-------------------

				if(field_of_view > max_fov) field_of_view = max_fov;
			}
		}

		if(button == GLUT_LEFT_BUTTON || button == GLUT_RIGHT_BUTTON)
		{
			if(button == GLUT_LEFT_BUTTON)
				mouse_button[0] = (state == GLUT_DOWN);

			if(button == GLUT_RIGHT_BUTTON)
				mouse_button[1] = (state == GLUT_DOWN);

			mouse_xy[0] = x;
			mouse_xy[1] = y;
		}
	}

	//-----------------------------------------------mouse drag----------------
	void mouse_drag(int x, int y)
	{
		int dx = x - mouse_xy[0];
		int dy = y - mouse_xy[1];

		if(dx || dy)
		{
			if(mouse_button[0])  //Orbit camera with LMB.
			{
				rotate[1] += dx * 0.2f;
				rotate[0] += dy * 0.15f;
			}

			if(mouse_button[1]) //Zoom camera with RMB.
			{
				int H = glutGet(GLUT_WINDOW_HEIGHT);
				field_of_view *= 1.f + 2.0f * dy / H;
				if(field_of_view > max_fov) field_of_view = max_fov;
			}
			mouse_xy[0] = x;
			mouse_xy[1] = y;
		}
	}
};


// ----------------------------------------------------------------struct of the pokemon-------
struct Pokemon
{
	GLUquadricObj* quadric;//Helper object to draw cylinders and spheres.
	float position[3]; //pokemon position.
	float direct_angle; //Movement direction angle.
	int forward, back, left, right; //User input commands. 1 if corresponding key is pressed, 0 otherwise.

	Pokemon()
	{
		quadric = 0;
		position[0] = 0;
		position[1] = 0;
		position[2] = 0;
		direct_angle = 0;
		back = 0;
		left = 0;
		right = 0;
	}

	//---------------------------------------------------animate-----------------
	void animate()
	{
		float time_1 = glutGet(GLUT_ELAPSED_TIME) / 1000.f; // --------------speed of camera move-------------
		static float time_2 = time_1;
		float delta_phase = (time_1-time_2) * 10; // ----------speed of frame animated----------

		//Determine the nature of movement: walking and/or turning
		int turn = right - left;

		if(turn)
		{
			// Turning left/right.

			direct_angle -= turn * delta_phase * 10; // -----------------------speed of frame animated-----------
		}

		time_2 = time_1;//Remember the current time to calculate elapsed time on the next frame.
	}


	//--------------------------------------draw body----------------
	void draw_body()
	{
		const float body_color[3] = {0.3f, 0.3f, 1.0f};
		const float belly_color[3] = {1.0f, 1.0f, 1.0f};
		const float head_color[3] = {0.3f, 0.3f, 1.0f};
		const float leye_color[3] = {0.0f, 0.0f, 0.0f};
		const float reye_color[3] = {0.0f, 0.0f, 0.0f};
		const float mleye_color[3] = {1.0f, 1.0f, 1.0f};
        const float mreye_color[3] = {1.0f, 1.0f, 1.0f};
        const float lnose_color[3] = {0.0f, 0.0f, 0.0f};
        const float rnose_color[3] = {0.0f, 0.0f, 0.0f};
        const float rball_color[3] = {1.0f, 0.0f, 0.0f};
        const float wball_color[3] = {1.0f, 1.0f, 1.0f};

		// GLU primitive drawing functions require a quadric object as a parameter.
		// It can be created once and reused many times.
		if(! quadric)
		{
			quadric = gluNewQuadric();
			gluQuadricDrawStyle(quadric, GLU_FILL);
		}

		// Make material shiny
		GLfloat material_spec[] = {0.5f, 0.5f, 0.5f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_spec);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 90);

		// -----------------------------model origin-----------
		glTranslatef(position[0], position[1] + 0.4f, position[2]);
		glRotatef(direct_angle, 0,1,0);

		// ------------------------------------head-------------
		glPushMatrix();
			glTranslatef(0.1f,0.2f,0);
			glScalef(0.26f,0.2f,0.26f); // size of head
			glColor3fv(head_color);
			glutSolidSphere(1, 50, 50);	//surface of sphere
		glPopMatrix();
        // ------------------------------------eye-------------
		glPushMatrix();
			glTranslatef(0.25,0.4f,0.16);//left eye
			glScalef(0.07f,0.07f,0.07f); // size of eye
			glColor3fv(mleye_color);
			glutSolidSphere(1, 50, 50);	//surface of sphere
		glPopMatrix();

		glPushMatrix();
			glTranslatef(0.23,0.43f,0.13);//left eye lid
			glScalef(0.1f,0.1f,0.1f); // size of eye
			glColor3fv(head_color);
			glutSolidSphere(1, 50, 50);	//surface of sphere
		glPopMatrix();

		glPushMatrix();
			glTranslatef(0.31,0.4f,0.14f);//eyeball
			glScalef(0.03f,0.03f,0.03f); // size of eye
			glColor3fv(leye_color);
			glutSolidSphere(1, 50, 50);	//surface of sphere
		glPopMatrix();

        glPushMatrix();
			glTranslatef(0.25,0.4f,-0.16f);//right eye
			glScalef(0.07f,0.07f,0.07f); // size of eye
			glColor3fv(mreye_color);
			glutSolidSphere(1, 50, 50);	//surface of sphere
		glPopMatrix();

		glPushMatrix();
			glTranslatef(0.23,0.43f,-0.13);//right eye lid
			glScalef(0.1f,0.1f,0.1f); // size of eye
			glColor3fv(head_color);
			glutSolidSphere(1, 50, 50);	//surface of sphere
		glPopMatrix();

		glPushMatrix();
			glTranslatef(0.31,0.4f,-0.14f);//eyeball
			glScalef(0.03f,0.03f,0.03f); // size of eye
			glColor3fv(reye_color);
			glutSolidSphere(1, 50, 50);	//surface of sphere
		glPopMatrix();
        // ----------------------nose-------------
        glPushMatrix();
			glTranslatef(0.35,0.25f,0.03f);//nose
			glRotatef (-30,5.0,0.0,-5.0);
			glScalef(0.01f,0.03f,0.01f); // size of eye
			glColor3fv(lnose_color);
			glutSolidSphere(1, 50, 50);	//surface of sphere
		glPopMatrix();

        glPushMatrix();
			glTranslatef(0.35,0.25f,-0.03f);//nose
			glRotatef (30,5.0,0.0,5.0);
			glScalef(0.01f,0.03f,0.01f); // size of eye
			glColor3fv(rnose_color);
			glutSolidSphere(1, 50, 50);	//surface of sphere
		glPopMatrix();

		// --------------------body-------------
		glPushMatrix();//limb
			glTranslatef(0,0,0);
			glRotatef(10,0,0,1);
			glScalef(0.4f,0.4f,0.4f);
			glColor3fv(body_color);
			glutSolidSphere(1, 30, 30);
		glPopMatrix();

		glPushMatrix();//belly
			glTranslatef(0.08,-0.009,0);
			glRotatef(10,0,0,1);
			glScalef(0.35f,0.35f,0.35f);
			glColor3fv(belly_color);
			glutSolidSphere(1, 30, 30);
		glPopMatrix();

        // -------------------------arm-------------

        glPushMatrix();//left shoulder
			glTranslatef(0.0,0.15,0.28);
			glRotatef(10,0,0,1);
			glScalef(0.15f,0.15f,0.15f);
			glColor3fv(body_color);
			glutSolidSphere(1,50,50);
		glPopMatrix();

		glPushMatrix();//left bicep
			glTranslatef(0.0,0.08,0.40);
			glRotatef(60,5.0,0.0,-1);
			glScalef(0.095f,0.095f,0.25f);
			glColor3fv(body_color);
			glutSolidSphere(1,50,50);
		glPopMatrix();

		glPushMatrix();//left elbow
			glTranslatef(0.0,-0.05,0.5);
			glRotatef(60,5.0,0.0,-1);
			glScalef(0.09f,0.09f,0.09f);
			glColor3fv(body_color);
			glutSolidSphere(1,50,50);
		glPopMatrix();

		glPushMatrix();//left arm
			glTranslatef(0.15,-0.08,0.6);
			glRotatef(60,0.0,1.0,0.0);
			glScalef(0.095f,0.095f,0.25f);
			glColor3fv(body_color);
			glutSolidSphere(1,50,50);
		glPopMatrix();

		glPushMatrix();//left hand
			glTranslatef(0.3,-0.09,0.7);
			glRotatef(60,0.0,1.0,0.0);
			glScalef(0.12f,0.12f,0.12f);
			glColor3fv(belly_color);
			glutSolidSphere(1,50,50);
		glPopMatrix();

		glPushMatrix();//right shoulder
			glTranslatef(0.0,0.15,-0.28);
			glRotatef(10,0,0,1);
			glScalef(0.15f,0.15f,0.15f);
			glColor3fv(body_color);
			glutSolidSphere(1,50,50);
		glPopMatrix();

		glPushMatrix();//right bicep
			glTranslatef(0.0,0.08,-0.40);
			glRotatef(60,-5.0,0.0,1);
			glScalef(0.095f,0.095f,0.25f);
			glColor3fv(body_color);
			glutSolidSphere(1,50,50);
		glPopMatrix();

		glPushMatrix();//right elbow
			glTranslatef(0.0,-0.05,-0.5);
			glRotatef(60,5.0,0.0,-1);
			glScalef(0.09f,0.09f,0.09f);
			glColor3fv(body_color);
			glutSolidSphere(1,50,50);
		glPopMatrix();

		glPushMatrix();//right arm
			glTranslatef(0.15,-0.08,-0.6);
			glRotatef(-60,0.0,1.0,0.0);
			glScalef(0.095f,0.095f,0.25f);
			glColor3fv(body_color);
			glutSolidSphere(1,50,50);
		glPopMatrix();

		glPushMatrix();//right hand
			glTranslatef(0.3,-0.09,-0.7);
			glRotatef(60,0.0,1.0,0.0);
			glScalef(0.12f,0.12f,0.12f);
			glColor3fv(belly_color);
			glutSolidSphere(1,50,50);
		glPopMatrix();

        // -------------------------legs-------------
        glPushMatrix();//left leg
			glTranslatef(0.0,-0.35,0.15);
			glRotatef(-10,1.0,0.0,0.0);
			glScalef(0.08f,0.2f,0.08f);
			glColor3fv(body_color);
			glutSolidSphere(1,50,50);
		glPopMatrix();

		glPushMatrix();//left foot
			glTranslatef(0.0,-0.5,0.18);
			glRotatef(-10,1.0,0.0,0.0);
			glScalef(0.08f,0.08f,0.08f);
			glColor3fv(body_color);
			glutSolidSphere(1,50,50);
		glPopMatrix();

        glPushMatrix();//right leg
			glTranslatef(0.0,-0.35,-0.15);
			glRotatef(10,1.0,0.0,0.0);
			glScalef(0.08f,0.2f,0.08f);
			glColor3fv(body_color);
			glutSolidSphere(1,50,50);
		glPopMatrix();

		glPushMatrix();//left foot
			glTranslatef(0.0,-0.5,-0.18);
			glRotatef(-10,1.0,0.0,0.0);
			glScalef(0.08f,0.08f,0.08f);
			glColor3fv(body_color);
			glutSolidSphere(1,50,50);
		glPopMatrix();

		// -------------------------pokeball-------------
        glPushMatrix();//redhalf
			glTranslatef(1.0,1.0,0.0);
			glRotatef(-10,1.0,0.0,0.0);
			glScalef(0.3f,0.3f,0.3f);
			glColor3fv(rball_color);
			glutSolidSphere(1,50,50);
		glPopMatrix();

		 glPushMatrix();//whitehalf
			glTranslatef(1.0,1.0,0.04);
			glRotatef(-10,1.0,0.0,0.0);
			glScalef(0.3f,0.3f,0.3f);
			glColor3fv(wball_color);
			glutSolidSphere(1,50,50);
		glPopMatrix();

		glPushMatrix();//black ring
			glTranslatef(1.0,1.0,0.0);
			glRotatef(-0.5,1.0,0.0,0.0);
			glScalef(0.31f,0.31f,0.08f);
			glColor3fv(rnose_color);
			glutSolidSphere(1,50,50);
		glPopMatrix();

		glPushMatrix();//pokeball push button ring
			glTranslatef(1.0,1.22,0.0);
			glRotatef(-0.5,1.0,0.0,0.0);
			glScalef(0.1f,0.1f,0.1f);
			glColor3fv(rnose_color);
			glutSolidSphere(1,50,50);
		glPopMatrix();

		glPushMatrix();//pokeball push button
			glTranslatef(1.0,1.29,0.0);
			glRotatef(-0.5,1.0,0.0,0.0);
			glScalef(0.04f,0.04f,0.04f);
			glColor3fv(belly_color);
			glutSolidSphere(1,50,50);
		glPopMatrix();


	}
};


Camera camera;
Pokemon pokemon;

//--------------------------light setup------------
void light_setup()
{
	// Setup Phong illumination model parameters.
	// Use 2 lights at opposing positions to illuminate the model from both sides.
	GLfloat light_position_1[] = { 1.0f,  1.0f,  1.0f, 0.0f}; // -----------------------------top left of user view-------
	GLfloat light_position_2[] = {-1.0f, -1.0f, -1.0f, 0.0f}; // -----------------------------bottom right of user view------
	GLfloat light_colour_1[] = {1.0f, 1.0f, 1.0f, 1.0f}; // ----------------------------------white light---------------
	GLfloat light_colour_2[] = {0.5f, 0.5f, 0.5f, 1.0f}; // ----------------------------------a bit dark light ---------
	GLfloat light_ambient[] = {0.1f, 0.1f, 0.1f, 1.0f}; // -----------------------------------dark light----------------

	glLightfv(GL_LIGHT0, GL_POSITION, light_position_1);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position_2);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_colour_1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_colour_2);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_colour_1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_colour_2);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_ambient);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
}

//-------------------------------------------------------------------------------------------------------------------display setup-----------
void display(void)
{
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,glutGet(GLUT_WINDOW_WIDTH),glutGet(GLUT_WINDOW_HEIGHT));
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	camera.projection_setup();
	camera.model_view();
	light_setup();
	glEnable(GL_LIGHTING);

	//Automatically normalize vertex normal vectors after scaling primitives.
	//This is essential for proper ligting computation.
	glEnable(GL_NORMALIZE);

	glEnable(GL_COLOR_MATERIAL);

	//Center view at the pokemon.
	glTranslatef(-pokemon.position[0], -pokemon.position[1]-0.5f, -pokemon.position[2]);

	glEnable(GL_TEXTURE_2D);

	glDisable(GL_TEXTURE_2D);
	pokemon.animate();
	pokemon.draw_body();
	glutSwapBuffers();
	glutPostRedisplay(); // Immediately request the next frame for continuous animation.
}

//-------------------------------------------------------------------------------------------------------------------key press---------------
void key_press(int key, int x, int y)
{
	if(key == GLUT_KEY_UP)
		pokemon.forward = 1;

	if(key == GLUT_KEY_DOWN)
		pokemon.back = 1;

	if(key == GLUT_KEY_LEFT)
		pokemon.left = 1;

	if(key == GLUT_KEY_RIGHT)
		pokemon.right = 1;
}

// -----------------------------------------------------------key release-------------
void key_release(int key, int x, int y)
{
	if(key == GLUT_KEY_UP)
		pokemon.forward = 0;

	if(key == GLUT_KEY_DOWN)
		pokemon.back = 0;

	if(key == GLUT_KEY_LEFT)
		pokemon.left = 0;

	if(key == GLUT_KEY_RIGHT)
		pokemon.right = 0;
}

// --------------------------------------------------------mouse click--------------
void mouse_click(int button, int state, int x, int y)
{
	camera.mouse_click(button, state, x, y);
}

// -----------------------------------------------------------mouse drag--------------
void mouse_drag(int x, int y)
{
	camera.mouse_drag(x, y);
}

// ---------------------------------------------------------main function------------
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Poliwrath");

	glutDisplayFunc(display);
	glutSpecialFunc(key_press);
	glutSpecialUpFunc(key_release);
	glutMouseFunc(mouse_click);
	glutMotionFunc(mouse_drag);

	glutMainLoop();
	return 0;
}
