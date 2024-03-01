#define dDOUBLE
//#define GL_GLEXT_PROTOTYPES

#include <iostream>
#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GL/glut.h"
#include "ode/ode.h"
#include "SOIL/SOIL.h"
#include <vector>
#include <ctime>
#define GL_GLEXT_PROTOTYPES
#define OBJ_COUNT 501
using namespace std;

dWorldID world;
dBodyID ball_body;
dBodyID cube_body;
dSpaceID space;
dGeomID ball_geom;
dGeomID cube_geom;
dGeomID plane_geom;
dMass ball_mass;
dMass cube_mass;
dJointGroupID cgroup;
GLuint Texture[3];

int MouseX, MouseY;
bool MouseLeftButton = false, MouseRightButton = false, MouseMiddleButton = false;
int CameraX = 1500, CameraY = -700.0, CameraZ = 3000;           //initial Camera Position
float angleX = 0.0, angleY = 0.0;

enum ObjectType { BALL, PLANE, CUBE };

struct Object {
    dBodyID body;
    dGeomID geom;
    dMass mass;
    ObjectType type;
    float color[4];
};
vector<Object> objects;

/*void LoadImages() {

    glGenTextures(3, Texture);

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, Texture[0]);
    Texture[0] = SOIL_load_OGL_texture("woodendoor.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Texture[1]);
    Texture[1] = SOIL_load_OGL_texture("woodensurface.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Texture[2]);
    Texture[2] = SOIL_load_OGL_texture("dominoes.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT);
    for (int i = 0; i < 3; i++) {
        if (0 == Texture[i]) {
            printf("SOIL loading error: '%s'\n", SOIL_last_result());
        }
    }

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
} */

static void nearCallback(void* data, dGeomID o1, dGeomID o2) {          //Collision Control
    const int N = 120;
    dContact contact[N];

    for (int i = 0; i < N; i++) {
        if (i > 2) {
            contact[i].surface.mode = dContactApprox1;          //Collision Settings
            contact[i].surface.mu = 10;
            contact[i].surface.mu2 = 10;
            contact[i].surface.bounce = 0.3;
            contact[i].surface.bounce_vel = 0.0;
        }
        else {
            contact[i].surface.mode = dContactBounce;
            contact[i].surface.mu = dInfinity;
            contact[i].surface.mu2 = 5000;
            contact[i].surface.bounce = 0.8;
            contact[i].surface.bounce_vel = 0.0;
        }
    }
    int n = dCollide(o1, o2, N, &contact[0].geom, sizeof(dContact));

    for (int i = 0; i < n; i++) {
        dJointID c = dJointCreateContact(world, cgroup, &contact[i]);
        dJointAttach(c, dGeomGetBody(o1), dGeomGetBody(o2));
    }

}

void drawDomino(float width, float height, float length)        //Create Domino Tiles
{
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);			// Top face
    glVertex3f(-width / 2, height / 2, -length / 2);
    glVertex3f(-width / 2, height / 2, length / 2);
    glVertex3f(width / 2, height / 2, length / 2);
    glVertex3f(width / 2, height / 2, -length / 2);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f);			// Bottom face
    glVertex3f(-width / 2, -height / 2, -length / 2);
    glVertex3f(width / 2, -height / 2, -length / 2);
    glVertex3f(width / 2, -height / 2, length / 2);
    glVertex3f(-width / 2, -height / 2, length / 2);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f);		   // Left face
    glVertex3f(-width / 2, -height / 2, -length / 2);
    glVertex3f(-width / 2, -height / 2, length / 2);
    glVertex3f(-width / 2, height / 2, length / 2);
    glVertex3f(-width / 2, height / 2, -length / 2);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);	    // Right face
    glVertex3f(width / 2, -height / 2, -length / 2);
    glVertex3f(width / 2, height / 2, -length / 2);
    glVertex3f(width / 2, height / 2, length / 2);
    glVertex3f(width / 2, -height / 2, length / 2);
    glEnd();


    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);	    // Front face
    glVertex3f(-width / 2, -height / 2, length / 2);
    glVertex3f(width / 2, -height / 2, length / 2);
    glVertex3f(width / 2, height / 2, length / 2);
    glVertex3f(-width / 2, height / 2, length / 2);
    glEnd();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, Texture[0]);           //apply texture
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f);	    // Back face
    glTexCoord2f(0, 0);    glVertex3f(-width / 2, -height / 2, -length / 2);
    glTexCoord2f(1, 0);    glVertex3f(-width / 2, height / 2, -length / 2);
    glTexCoord2f(1, 1);    glVertex3f(width / 2, height / 2, -length / 2);
    glTexCoord2f(0, 1);    glVertex3f(width / 2, -height / 2, -length / 2);
    glDisable(GL_TEXTURE_2D);

    glEnd();
}

