#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"      // Image loading Utility functions

#define GL_GLEXT_PROTOTYPES
#include "glext.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>          // EXIT_FAILURE
using std::stringstream;
using std::cout;
using std::endl;
using std::ends;


// GLUT CALLBACK functions
void displayCB();
void reshapeCB(int w, int h);
void timerCB(int millisec);
void keyboardCB(unsigned char key, int x, int y);
void mouseCB(int button, int stat, int x, int y);
void mouseMotionCB(int x, int y);

void initGL();
int  initGLUT(int argc, char** argv);
bool initSharedMem();
void clearSharedMem();
void initLights();
void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ);
void toPerspective();
void toOrtho();
void moveForward();
void moveBack();
void left(int x);
void right(int x);
void moveUp(int y);
void moveDown(int y);

void top_sponge();
void bottom_sponge();
void wipes();
void wipes_label();
static void doughnut(GLfloat r, GLfloat R, GLint nsides, GLint rings);
void top_towels(GLdouble innerRadius, GLdouble outerRadius, GLint nsides, GLint rings);
void bottom_towels();
void counter();

// constants
const int   SCREEN_WIDTH = 800;
const int   SCREEN_HEIGHT = 600;
const float CAMERA_DISTANCE = 10.0f;

// global variables
int screenWidth;
int screenHeight;
bool mouseLeftDown;
bool mouseRightDown;
bool mouseMiddleDown;
float mouseX, mouseY;
float cameraAngleX;
float cameraAngleY;
float cameraDistance;
int drawMode;
int displayMode;
GLuint texture[8];

#define PI 3.1415927

// cube ///////////////////////////////////////////////////////////////////////
// 
//    v6----- v5
//   /|      /|
//  v1------v0|
//  | |     | |
//  | |v7---|-|v4
//  |/      |/
//  v2------v3

// vertex array for counter and sponge
GLfloat vertices[] = { 1, 1, 1,  -1, 1, 1,  -1,-1, 1,   1,-1, 1,   // v0,v1,v2,v3 (front)
                       1, 1, 1,   1,-1, 1,   1,-1,-1,   1, 1,-1,   // v0,v3,v4,v5 (right)
                       1, 1, 1,   1, 1,-1,  -1, 1,-1,  -1, 1, 1,   // v0,v5,v6,v1 (top)
                      -1, 1, 1,  -1, 1,-1,  -1,-1,-1,  -1,-1, 1,   // v1,v6,v7,v2 (left)
                      -1,-1,-1,   1,-1,-1,   1,-1, 1,  -1,-1, 1,   // v7,v4,v3,v2 (bottom)
                       1,-1,-1,  -1,-1,-1,  -1, 1,-1,   1, 1,-1 }; // v4,v7,v6,v5 (back)

// normal array for counter and sponge
GLfloat normals[] = { 0, 0, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1,   // v0,v1,v2,v3 (front)
                       1, 0, 0,   1, 0, 0,   1, 0, 0,   1, 0, 0,   // v0,v3,v4,v5 (right)
                       0, 1, 0,   0, 1, 0,   0, 1, 0,   0, 1, 0,   // v0,v5,v6,v1 (top)
                      -1, 0, 0,  -1, 0, 0,  -1, 0, 0,  -1, 0, 0,   // v1,v6,v7,v2 (left)
                       0,-1, 0,   0,-1, 0,   0,-1, 0,   0,-1, 0,   // v7,v4,v3,v2 (bottom)
                       0, 0,-1,   0, 0,-1,   0, 0,-1,   0, 0,-1 }; // v4,v7,v6,v5 (back)

