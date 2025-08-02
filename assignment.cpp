#include <GL/freeglut.h>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>

float zoom = -25.0f;
float camX = 0.0f, camY = 0.0f;
float bobbingAngle = 0.0f;
bool isMoving = true;
float moveSpeed = 0.01f;

struct Bubble {
    float x, y, z, speed;
};
std::vector<Bubble> bubbles;

struct Fish {
    float x, y, z, speed;
    float r, g, b;
};
std::vector<Fish> fishes;

float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

void initLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    GLfloat lightPos[] = { 0.0f, 10.0f, 10.0f, 1.0f };
    GLfloat ambient[] = { 0.2f, 0.2f, 0.3f, 1.0f };
    GLfloat diffuse[] = { 0.9f, 0.9f, 0.9f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
}

void drawSubmarine() {
    float bob = isMoving ? 0.1f * sin(bobbingAngle) : 0.0f;
    glPushMatrix();
    glTranslatef(0.0f, bob, 0.0f);

    // Body
    glPushMatrix();
    glColor3f(0.1f, 0.1f, 0.1f);
    glRotatef(90, 0, 1, 0);
    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, 1.0f, 1.0f, 6.0f, 30, 1);

    glPushMatrix(); glTranslatef(0.0f, 0.0f, 6.0f); gluDisk(quad, 0, 1.0f, 30, 1); glPopMatrix();
    gluDisk(quad, 0, 1.0f, 30, 1);
    glPopMatrix();

    // Top Dome with lights
    glPushMatrix();
    glTranslatef(6.0f, 0.0f, 0.0f);
    glColor3f(0.7f, 0.8f, 1.0f);
    glutSolidSphere(0.5, 30, 30);

    glColor3f(0.5f, 0.6f, 0.9f);
    for (int i = 0; i < 6; i++) {
        float angle = i * 60.0f * 3.14159f / 180.0f;
        float x = 0.7f * cos(angle);
        float y = 0.7f * sin(angle);
        glPushMatrix();
        glTranslatef(x, y, 0.0f);
        glutSolidSphere(0.15, 20, 20);
        glPopMatrix();
    }
    glPopMatrix();

    // Periscope
    glPushMatrix();
    glColor3f(0.2f, 0.2f, 0.2f);
    glTranslatef(3.0f, 0.8f, 0.0f);
    glRotatef(-90, 0, 1, 0);
    quad = gluNewQuadric();
    gluCylinder(quad, 0.1f, 0.1f, 1.0f, 10, 1);
   
    glPopMatrix();

    // Flag
    // Flag pole (thick and black)
    glPushMatrix();
    glTranslatef(3.0f, 0.8f, 0.0f); // position of flag pole
    glColor3f(0.0f, 0.0f, 0.0f);   // black color
    glRotatef(-90, 1, 0, 0);       // vertical
    GLUquadric* pole = gluNewQuadric();
    gluCylinder(pole, 0.05f, 0.05f, 1.0f, 10, 1); // thicker
    gluDeleteQuadric(pole);
    glPopMatrix();

    // Flag (red triangle)
    glPushMatrix();
    glTranslatef(3.0f, 1.8f, 0.0f); // top of the flag pole
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLES);
    glVertex3f(0, 0, 0);
    glVertex3f(0.6f, 0.2f, 0);
    glVertex3f(0, 0.4f, 0);
    glEnd();
    glPopMatrix();


    // Elliptical window
    glPushMatrix();
    glTranslatef(2.0f, 0.0f, 1.01f); // on the side of submarine
    glScalef(0.6f, 0.4f, 1.0f); // Make it elliptical
    glColor3f(1.0f, 1.0f, 1.0f);
    glutSolidSphere(1.0, 30, 30);
    glPopMatrix();

    glPopMatrix();
}

void drawBubbles() {
    if (!isMoving) return;
    glDisable(GL_LIGHTING);
    glColor3f(0.8f, 0.9f, 1.0f);
    for (auto& b : bubbles) {
        glPushMatrix();
        glTranslatef(b.x, b.y, b.z);
        glutSolidSphere(0.05, 10, 10);
        glPopMatrix();
    }
    glEnable(GL_LIGHTING);
}

void drawFish() {
    for (auto& f : fishes) {
        glPushMatrix();
        glTranslatef(f.x, f.y, f.z);
        glScalef(1.5f, 0.7f, 0.5f);  // Elliptical shape
        glColor3f(f.r, f.g, f.b);
        glutSolidSphere(0.2, 20, 20);  // Main body
        glPopMatrix();

        // Fins
        glPushMatrix();
        glTranslatef(f.x, f.y, f.z);
        glColor3f(f.r * 0.8f, f.g * 0.8f, f.b * 0.8f);
        glBegin(GL_TRIANGLES);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(-0.1f, 0.1f, 0.0f);
        glVertex3f(-0.1f, -0.1f, 0.0f);
        glEnd();
        glPopMatrix();
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(camX, camY, zoom);

    drawSubmarine();
    drawBubbles();
    drawFish();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    float ratio = (float)w / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, ratio, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int, int) {
    switch (key) {
    case 'w': zoom += 0.5f; break;  //zoomin
    case 's': zoom -= 0.5f; break;  //zoom out
    case 'q': isMoving = !isMoving; break;  //start stop
    case 'f': moveSpeed = 0.03f; break;  //fast speed
    case 'l': moveSpeed = 0.005f; break;  //slow speed
    case 'm': moveSpeed = 0.01f; break;  //medium speed
    case 27: exit(0);
    }
    glutPostRedisplay();
}

void special(int key, int, int) {
    switch (key) {
    case GLUT_KEY_LEFT:  camX += 0.5f; break;
    case GLUT_KEY_RIGHT: camX -= 0.5f; break;
    case GLUT_KEY_UP:    camY -= 0.5f; break;
    case GLUT_KEY_DOWN:  camY += 0.5f; break;
    }
    glutPostRedisplay();
}

void idle() {
    if (isMoving) {
        bobbingAngle += moveSpeed;
        for (auto& b : bubbles) {
            b.y += b.speed;
            if (b.y > 5.0f) b.y = -1.0f;
        }
        for (auto& f : fishes) {
            f.x += f.speed;
            if (f.x > 15.0f) f.x = -15.0f;
        }
    }
    glutPostRedisplay();
}

void initSceneObjects() {
    for (int i = 0; i < 50; ++i) {
        bubbles.push_back({
            randomFloat(-2.0f, 2.0f),
            randomFloat(-1.0f, 4.0f),
            randomFloat(-3.0f, 3.0f),
            randomFloat(0.002f, 0.005f)
            });
    }

    for (int i = 0; i < 15; ++i) {
        fishes.push_back({
            randomFloat(-10.0f, 10.0f),
            randomFloat(-2.0f, 2.0f),
            randomFloat(-4.0f, 4.0f),
            randomFloat(0.005f, 0.01f),
            randomFloat(0.2f, 1.0f),
            randomFloat(0.2f, 1.0f),
            randomFloat(0.2f, 1.0f)
            });
    }
}

int main(int argc, char** argv) {
    srand(static_cast<unsigned int>(time(0)));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 600);
    glutCreateWindow("Blue Ocean Submarine Scene");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glClearColor(0.0f, 0.3f, 0.6f, 1.0f); // Ocean blue
    initLighting();
    initSceneObjects();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}