void DrawTable(float width, float height, float length) {

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, Texture[1]);           //apply texture
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);			// Top face
    glTexCoord2f(0, 0);    glVertex3f(-width / 2, height / 2, -length / 2);
    glTexCoord2f(1, 0);    glVertex3f(-width / 2, height / 2, length / 2);
    glTexCoord2f(1, 1);    glVertex3f(width / 2, height / 2, length / 2);
    glTexCoord2f(0, 1);    glVertex3f(width / 2, height / 2, -length / 2);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f);			// Bottom face
    glVertex3f(-width / 2, -height / 2, -length / 2);
    glVertex3f(width / 2, -height / 2, -length / 2);
    glVertex3f(width / 2, -height / 2, length / 2);
    glVertex3f(-width / 2, -height / 2, length / 2);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3f(-1.0f, 0.0f, 0.0f);		   // Left face
    glVertex3f(-width / 2, -height / 2, -length / 2);
    glVertex3f(-width / 2, -height / 2, length / 2);
    glVertex3f(-width / 2, height / 2, length / 2);
    glVertex3f(-width / 2, height / 2, -length / 2);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3f(1.0f, 0.0f, 0.0f);	    // Right face
    glVertex3f(width / 2, -height / 2, -length / 2);
    glVertex3f(width / 2, height / 2, -length / 2);
    glVertex3f(width / 2, height / 2, length / 2);
    glVertex3f(width / 2, -height / 2, length / 2);
    glEnd();


    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);	    // Front face
    glVertex3f(-width / 2, -height / 2, length / 2);
    glVertex3f(width / 2, -height / 2, length / 2);
    glVertex3f(width / 2, height / 2, length / 2);
    glVertex3f(-width / 2, height / 2, length / 2);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, -1.0f);	    // Back face
    glVertex3f(-width / 2, -height / 2, -length / 2);
    glVertex3f(-width / 2, height / 2, -length / 2);
    glVertex3f(width / 2, height / 2, -length / 2);
    glVertex3f(width / 2, -height / 2, -length / 2);
    glDisable(GL_TEXTURE_2D);

    glEnd();

}