//Yellow
GLfloat top_sponge_colors[] = { 1, 1, 0,   1, 1, 0,   1, 1, 0,   1, 1, 0,   // v0,v1,v2,v3 (front)
                                1, 1, 0,   1, 1, 0,   1, 1, 0,   1, 1, 0,   // v0,v3,v4,v5 (right)
                                1, 1, 0,   1, 1, 0,   1, 1, 0,   1, 1, 0,   // v0,v5,v6,v1 (top)
                                1, 1, 0,   1, 1, 0,   1, 1, 0,   1, 1, 0,   // v1,v6,v7,v2 (left)
                                1, 1, 0,   1, 1, 0,   1, 1, 0,   1, 1, 0,   // v7,v4,v3,v2 (bottom)
                                1, 1, 0,   1, 1, 0,   1, 1, 0,   1, 1, 0 }; // v4,v7,v6,v5 (back)

//dark green
GLfloat bottom_sponge_colors[] = { 0.0, 0.1, 0.0,   0.0, 0.1, 0.0,   0.0, 0.1, 0.0,   0.0, 0.1, 0.0,   // v0,v1,v2,v3 (front)
                                0.0, 0.1, 0.0,   0.0, 0.1, 0.0,   0.0, 0.1, 0.0,   0.0, 0.1, 0.0,   // v0,v3,v4,v5 (right)
                                0.0, 0.1, 0.0,   0.0, 0.1, 0.0,   0.0, 0.1, 0.0,   0.0, 0.1, 0.0,   // v0,v5,v6,v1 (top)
                                0.0, 0.1, 0.0,   0.0, 0.1, 0.0,   0.0, 0.1, 0.0,   0.0, 0.1, 0.0,   // v1,v6,v7,v2 (left)
                                0.0, 0.1, 0.0,   0.0, 0.1, 0.0,   0.0, 0.1, 0.0,   0.0, 0.1, 0.0,   // v7,v4,v3,v2 (bottom)
                                0.0, 0.1, 0.0,   0.0, 0.1, 0.0,   0.0, 0.1, 0.0,   0.0, 0.1, 0.0 }; // v4,v7,v6,v5 (back)

// texCoord array for counter and sponge
GLfloat texCoords[] = { 1, 0,   0, 0,   0, 1,   1, 1,             // v0,v1,v2,v3 (front)
                         0, 0,   0, 1,   1, 1,   1, 0,             // v0,v3,v4,v5 (right)
                         1, 1,   1, 0,   0, 0,   0, 1,             // v0,v5,v6,v1 (top)
                         1, 0,   0, 0,   0, 1,   1, 1,             // v1,v6,v7,v2 (left)
                         0, 1,   1, 1,   1, 0,   0, 0,             // v7,v4,v3,v2 (bottom)
                         0, 1,   1, 1,   1, 0,   0, 0, };          // v4,v7,v6,v5 (back)       

// index array of vertex array for counter and sponge
GLubyte indices[] = { 0, 1, 2,   2, 3, 0,      // front
                       4, 5, 6,   6, 7, 4,      // right
                       8, 9,10,  10,11, 8,      // top
                      12,13,14,  14,15,12,      // left
                      16,17,18,  18,19,16,      // bottom
                      20,21,22,  22,23,20 };    // back

// color array for counter
GLfloat counter_colors[] = { 0.6, 0.4, .23,   0.6, 0.4, .23,   0.6, 0.4, .23,   0.6, 0.4, .23,   // v0,v1,v2,v3 (front)
                                 0.6, 0.4, .23,   0.6, 0.4, .23,   0.6, 0.4, .23,   0.6, 0.4, .23,   // v0,v3,v4,v5 (right)
                                 0.6, 0.4, .23,   0.6, 0.4, .23,   0.6, 0.4, .23,   0.6, 0.4, .23,   // v0,v5,v6,v1 (top)
                                 0.6, 0.4, .23,   0.6, 0.4, .23,   0.6, 0.4, .23,   0.6, 0.4, .23,   // v1,v6,v7,v2 (left)
                                 0.6, 0.4, .23,   0.6, 0.4, .23,   0.6, 0.4, .23,   0.6, 0.4, .23,   // v7,v4,v3,v2 (bottom)
                                 0.6, 0.4, .23,   0.6, 0.4, .23,   0.6, 0.4, .23,   0.6, 0.4, .23 }; // v4,v7,v6,v5 (back)

