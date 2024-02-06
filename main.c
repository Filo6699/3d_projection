#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

typedef struct {
    double x, y, z;
} vec3;

typedef struct {
    double pitch, yaw;
} rot;

typedef struct {
    rot topLeft, topRight, bottomLeft, bottomRight;
} cameraPoints;

double FOV = 150;
double VECTFOV = 90;
rot cameraRotation = {0, 0};
double xMult = 0;
double yMult = 0;

vec3 points[8] = {
    {6, 1, 1},
    {4, 1, 1},
    {6, 1, -1},
    {4, 1, -1},

    {6, -1, 1},
    {4, -1, 1},
    {6, -1, -1},
    {4, -1, -1},
};
int pointAmount = sizeof(points) / sizeof(vec3);

void init() {
    xMult = 2 / FOV;
    yMult = 2 / VECTFOV;
}

void drawCircle(float centerX, float centerY, float radius, int numSegments) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < numSegments; ++i) {
        float theta = 2.0f * 3.1415926f * i / numSegments;
        float x = radius * cos(theta);
        float y = radius * sin(theta);
        glVertex2f(x + centerX, y + centerY);
    }
    glEnd();
}

void validateRotation(rot *rotation) {
    if (rotation->pitch > 360) {
        rotation->pitch -= 360;
    }
    if (rotation->pitch < 0) {
        rotation->pitch += 360;
    }
    if (rotation->yaw >= 180) {
        rotation->yaw -= 360;
    }
    if (rotation->yaw < -180) {
        rotation->yaw += 360;
    }
}

rot projectPoint(vec3 point) {
    double normalizeFactor = fmax(
        fabs(point.x),
        fmax(
            fabs(point.y),
            fabs(point.z)
        )
    );

    vec3 nPoint = {
        point.x / normalizeFactor,
        point.y / normalizeFactor,
        point.z / normalizeFactor
    };

    double pitch = atan2(nPoint.z, nPoint.x) * 180 / M_PI;

    double yFact = nPoint.y / (fabs(nPoint.x) + fabs(nPoint.y) + fabs(nPoint.z));
    double yaw = 90 * yFact;

    rot rotation = {pitch, yaw};

    return rotation;
}

double validatePitch(double angle) {
    if (angle > 360) {
        angle -= 360;
    }
    if (angle < 0) {
        angle += 360;
    }
    return angle;
}

double validateYaw(double angle) {
    if (angle > 180) {
        angle -= 360;
    }
    if (angle < -180) {
        angle += 360;
    }
    return angle;
}

double angleDiff(double a1, double a2) {
    double diff1 = a2 - a1;
    double diff2 = a1 - a2;

    if (diff1 < 0) {
        diff1 += 360;
    }
    if (diff2 < 0) {
        diff2 += 360;
    }

    if (diff1 < diff2) {
        return -diff1;
    } else {
        return diff2;
    }
}

void drawPoint(rot projection) {

    if (fabs(angleDiff(projection.pitch, cameraRotation.pitch)) <= FOV / 2 &&
        fabs(angleDiff(projection.yaw,   cameraRotation.yaw))   <= VECTFOV / 2) {

        double projX = xMult * angleDiff(projection.pitch, cameraRotation.pitch);
        double projY = yMult * angleDiff(projection.yaw, cameraRotation.yaw);

        drawCircle(projX, projY, 0.01, 25);
    }
}

void update() {
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);

    for (int i = 0; i < pointAmount; i++) {
        vec3 point = points[i];
        rot projection = projectPoint(point);
        drawPoint(projection);
    }

    glFlush();
    validateRotation(&cameraRotation);
}

int main(int argc, char** argv) {
    init();

    glutInit(&argc, argv);
    glutCreateWindow("Insanity");
    glutDisplayFunc(display);
    glutTimerFunc(16, update, 0);
    glutMainLoop();

    return 0;
}