void DrawObject(Object& ob) {
    // materials setup per object basis
    glMaterialfv(GL_FRONT, GL_AMBIENT, ob.color);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, ob.color);
    glMaterialfv(GL_FRONT, GL_SPECULAR, ob.color);
    if (ob.type == BALL) {
        glPushMatrix();
        glMaterialf(GL_FRONT, GL_SHININESS, 120.0);
        const dReal* realP = dBodyGetPosition(ob.body);
        glTranslatef(realP[0], realP[1], realP[2]);
        glutSolidSphere(dGeomSphereGetRadius(ob.geom), 20, 20);

        glPopMatrix();
    }
    else if (ob.type == PLANE) {
        glPushMatrix();
        dVector3 v;         //set a vector v
        dGeomBoxGetLengths(ob.geom, v); //get the size of plane and put it in the v
        dReal* x = (dReal*)&v;
        //       glScalef(x[0], x[1], x[2]);     //make the plane bigger

        DrawTable(v[0], v[1], v[2]);
        //       glutSolidCube(1);               //draw it

        glPopMatrix();
    }
    else if (ob.type == CUBE) {
        glPushMatrix();
        glMaterialf(GL_FRONT, GL_SHININESS, 120.0);
        const dReal* realP = dBodyGetPosition(ob.body);
        const dReal* R = dBodyGetRotation(ob.body);
        glTranslatef(realP[0], realP[1], realP[2]);
        GLdouble matrix[16];                            //make spin around all axis
        matrix[0] = R[0];
        matrix[1] = R[4];
        matrix[2] = R[8];
        matrix[3] = 0;
        matrix[4] = R[1];
        matrix[5] = R[5];
        matrix[6] = R[9];
        matrix[7] = 0;
        matrix[8] = R[2];
        matrix[9] = R[6];
        matrix[10] = R[10];
        matrix[11] = 0;
        matrix[12] = 0;
        matrix[13] = 0;
        matrix[14] = 0;
        matrix[15] = 1;
        glMultMatrixd(matrix);
        drawDomino(50, 100, 10);        //draw the Domino (dimensions)
        //glutSolidCube(20);
        glPopMatrix();
    }
}

void Draw()
{
    dSpaceCollide(space, 0, &nearCallback);

    dWorldQuickStep(world, 0.1);

    dJointGroupEmpty(cgroup);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(angleX, 0.0, 0.5, 0.0);  //rotate the whole scene if angle changes (mouse click and move)
    glRotatef(angleY, 0.2, 0.0, 0.0);

    if (angleX > 360.0) {
        angleX = 0.0;
    }
    if (angleY > 360.0) {
        angleY = 0.0;
    }
    //angle += 1;

    glNormal3f(0.5, 0.5, 0.1);
    for (int i = 0; i < 62; i++) {
        DrawObject(objects[i]);
    }
    glFlush();
    glutSwapBuffers();
}

void Initialize() {

    //LoadImages();
    glClearColor(0.8, 0.9, 0.8, 0.0);
    glEnable(GL_NORMALIZE);

    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_LIGHTING);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glShadeModel(GL_SMOOTH);

    GLfloat qaAmbientLight[] = { 0.2, 0.2, 0.2, 1.0 };    //set light settings
    GLfloat qaDiffuseLight[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat qaSpecularLight[] = { 1.0, 1.0, 1.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, qaAmbientLight);   //call light settings
    glLightfv(GL_LIGHT0, GL_DIFFUSE, qaDiffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, qaSpecularLight);


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glOrtho(-200.0, 200.0, 200.0, -200.0, 0.0, 1000.0);

    // when near is too low(i.e. 0), flickering occurs
    gluPerspective(45.0f, 1.0f, 40.0f, 10000.0f);

    gluLookAt(CameraX, CameraY, CameraZ, 0.0, 0.0, 800.0, 0, -0.5, 0);   //look from an agle

    GLfloat qaLightPosition[] = { 100.0, -600.0, -100.0, 0.0 };     //set light glfloat
    glLightfv(GL_LIGHT0, GL_POSITION, qaLightPosition);     //set light position
    glEnable(GL_LIGHT0);        //lights on		
}

void MakeSomeBalls(void) {    // make some balls

    for (int i = 0; i < 1; i++) {
        Object a;
        a.body = dBodyCreate(world);
        a.type = BALL;
        dMassSetZero(&a.mass);
        dMassSetSphereTotal(&a.mass, 2, 10);
        dBodySetMass(a.body, &a.mass);

        a.geom = dCreateSphere(space, 10);
        dGeomSetBody(a.geom, a.body);
        dGeomSetData(a.geom, (void*)"ball");
        dGeomSetPosition(a.geom, 0, -100, -1400);
        dBodyAddForce(a.body, 0, 0, 2000);

        a.color[0] = rand() % 1000 * 0.001;
        a.color[1] = rand() % 1000 * 0.001;
        a.color[2] = rand() % 1000 * 0.001;
        a.color[3] = 1.0f;

        objects.push_back(a);
    }
}