//material settings for objects
GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };

GLfloat mat_ambient[] = { 0.7, 0.7, 0.7, 1.0 };

GLfloat mat_diffuse[] = { 0.1, 0.5, 0.8, 1.0 };

GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };

GLfloat no_shininess[] = { 0.0 };

GLfloat low_shininess[] = { 5.0 };

GLfloat mat_emmission[] = { 0.3, 0.2, 0.2, 0.0 };

///////////////////////////////////////////////////////////////////////////////
// Objects
///////////////////////////////////////////////////////////////////////////////

//Draw the yellow part of the sponge
void top_sponge()
{
    // enable and specify pointers to vertex arrays
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);

    glNormalPointer(GL_FLOAT, 0, normals);
    glColorPointer(3, GL_FLOAT, 0, top_sponge_colors);
    glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
    glVertexPointer(3, GL_FLOAT, 0, vertices);

    glPushMatrix();

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emmission);

    glTranslatef(0.5, -0.9, 1.5); // move to upper left corner
    glScalef(0.8f, 0.2f, 0.5f);

    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);

    glPopMatrix();

    glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

//Draws the green bottom part of the sponge
void bottom_sponge()
{
    // enable and specify pointers to vertex arrays
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);

    glNormalPointer(GL_FLOAT, 0, normals);
    glColorPointer(3, GL_FLOAT, 0, bottom_sponge_colors);
    glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
    glVertexPointer(3, GL_FLOAT, 0, vertices);

    glPushMatrix();

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emmission);

    glTranslatef(0.5, -1.2, 1.5); // move to upper left corner
    glScalef(0.8f, 0.1f, 0.5f);

    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);

    glPopMatrix();

    glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

//Draws the wipes
void wipes()
{
    /* top triangle */
    double i, resolution = 0.1;
    double height = 1;
    double radius = 0.5;

    glPushMatrix();

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emmission);

    glTranslatef(1.2, -1.3, 0);
    glScalef(1.81f, 3.65f, 1.81f);
        
    glBindTexture(GL_TEXTURE_2D, texture[2]); //wipes top

    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1.0f, 1.0f, 0.0f);
    glTexCoord2f(0.5, 0.5);
    glVertex3f(0, height, 0);  /* center */
    for (i = 2 * PI; i >= 0; i -= resolution)

    {
        glNormal3f(1, 1, 1);
        glTexCoord2f(0.5f * cos(i) + 0.5f, 0.5f * sin(i) + 0.5f);
        glVertex3f(radius * cos(i), height, radius * sin(i));
    }
    /* close the loop back to 0 degrees */
    glTexCoord2f(0.5, 0.5);
    glVertex3f(radius, height, 0);
    glEnd();

    /* bottom triangle: note: for is in reverse order */
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.5, 0.5);
    glVertex3f(0, 0, 0);  /* center */
    for (i = 0; i <= 2 * PI; i += resolution)
    {
        glTexCoord2f(0.5f * cos(i) + 0.5f, 0.5f * sin(i) + 0.5f);
        glVertex3f(radius * cos(i), 0, radius * sin(i));
    }
    glEnd();

    /* middle tube */
    glBindTexture(GL_TEXTURE_2D, texture[3]); //wipes middle
    glBegin(GL_QUAD_STRIP);
    glColor3f(1.0f, 1.0f, 1.0f);
    for (i = 0; i <= 2 * PI; i += resolution)
    {
        const float tc = (i / (float)(2 * PI));
        glTexCoord2f(tc, 0.0);
        glVertex3f(radius * cos(i), 0, radius * sin(i));
        glNormal3f(cos(i), 1, sin(i));
        glTexCoord2f(tc, 1.0);
        glVertex3f(radius * cos(i), height, radius * sin(i));
    }
    /* close the loop back to zero degrees */
    glTexCoord2f(0.0, 0.0);
    glVertex3f(radius, 0, 0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(radius, height, 0);
    glEnd();

    glPopMatrix();
}

//Draws the labels on the wipes
void wipes_label()
{
    /* top triangle */
    double i, resolution = 0.1;
    double height = 1;
    double radius = 0.5;

    glPushMatrix();

    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

    glTranslatef(1.2, -1.1, 0);
    glRotatef(-180, 0.0, 1.0, 0.0); //rotates texture to the front camera
    glScalef(1.82f, 3.0f, 1.82f);

    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(0.5, 0.5);
    glVertex3f(0, height, 0);  /* center */
    for (i = 2 * PI; i >= 0; i -= resolution)

    {
        glTexCoord2f(0.5f * cos(i) + 0.5f, 0.5f * sin(i) + 0.5f);
        glVertex3f(radius * cos(i), height, radius * sin(i));
    }
    /* close the loop back to 0 degrees */
    glTexCoord2f(0.5, 0.5);
    glVertex3f(radius, height, 0);
    glEnd();

    /* bottom triangle: note: for is in reverse order */
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.5, 0.5);
    glVertex3f(0, 0, 0);  /* center */
    for (i = 0; i <= 2 * PI; i += resolution)
    {
        glTexCoord2f(0.5f * cos(i) + 0.5f, 0.5f * sin(i) + 0.5f);
        glVertex3f(radius * cos(i), 0, radius * sin(i));
    }
    glEnd();

    /* middle tube */
    glBindTexture(GL_TEXTURE_2D, texture[4]); //wipes label
    glBegin(GL_QUAD_STRIP);
    glColor3f(1.0f, 1.0f, 1.0f);
    for (i = 0; i <= 2 * PI; i += resolution)
    {
        const float tc = (i / (float)(2 * PI));
        glTexCoord2f(tc, 0.0);
        glVertex3f(radius * cos(i), 0, radius * sin(i));
        glNormal3f(cos(i), 1, sin(i));
        glTexCoord2f(tc, 1.0);
        glVertex3f(radius * cos(i), height, radius * sin(i));
    }
    /* close the loop back to zero degrees */
    glTexCoord2f(0.0, 0.0);
    glVertex3f(radius, 0, 0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(radius, height, 0);
    glEnd();

    glPopMatrix();
}

//draws the top of paper towels
static void doughnut(GLfloat r, GLfloat R, GLint nsides, GLint rings)
{

    int i, j;
    GLfloat theta, phi, theta1;
    GLfloat cosTheta, sinTheta;
    GLfloat cosTheta1, sinTheta1;
    GLfloat ringDelta, sideDelta;

    ringDelta = 2.0 * PI / rings;
    sideDelta = 2.0 * PI / nsides;

    theta = 0.0;
    cosTheta = 1.0;
    sinTheta = 0.0;

    for (i = rings - 1; i >= 0; i--) {
        theta1 = theta + ringDelta;
        cosTheta1 = cos(theta1);
        sinTheta1 = sin(theta1);

        glBindTexture(GL_TEXTURE_2D, texture[5]); //top of towels
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_QUAD_STRIP);
        phi = 0.0;
        for (j = nsides; j >= 0; j--) {
            GLfloat cosPhi, sinPhi, dist;

            phi += sideDelta;
            cosPhi = cos(phi);
            sinPhi = sin(phi);
            dist = R + r * cosPhi;

            glNormal3f(cosTheta1 * cosPhi, -sinTheta1 * cosPhi, sinPhi);
            glVertex3f(cosTheta1 * dist, -sinTheta1 * dist, r * sinPhi);
            glNormal3f(cosTheta * cosPhi, -sinTheta * cosPhi, sinPhi);
            glVertex3f(cosTheta * dist, -sinTheta * dist, r * sinPhi);

            glTexCoord2f(ringDelta / PI, sideDelta / PI);

        }
        glEnd();
        theta = theta1;
        cosTheta = cosTheta1;
        sinTheta = sinTheta1;

    }

}