void MakeSomeDominos(void) {    //make some dominos

    for (int i = 0; i < 60; i++) {
        Object a;
        a.body = dBodyCreate(world);
        a.type = CUBE;
        dMassSetZero(&a.mass);
        dMassSetBoxTotal(&a.mass, 8, 50, 100, 10);
        dBodySetMass(a.body, &a.mass);

        a.geom = dCreateBox(space, 50, 100, 10);
        dGeomSetBody(a.geom, a.body);
        dGeomSetData(a.geom, (void*)"domino");
        dGeomSetPosition(a.geom, 0, -65, -1200 + i * 50);
        dMatrix3 R;
        dRFromAxisAndAngle(R, 1, 0, 0, 3 * M_PI / 180);
        if (int i=1) dGeomSetRotation(a.geom, R);

        /* a.color[0] = rand() % 1000 * 0.001;           //set color to dominoes
        a.color[1] = rand() % 1000 * 0.001;
        a.color[2] = rand() % 1000 * 0.001;
        a.color[3] = 1.0f;
        for (int i = 0; i < 4; i++) {
            a.color[i] = 1;
        }
        */

        objects.push_back(a);
    }

}

void MakeTheTable(void) {
    Object plane;
    plane.type = PLANE;
    plane.geom = dCreateBox(space, 1000.0, 30, 3700.0);

        plane.color[0] = 0.6;   // paint it
        plane.color[1] = 0.0;
        plane.color[2] = 0.2;
        plane.color[3] = 1.0;
    for (int i = 0; i < 4; i++) {
        plane.color[i] = 1;
    }

    objects.push_back(plane);
}

void Timer(int iUnused) {
    glutPostRedisplay();
    glutTimerFunc(1, Timer, 0);
}

void MouseClick(int button, int state, int x, int y) {		//get the mouse down in global bools
    MouseX = x;
    MouseY = y;
    if (button == GLUT_LEFT_BUTTON) {
        MouseLeftButton = true;
    }
    else  MouseLeftButton = false;
    if (button == GLUT_RIGHT_BUTTON) {
        MouseRightButton = true;
    }
    else  MouseRightButton = false;
}
void MouseMove(int x, int y) {				//get the mouse move
    int dX, dY;
    dX = x - MouseX;
    dY = y - MouseY;
    if (MouseLeftButton) {					//when mouse move AND down get the mouse speed in dx
        angleX = angleX + dX;						//angle increase with mouse speed (angle goes to rotate the scene)
        angleY = angleY + dY;
    }
    MouseX = x;
    MouseY = y;
}

int main(int argc, char** argv) {
    srand(time(NULL));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(700, 700);
    glutInitWindowPosition(200, 200);

    glutCreateWindow("test");
    Initialize();
    dInitODE();

    world = dWorldCreate();
    dWorldSetGravity(world, 0.0, 30.0, 0.0);
    dWorldSetERP(world, 0.9);
    dWorldSetCFM(world, 1e-4);
    dWorldSetLinearDamping(world, 0.00001);
    dWorldSetAngularDamping(world, 0.005);
    dWorldSetMaxAngularSpeed(world, 200);
    dWorldSetContactMaxCorrectingVel(world, 0.1);
    dWorldSetContactSurfaceLayer(world, 0.1);

    glutMouseFunc(MouseClick);
    glutMotionFunc(MouseMove);

    space = dSimpleSpaceCreate(0);
    cgroup = dJointGroupCreate(0);

    MakeSomeBalls();
    MakeSomeDominos();
    MakeTheTable();

    glutDisplayFunc(Draw);
    Timer(0);

    glutMainLoop();
    dWorldDestroy(world);

    dCloseODE();
    return 0;
}