void top_towels(GLdouble innerRadius, GLdouble outerRadius, GLint nsides, GLint rings)
{
    glPushMatrix();

    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

    glTranslatef(-1.0, 3.2, -0.3);
    glRotatef(-90, 1.0, 0.0, 0.0);
    glScalef(1.566f, 1.566f, 0.1f);
    doughnut(innerRadius, outerRadius, nsides, rings); //draws torus
    glPopMatrix();
}

//Draws bottom of paper towels
void bottom_towels()
{


    /* top triangle */
    double i, resolution = 0.1;
    double height = 1;
    double radius = 0.5;

    glPushMatrix();

    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);

    glTranslatef(-1.0, -1.3, -0.3);
    glScalef(2.5f, 4.5f, 2.5f);

    glBegin(GL_TRIANGLE_FAN);
    glColor3f(0.0f, 0.0f, 0.0f);
    glTexCoord2f(0.5, 0.5);
    glVertex3f(0, height, 0);  /* center */
    for (i = 2 * PI; i >= 0; i -= resolution)

    {
        glTexCoord2f(0.5f * cos(i) + 0.5f, 0.5f * sin(i) + 0.5f);
        glVertex3f(radius * cos(i), height, radius * sin(i));
    }
    /* close the loop back to 0 degrees */
    glTexCoord2f(0.5, 0.5);
    glVertex3f(radius, height, 0);
    glEnd();

    /* bottom triangle: note: for is in reverse order */
    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.5, 0.5);
    glVertex3f(0, 0, 0);  /* center */
    for (i = 0; i <= 2 * PI; i += resolution)
    {
        glTexCoord2f(0.5f * cos(i) + 0.5f, 0.5f * sin(i) + 0.5f);
        glVertex3f(radius * cos(i), 0, radius * sin(i));
    }
    glEnd();

    /* middle tube */
    glBindTexture(GL_TEXTURE_2D, texture[6]); //bottom towels
    glBegin(GL_QUAD_STRIP);
    glColor3f(1.0f, 1.0f, 1.0f);
    for (i = 0; i <= 2 * PI; i += resolution)
    {
        const float tc = (i / (float)(2 * PI));
        glTexCoord2f(tc, 0.0);
        glVertex3f(radius * cos(i), 0, radius * sin(i));
        glNormal3f(cos(i), 1, sin(i));
        glTexCoord2f(tc, 1.0);
        glVertex3f(radius * cos(i), height, radius * sin(i));
    }
    /* close the loop back to zero degrees */
    glTexCoord2f(0.0, 0.0);
    glVertex3f(radius, 0, 0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(radius, height, 0);
    glEnd();

    glPopMatrix();
}

//Draws counter
void counter()
{
    // enable and specify pointers to vertex arrays
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);

    glNormalPointer(GL_FLOAT, 0, normals);
    glColorPointer(3, GL_FLOAT, 0, counter_colors);
    glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
    glVertexPointer(3, GL_FLOAT, 0, vertices);

    glPushMatrix();

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emmission);

    glTranslatef(-0.2, -1.3, -0.02);
    glRotatef(-90, 1.0, 0.0, 0.0);
    glScalef(5.0f, 5.0f, 0.01f);

    glBindTexture(GL_TEXTURE_2D, texture[7]); //counter
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);

    glPopMatrix();

    glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}

///////////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
    // init global vars
    initSharedMem();

    // init GLUT and GL
    initGLUT(argc, argv);
    initGL();

    ///////////////////////////////////////////////////////////////////////////////
    // Texture
    ///////////////////////////////////////////////////////////////////////////////

    int width, height, channels;
    
    //copy the texture to OpenGL
    glGenTextures(8, texture);
      
    ///////////////////////////////////////////////////////////////////////////////
    //top sponge
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    unsigned char* image = stbi_load("sponge_top.jpg", &width, &height, &channels, 0);
    // select modulate to mix texture with color for shading
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    stbi_image_free(image);
    // unbind
    glBindTexture(GL_TEXTURE_2D, 0);

    ///////////////////////////////////////////////////////////////////////////////
    //bottom sponge
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    image = stbi_load("sponge_bottom.jpg", &width, &height, &channels, 0);
    // select modulate to mix texture with color for shading
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    stbi_image_free(image);
    // unbind
    glBindTexture(GL_TEXTURE_2D, 0);
        
    ///////////////////////////////////////////////////////////////////////////////
    //wipes top
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    stbi_set_flip_vertically_on_load(true);
    image = stbi_load("wipes_top.jpg", &width, &height, &channels, 0);
    // select modulate to mix texture with color for shading
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    stbi_image_free(image);
    // unbind
    glBindTexture(GL_TEXTURE_2D, 0);
    
    ///////////////////////////////////////////////////////////////////////////////
    //wipes middle
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    stbi_set_flip_vertically_on_load(true);
    image = stbi_load("wipes_middle.jpg", &width, &height, &channels, 0);
    // select modulate to mix texture with color for shading
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    stbi_image_free(image);
    // unbind
    glBindTexture(GL_TEXTURE_2D, 0);

    ///////////////////////////////////////////////////////////////////////////////
    //wipes label
    glBindTexture(GL_TEXTURE_2D, texture[4]);
    stbi_set_flip_vertically_on_load(true);
    image = stbi_load("wipes_label.jpg", &width, &height, &channels, 0);
    // select modulate to mix texture with color for shading
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    stbi_image_free(image);
    // unbind
    glBindTexture(GL_TEXTURE_2D, 0);

    ///////////////////////////////////////////////////////////////////////////////
    //top of towels
    glBindTexture(GL_TEXTURE_2D, texture[5]);
    image = stbi_load("towel_top.jpg", &width, &height, &channels, 0);
    // select modulate to mix texture with color for shading
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    stbi_image_free(image);
    // unbind
    glBindTexture(GL_TEXTURE_2D, 0);

    ///////////////////////////////////////////////////////////////////////////////
    //bottom towels
    glBindTexture(GL_TEXTURE_2D, texture[6]);
    stbi_set_flip_vertically_on_load(true);
    image = stbi_load("towel_bottom.jpg", &width, &height, &channels, 0);
    // select modulate to mix texture with color for shading
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    stbi_image_free(image);
    // unbind
    glBindTexture(GL_TEXTURE_2D, 0);

    ///////////////////////////////////////////////////////////////////////////////
    //counter
    glBindTexture(GL_TEXTURE_2D, texture[7]);
    image = stbi_load("counter.jpg", &width, &height, &channels, 0);
    // select modulate to mix texture with color for shading
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    stbi_image_free(image);
    // unbind
    glBindTexture(GL_TEXTURE_2D, 0);

    ///////////////////////////////////////////////////////////////////////////////
    // End of Texture
    ///////////////////////////////////////////////////////////////////////////////


    // the last GLUT call (LOOP)
    // window will be shown and display callback is triggered by events
    // NOTE: this call never return main().
    glutMainLoop(); /* Start GLUT event-processing loop */

    //deletes textures
    glDeleteTextures(8, texture);

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// initialize GLUT for windowing
///////////////////////////////////////////////////////////////////////////////
int initGLUT(int argc, char** argv)
{
    // GLUT stuff for windowing
    // initialization openGL window.
    // it is called before any other GLUT routine
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);   // display mode

    glutInitWindowSize(screenWidth, screenHeight);  // window size

    glutInitWindowPosition(100, 100);               // window location

    // finally, create a window with openGL context
    // Window will not displayed until glutMainLoop() is called
    // it returns a unique ID
    int handle = glutCreateWindow(argv[0]);     // param is the title of window

    // register GLUT callback functions
    glutDisplayFunc(displayCB);
    glutTimerFunc(33, timerCB, 33);             // redraw only every given millisec
    glutReshapeFunc(reshapeCB);
    glutKeyboardFunc(keyboardCB);
    glutMouseFunc(mouseCB);
    glutMotionFunc(mouseMotionCB);

    return handle;
}

///////////////////////////////////////////////////////////////////////////////
// initialize OpenGL
// disable unused features
///////////////////////////////////////////////////////////////////////////////
void initGL()
{
    glShadeModel(GL_SMOOTH);                    // shading mathod: GL_SMOOTH or GL_FLAT
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);      // 4-byte pixel alignment

    // enable /disable features
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    //glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    //glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);

    // track material ambient and diffuse from surface color, call it before glEnable(GL_COLOR_MATERIAL)
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);

    glClearColor(0, 0, 0, 0);                   // background color
    glClearStencil(0);                          // clear stencil buffer
    glClearDepth(1.0f);                         // 0 is near, 1 is far
    glDepthFunc(GL_LEQUAL);

    initLights();
}

///////////////////////////////////////////////////////////////////////////////
// initialize global variables
///////////////////////////////////////////////////////////////////////////////
bool initSharedMem()
{
    screenWidth = SCREEN_WIDTH;
    screenHeight = SCREEN_HEIGHT;

    mouseLeftDown = mouseRightDown = mouseMiddleDown = false;
    mouseX = mouseY = 0;

    cameraAngleX = cameraAngleY = 0.0f;
    cameraDistance = CAMERA_DISTANCE;

    drawMode = 0; // 0:fill, 1: wireframe, 2:points
    displayMode = 0; //0: Perspective, 1: ortho

    return true;
}

///////////////////////////////////////////////////////////////////////////////
// clean up global vars
///////////////////////////////////////////////////////////////////////////////
void clearSharedMem()
{
}

///////////////////////////////////////////////////////////////////////////////
// initialize lights
///////////////////////////////////////////////////////////////////////////////
void initLights()
{
    // set up light colors (ambient, diffuse, specular)
    GLfloat lightKa[] = { .2f, .2f, .2f, 1.0f };  // ambient light
    GLfloat lightKd[] = { .7f, .7f, .7f, 1.0f };  // diffuse light
    GLfloat lightKs[] = { 1, 1, 1, 1 };           // specular light
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);

    // position the light
    float lightPos[4] = { 0, 20, 0, 1 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glEnable(GL_LIGHT0);                        // MUST enable each light source after configuration
}

///////////////////////////////////////////////////////////////////////////////
// set camera position and lookat direction
///////////////////////////////////////////////////////////////////////////////
void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(posX, posY, posZ, targetX, targetY, targetZ, 0, 1, 0); // eye(x,y,z), focal(x,y,z), up(x,y,z)
}

///////////////////////////////////////////////////////////////////////////////
// set the projection matrix as perspective
///////////////////////////////////////////////////////////////////////////////
void toPerspective()
{
    // set viewport to be the entire window
    glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);

    // set perspective viewing frustum
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, (float)(screenWidth) / screenHeight, 1.0f, 1000.0f); // FOV, AspectRatio, NearClip, FarClip

    // switch to modelview matrix in order to set scene
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

///////////////////////////////////////////////////////////////////////////////
// set projection matrix as orthogonal
///////////////////////////////////////////////////////////////////////////////
void toOrtho()
{
    // set viewport to be the entire window
    glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);

    // set orthographic viewing frustum
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, screenWidth, 0, screenHeight, -1, 1);

    // switch to modelview matrix in order to set scene
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


//=============================================================================
// CALLBACKS
//=============================================================================

void displayCB()
{
    // clear buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // save the initial ModelView matrix before modifying ModelView matrix
    glPushMatrix();

    // tramsform camera
    glTranslatef(0, 0, -cameraDistance);
    glRotatef(cameraAngleX, 1, 0, 0);   // pitch
    glRotatef(cameraAngleY, 0, 1, 0);   // heading

    top_sponge();
    bottom_sponge();

    wipes();
    wipes_label();
    
    top_towels(0.3, 0.5, 50, 50);
    bottom_towels();

    counter();

    glPopMatrix();

    glutSwapBuffers();
}

//Reshapes window
void reshapeCB(int w, int h)
{
    screenWidth = w;
    screenHeight = h;
    toPerspective();
}

void timerCB(int millisec)
{
    glutTimerFunc(millisec, timerCB, millisec);
    glutPostRedisplay();
}

//keyoard functions
void moveForward()
{
    cameraDistance -= cameraDistance * 0.2f;
   
}
void moveBack()
{
    cameraDistance += cameraDistance * 0.2f;
}
void left(int x)
{
    cameraAngleY += x * 0.02f;
}
void right(int x)
{
    cameraAngleY -= x * 0.02f;
}
void moveUp(int y)
{
    cameraAngleX += y * 0.02f;
}
void moveDown(int y)
{
    cameraAngleX -= y * 0.02f;
}

//process all input : query GLFW whether relevant keys are pressed / released this frame and react accordingly
void keyboardCB(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27: // ESCAPE
        clearSharedMem();
        exit(0);
        break;

        //moves camera forward
    case 'w':
    case 'W':
        moveForward();
        break;

        // moves camera back
    case 's':
    case 'S':
        moveBack();
        break;

        //moves camera left
    case 'a':
    case 'A':
        left(x);
        break;

        //moves camera right
    case 'd':
    case 'D':
        right(x);
        break;

        //moves camera up
    case 'q':
    case 'Q':
        moveUp(y);
        break;

        //moves camera down
    case 'e':
    case 'E':
        moveDown(y);
        break;

        //changes between persective and orthogonal mode
    case 'p':
    case 'P':
        displayMode = ++displayMode % 2;
        if (displayMode == 0) //perspective mode
        {
            toPerspective();
        }
        else
        {
            // orthogonal mode
            glMatrixMode(GL_PROJECTION);        // switch to projection matrix
            glPushMatrix();                     // save current projection matrix
            glLoadIdentity();                   // reset projection matrix
            gluOrtho2D(0, screenWidth, 0, screenHeight); // set to orthogonal projection
        }
        break;

    case 'v': // switch rendering modes (fill -> wire -> point)
    case 'V':
        drawMode = ++drawMode % 3;
        if (drawMode == 0)        // fill mode
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
        }
        else if (drawMode == 1)  // wireframe mode
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
        }
        else                    // point mode
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);
        }
        break;

    default:
        ;
    }
}

void mouseCB(int button, int state, int x, int y)
    {
        mouseX = x;
        mouseY = y;

        if (button == GLUT_LEFT_BUTTON)
        {
            if (state == GLUT_DOWN)
            {
                mouseLeftDown = true;
            }
            else if (state == GLUT_UP)
                mouseLeftDown = false;
        }

        else if (button == GLUT_RIGHT_BUTTON)
        {
            if (state == GLUT_DOWN)
            {
                mouseRightDown = true;
            }
            else if (state == GLUT_UP)
                mouseRightDown = false;
        }

        else if (button == GLUT_MIDDLE_BUTTON)
        {
            if (state == GLUT_DOWN)
            {
                mouseMiddleDown = true;
            }
            else if (state == GLUT_UP)
                mouseMiddleDown = false;
        }
    }

void mouseMotionCB(int x, int y)
{
    if (mouseLeftDown)
    {
        cameraAngleY += (x - mouseX);
        cameraAngleX += (y - mouseY);
        mouseX = x;
        mouseY = y;
    }
    if (mouseRightDown)
    {
        cameraDistance -= (y - mouseY) * 0.2f;
        mouseY = y;
    }
}
