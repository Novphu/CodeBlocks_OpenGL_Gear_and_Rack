#include "gear_rack_simulator.h"

extern GearProperties gear1Props;
extern GearProperties gear2Props;
extern GearProperties gear3Props;
extern GearProperties gear4Props;
extern float rackColor[3];
extern float componentsAngleY;

float gearColors[4][3] = {
    {0.2, 0.5, 0.2},
    {0.75, 0.75, 0.75},
    {0.8, 0.5, 0.2},
    {0.5, 0.9, 0.2}
};

// Gear angles and rotation
float angleGear1 = 0.0;
float angleGear2 = 0.0;
float angleGear3 = 0.0;
float angleGear4 = 0.0;
float rotationSpeedGear = 2.0;

// Cylinder properties
GLfloat baseRadius = 0.3;
GLfloat topRadius = 0.15;
GLfloat height = 2.3;
int slices = 50;
int stacks = 30;


extern float rotateangle ;
extern float rackPositionX ;
extern float rackSpeed ;
extern bool rackMovingRight ;
extern bool isRotating ;


// Rotation control
extern float currentRotationSpeed;
extern float targetRotationSpeed;
extern float acceleration;
extern bool isAccelerating;
extern bool isDecelerating;

void drawBeveledEdge(float x1, float y1, float z1,
                            float x2, float y2, float z2,
                            float x3, float y3, float z3,
                            float x4, float y4, float z4)
{
    glBegin(GL_QUADS);
    calculateNormal(x1, y1, z1, x2, y2, z2, x3, y3, z3);
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y2, z2);
    glVertex3f(x3, y3, z3);
    glVertex3f(x4, y4, z4);
    glEnd();
}

void drawTooth(float angle, float nextAngle, float outerRadiusBase, float toothHeight,
               float toothWidthTop, float thickness, float bevelAmount)
{
    float midAngle = (angle + nextAngle) / 2;
    float outerRadiusTop = outerRadiusBase + toothHeight;

    float x1 = cos(angle) * outerRadiusBase;
    float y1 = sin(angle) * outerRadiusBase;
    float x4 = cos(nextAngle) * outerRadiusBase;
    float y4 = sin(nextAngle) * outerRadiusBase;

    float x2 = cos(midAngle - toothWidthTop / 2) * outerRadiusTop;
    float y2 = sin(midAngle - toothWidthTop / 2) * outerRadiusTop;
    float x3 = cos(midAngle + toothWidthTop / 2) * outerRadiusTop;
    float y3 = sin(midAngle + toothWidthTop / 2) * outerRadiusTop;

    // M?t tru?c rang
    drawBeveledEdge(x1, y1, thickness / 2, x2, y2, thickness / 2, x3, y3, thickness / 2, x4, y4, thickness / 2);

    // M?t sau rang
    drawBeveledEdge(x4, y4, -thickness / 2, x3, y3, -thickness / 2, x2, y2, -thickness / 2, x1, y1, -thickness / 2);

    // Các m?t bên
    drawBeveledEdge(x1, y1, thickness / 2, x1, y1, -thickness / 2, x2, y2, -thickness / 2, x2, y2, thickness / 2);
    drawBeveledEdge(x4, y4, thickness / 2, x4, y4, -thickness / 2, x3, y3, -thickness / 2, x3, y3, thickness / 2);

    // M?t trên (d?nh rang)
    drawBeveledEdge(x2, y2, thickness / 2, x2, y2, -thickness / 2, x3, y3, -thickness / 2, x3, y3, thickness / 2);

    // Bevel edges
    float bevelOuter1x = cos(angle) * (outerRadiusBase + bevelAmount);
    float bevelOuter1y = sin(angle) * (outerRadiusBase + bevelAmount);
    float bevelOuter2x = cos(nextAngle) * (outerRadiusBase + bevelAmount);
    float bevelOuter2y = sin(nextAngle) * (outerRadiusBase + bevelAmount);

    drawBeveledEdge(x1, y1, thickness / 2, bevelOuter1x, bevelOuter1y, thickness / 2, bevelOuter1x, bevelOuter1y, -thickness / 2, x1, y1, -thickness / 2);
    drawBeveledEdge(x4, y4, thickness / 2, bevelOuter2x, bevelOuter2y, thickness / 2, bevelOuter2x, bevelOuter2y, -thickness / 2, x4, y4, -thickness / 2);
}

void drawGear(GearProperties props, int gearIndex)
{
     // S? d?ng màu t? m?ng gearColors
    GLfloat gearAmbient[] = {gearColors[gearIndex][0]*0.2, gearColors[gearIndex][1]*0.2, gearColors[gearIndex][2]*0.2, 1.0};
    GLfloat gearDiffuse[] = {gearColors[gearIndex][0], gearColors[gearIndex][1], gearColors[gearIndex][2], 1.0};
    GLfloat gearSpecular[] = {0.8, 0.8, 0.8, 1.0};
    GLfloat gearShininess = 50.0;

    glMaterialfv(GL_FRONT, GL_AMBIENT, gearAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, gearDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, gearSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, gearShininess);

    // V? ph?n trung tâm (h?nh tr?)
    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, props.innerRadius, props.innerRadius, props.thickness, 100, 1);
    gluDeleteQuadric(quad);

    // V? các rang
    for (int i = 0; i < props.numTeeth; i++)
    {
        float angle = 2 * PI * i / props.numTeeth;
        float nextAngle = 2 * PI * (i + 1) / props.numTeeth;
        drawTooth(angle, nextAngle, props.outerRadius, props.toothHeight, props.toothWidthTop, props.thickness, bevelAmount);
    }

    // V? m?t trên và du?i c?a bánh rang
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, props.thickness / 2);
    for (int i = 0; i <= 100; ++i)
    {
        float angle = 2 * PI * i / 100;
        glVertex3f(cos(angle) * props.outerRadius, sin(angle) * props.outerRadius, props.thickness / 2);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(0.0, 0.0, -props.thickness / 2);
    for (int i = 0; i <= 100; ++i)
    {
        float angle = 2 * PI * i / 100;
        glVertex3f(cos(angle) * props.outerRadius, sin(angle) * props.outerRadius, -props.thickness / 2);
    }
    glEnd();
}


void drawBox()
{
    glBegin(GL_QUADS);

    glNormal3f(0.0 ,0.0, 1.0);
    glVertex3f(-0.5, -0.5, 0.5);
    glVertex3f(0.5, -0.5, 0.5);
    glVertex3f(0.5, 0.5, 0.5);
    glVertex3f(-0.5, 0.5, 0.5);

    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(-0.5, -0.5, -0.5);
    glVertex3f(0.5, -0.5, -0.5);
    glVertex3f(0.5, 0.5, -0.5);
    glVertex3f(-0.5, 0.5, -0.5);

    glNormal3f(-1.0, 0.0, 0.0);
    glVertex3f(-0.5, -0.5, -0.5);
    glVertex3f(-0.5, -0.5, 0.5);
    glVertex3f(-0.5, 0.5, 0.5);
    glVertex3f(-0.5, 0.5, -0.5);

    glNormal3f(1.0, 0.0, 0.0);
    glVertex3f(0.5, -0.5, -0.5);
    glVertex3f(0.5, -0.5, 0.5);
    glVertex3f(0.5, 0.5, 0.5);
    glVertex3f(0.5, 0.5, -0.5);

    glNormal3f(0.0, -1.0, 0.0);
    glVertex3f(-0.5, -0.5, -0.5);
    glVertex3f(0.5,0.5,-0.5);
    glVertex3f(0.5, -0.5, 0.5);
    glVertex3f(-0.5, -0.5, 0.5);

    glNormal3f(0.0, 1.0, 0.0);
    glVertex3f(-0.5, 0.5, -0.5);
    glVertex3f(0.5, 0.5, -0.5);
    glVertex3f(0.5, 0.5, 0.5);
    glVertex3f(-0.5, 0.5, 0.5);
    glEnd();
}

void drawGround()
{
    // L?u tr?ng thái lighting hi?n t?i
    glPushAttrib(GL_LIGHTING_BIT);

    // Thi?t l?p v?t li?u cho m?t ??t
    GLfloat groundAmbient[] = {0.2, 0.3, 0.2, 1.0};
    GLfloat groundDiffuse[] = {0.4, 0.6, 0.4, 1.0};
    GLfloat groundSpecular[] = {0.1, 0.1, 0.1, 1.0};
    float groundShininess = 20.0;

    glMaterialfv(GL_FRONT, GL_AMBIENT, groundAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, groundDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, groundSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, groundShininess);

    // Kích th??c và ?? phân gi?i m?t ??t
    float groundSize = 25.0;
    float yPos = -2.5;
    int divisions = 30;

    // V? m?t ??t chính (màu xanh lá nh?t)
    glBegin(GL_QUADS);
    glNormal3f(0.0, 1.0, 0.0);
    glVertex3f(-groundSize, yPos, -groundSize);
    glVertex3f(groundSize, yPos, -groundSize);
    glVertex3f(groundSize, yPos, groundSize);
    glVertex3f(-groundSize, yPos, groundSize);
    glEnd();

    // T?t lighting ?? v? ???ng k?
    glDisable(GL_LIGHTING);

    // V? các ???ng k? song song (l??i)
    float step = (2 * groundSize) / divisions;
    glLineWidth(1.0);
    glBegin(GL_LINES);

    // Màu ???ng k? chính (xám ??m)
    glColor3f(0.3, 0.3, 0.3);
    for(float i = -groundSize; i <= groundSize; i += step)
    {
        // ???ng theo tr?c X
        glVertex3f(i, yPos + 0.02, -groundSize);
        glVertex3f(i, yPos + 0.02, groundSize);

        // ???ng theo tr?c Z
        glVertex3f(-groundSize, yPos + 0.02, i);
        glVertex3f(groundSize, yPos + 0.02, i);
    }
    glEnd();

    // B?t l?i lighting
    glEnable(GL_LIGHTING);

    // Khôi ph?c tr?ng thái lighting ban ??u
    glPopAttrib();
}

void drawCylinderEndBox(float width, float height, float depth)
{
    glTranslatef(0.0,0.0,0.5);
    glRotatef(90.0,1.0,0.0,0.0);

    glBegin(GL_QUADS);
    glColor3f(0.9, 0.4, 0.1);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(-width / 2, -height / 2, depth / 2);
    glVertex3f(width / 2, -height / 2, depth / 2);
    glVertex3f(width / 2, height / 2, depth / 2);
    glVertex3f(-width / 2, height / 2, depth / 2);

    glColor3f(0.9, 0.4, 0.1);
    glNormal3f(0.0, 0.0, -1.0);
    glVertex3f(-width / 2, -height / 2, -depth /2);
    glVertex3f(width / 2, -height / 2, -depth / 2);
    glVertex3f(width / 2, height / 2, -depth / 2);
    glVertex3f(-width / 2, height / 2, -depth / 2);

    glColor3f(0.9, 0.4, 0.1);
    glNormal3f(-1.0, 0.0, 0.0);
    glVertex3f(-width / 2, -height / 2, -depth / 2);
    glVertex3f(-width / 2, -height / 2, depth / 2);
    glVertex3f(-width / 2, height / 2, depth / 2);
    glVertex3f(-width / 2, height / 2, -depth / 2);

    glColor3f(0.9, 0.4, 0.1);
    glNormal3f(1.0, 0.0, 0.0);
    glVertex3f(width / 2, -height / 2, -depth / 2);
    glVertex3f(width / 2, -height / 2, depth / 2);
    glVertex3f(width / 2, height / 2, depth / 2);
    glVertex3f(width / 2, height / 2, -depth / 2);

    glColor3f(0.9, 0.4, 0.1);
    glNormal3f(0.0, -1.0, 0.0);
    glVertex3f(-width / 2, -height / 2, -depth / 2);
    glVertex3f(width / 2, -height / 2, -depth / 2);
    glVertex3f(width / 2, -height / 2, depth / 2);
    glVertex3f(-width / 2, -height / 2, depth / 2);

    glColor3f(0.9, 0.4, 0.1);
    glNormal3f(0.0, 1.0, 0.0);
    glVertex3f(-width / 2, height / 2, -depth / 2);
    glVertex3f(width / 2, height / 2, -depth / 2);
    glVertex3f(width / 2, height / 2, depth / 2);
    glVertex3f(-width / 2, height / 2, depth / 2);
    glEnd();
}

void drawCylinder(float radius, float height, int slices, int stacks)
{
    glTranslatef(0.0,0.5,0.5);
    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, radius, radius, height, slices, stacks);
    gluDeleteQuadric(quad);
}

void drawCircle(float radius, int slices)
{
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0.0, 0.0, 0.0);
    for (int i = 0; i <= slices; ++i)
    {
        float angle = 2.0 * M_PI * i / slices;
        glVertex3f(radius * cos(angle), radius * sin(angle), 0.0);
    }
    glEnd();
}

void drawSideCylinder(float offsetX, float offsetY, float offsetZ, float rotationAngle,
                      float rotationX, float rotationY, float rotationZ, float radius, float height)
    {
    glPushMatrix();
        glTranslatef(offsetX, offsetY, offsetZ);
        glRotatef(rotationAngle, rotationX, rotationY, rotationZ);
        glColor3f(0.2, 0.6, 0.8);
        drawCylinder(radius, height, 100, 100);
        glPushMatrix();
            glTranslatef(0.0, 0.0, height);
            glColor3f(0.4, 0.8, 0.8);
            drawCircle(radius, 100);
        glPopMatrix();
        glPushMatrix();
            glColor3f(0.4, 0.8, 0.8);
            drawCircle(radius, 100);
        glPopMatrix();
    glPopMatrix();
}

void drawRackBevel(RackProperties rackProps)
{
    float bevel = rackProps.bevelSize;

    glBegin(GL_QUADS);
        // Vát mép m?t tru?c
        glNormal3f(0.0, -1.0, 1.0);
        glVertex3f(0.0, 0.0, rackProps.thickness/2);
        glVertex3f(rackProps.length, 0.0, rackProps.thickness/2);
        glVertex3f(rackProps.length, bevel, rackProps.thickness/2 - bevel);
        glVertex3f(0.0, bevel, rackProps.thickness/2 - bevel);

        // Vát mép m?t sau
        glNormal3f(0.0, -1.0, -1.0);
        glVertex3f(0.0, 0.0, -rackProps.thickness/2);
        glVertex3f(rackProps.length, 0.0, -rackProps.thickness/2);
        glVertex3f(rackProps.length, bevel, -rackProps.thickness/2 + bevel);
        glVertex3f(0.0, bevel, -rackProps.thickness/2 + bevel);
    glEnd();
}

void drawTrapezoidalTooth(float x, RackProperties rackProps)
{
	// Thi?t l?p v?t li?u cho rang (màu vàng)
    GLfloat toothAmbient[] = {0.4, 0.4, 0.2, 1.0};
    GLfloat toothDiffuse[] = {0.9, 0.8, 0.1, 1.0};
    GLfloat toothSpecular[] = {0.7, 0.7, 0.7, 1.0};
    float toothShininess = 60.0;

    glMaterialfv(GL_FRONT, GL_AMBIENT, toothAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, toothDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, toothSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, toothShininess);

    float x_left = x;
    float x_right = x + rackProps.toothWidthBase;
    float x_top_left = x + (rackProps.toothWidthBase - rackProps.toothWidthTop)/2.0;
    float x_top_right = x_top_left + rackProps.toothWidthTop;
    float y_base = 0.0; // ?áy rang n?m ? y = 0 theo h? t?a d? c?a thanh rang
    float y_top = rackProps.toothHeight;
    float z_front = rackProps.thickness/2;
    float z_back = -rackProps.thickness/2;

    // M?t tru?c rang
    glBegin(GL_QUADS);
        calculateNormal(x_left, y_base, z_front,x_right, y_base, z_front,x_top_right, y_top, z_front);
        glVertex3f(x_left, y_base, z_front);
        glVertex3f(x_right, y_base, z_front);
        glVertex3f(x_top_right, y_top, z_front);
        glVertex3f(x_top_left, y_top, z_front);
    glEnd();

    // M?t sau rang
    glBegin(GL_QUADS);
        calculateNormal(x_right, y_base, z_back,x_left, y_base, z_back,x_top_left, y_top, z_back);
        glVertex3f(x_right, y_base, z_back);
        glVertex3f(x_left, y_base, z_back);
        glVertex3f(x_top_left, y_top, z_back);
        glVertex3f(x_top_right, y_top, z_back);
    glEnd();

    // M?t trái rang
    glBegin(GL_QUADS);
        calculateNormal(x_left, y_base, z_back, x_left, y_base, z_front,x_top_left, y_top, z_front);
        glVertex3f(x_left, y_base, z_back);
        glVertex3f(x_left, y_base, z_front);
        glVertex3f(x_top_left, y_top, z_front);
        glVertex3f(x_top_left, y_top, z_back);
    glEnd();

    // M?t ph?i rang
    glBegin(GL_QUADS);
        calculateNormal(x_right, y_base, z_front,x_right, y_base, z_back,x_top_right, y_top, z_back);
        glVertex3f(x_right, y_base, z_front);
        glVertex3f(x_right, y_base, z_back);
        glVertex3f(x_top_right, y_top, z_back);
        glVertex3f(x_top_right, y_top, z_front);
    glEnd();

    // M?t trên rang
    glBegin(GL_QUADS);
        calculateNormal(x_top_left, y_top, z_front,x_top_right, y_top, z_front,x_top_right, y_top, z_back);
        glVertex3f(x_top_left, y_top, z_front);
        glVertex3f(x_top_right, y_top, z_front);
        glVertex3f(x_top_right, y_top, z_back);
        glVertex3f(x_top_left, y_top, z_back);
    glEnd();

    // M?t du?i rang (k?t n?i v?i thân thanh rang)
    glBegin(GL_QUADS);
        calculateNormal(x_left, y_base, z_back,x_right, y_base, z_back,x_right, y_base, z_front);
        glVertex3f(x_left, y_base, z_back);
        glVertex3f(x_right, y_base, z_back);
        glVertex3f(x_right, y_base, z_front);
        glVertex3f(x_left, y_base, z_front);
    glEnd();
}

void drawRack(RackProperties rackProps)
{
    glPushMatrix();

    // Thi?t l?p v?t li?u cho thanh rang v?i màu s?c m?i
    GLfloat rackAmbient[] = {rackColor[0]*0.3, rackColor[1]*0.3, rackColor[2]*0.3, 1.0};
    GLfloat rackDiffuse[] = {rackColor[0], rackColor[1], rackColor[2], 1.0};
    GLfloat rackSpecular[] = {0.5, 0.5, 0.5, 1.0};
    float rackShininess = 50.0;

    glMaterialfv(GL_FRONT, GL_AMBIENT, rackAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, rackDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, rackSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, rackShininess);

    // V? thân chính c?a thanh r?ng
    glBegin(GL_QUADS);
        // M?t tr??c (Z d??ng)
        glNormal3f(0.0, 1.0, 0.0);
        glVertex3f(-0.2, 0.0, rackProps.thickness/2);  // ?i?m A
        glVertex3f(rackProps.length +0.3, 0.0, rackProps.thickness/2);  // ?i?m B
        glVertex3f(rackProps.length+0.3, 0.2, rackProps.thickness/2);  // ?i?m C
        glVertex3f(-0.2, 0.2, rackProps.thickness/2);  // ?i?m D

        // M?t sau (Z âm)
        glNormal3f(0.0, -1.0, 0.0);
        glVertex3f(-0.2, 0.2, -rackProps.thickness/2);  // ?i?m E
        glVertex3f(rackProps.length+0.3, 0.2, -rackProps.thickness/2);  // ?i?m F
        glVertex3f(rackProps.length+0.3, 0.0, -rackProps.thickness/2);  // ?i?m G
        glVertex3f(-0.2, 0.0, -rackProps.thickness/2);  // ?i?m H

        // M?t trái
        glNormal3f(-1.0, 0.0, 0.0);
        glVertex3f(-0.2, 0.0, -rackProps.thickness/2);
        glVertex3f(-0.2, 0.0, rackProps.thickness/2);
        glVertex3f(-0.2, 0.2, rackProps.thickness/2);
        glVertex3f(-0.2, 0.2, -rackProps.thickness/2);

        // M?t ph?i
        glNormal3f(1.0, 0.0, 0.0);
        glVertex3f(rackProps.length+0.3, 0.0, -rackProps.thickness/2);
        glVertex3f(rackProps.length+0.3, 0.0, rackProps.thickness/2);
        glVertex3f(rackProps.length+0.3, 0.2, rackProps.thickness/2);
        glVertex3f(rackProps.length+0.3, 0.2, -rackProps.thickness/2);

        // M?t du?i (ph?ng)
        glNormal3f(0.0, 0.0, -1.0);
        glVertex3f(-0.2, 0.0, -rackProps.thickness/2);
        glVertex3f(rackProps.length+0.3, 0.0, -rackProps.thickness/2);
        glVertex3f(rackProps.length+0.3, 0.0, rackProps.thickness/2);
        glVertex3f(-0.2, 0.0, rackProps.thickness/2);

        // M?t trên (ph?ng gi?a các rang)
        glNormal3f(0.0, 0.0, 1.0);
        glVertex3f(-0.2, 0.2, -rackProps.thickness/2);
        glVertex3f(0.5, 0.2, -rackProps.thickness/2);
        glVertex3f(0.5, 0.2, rackProps.thickness/2);
        glVertex3f(-0.2, 0.2, rackProps.thickness/2);
    glEnd();

    glPushMatrix();
    glTranslatef(-0.1, 0.3, -0.5);
    drawCylinderEndBox(0.2, rackProps.thickness, 0.2);
    glTranslatef(8.3, 0.0, -0.5);
    drawCylinderEndBox(0.2, 0.2, rackProps.thickness);
	glPopMatrix();

    // Thi?t l?p v?t li?u cho rang
    GLfloat toothAmbient[] = {0.4, 0.4, 0.2, 1.0};
    GLfloat toothDiffuse[] = {0.9, 0.8, 0.1, 1.0};
    GLfloat toothSpecular[] = {0.7, 0.7, 0.7, 1.0};
    float toothShininess = 60.0;

    glMaterialfv(GL_FRONT, GL_AMBIENT, toothAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, toothDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, toothSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, toothShininess);

    // V? các rang
    for (float x =0.0f; x < rackProps.length; x += rackProps.toothSpacing)
    {
        glPushMatrix();
        glTranslatef(x, 0.2, 0.0); // D?ch chuy?n rang lên trên thân thanh rang
        drawTrapezoidalTooth(0.0, rackProps);
        glPopMatrix();
    }

    glPopMatrix();
}

void drawRotatingComponents()
{

    glTranslatef(-0.5, 0.0, 0.0);
    glRotatef(90.0, 0.0, 0.0, 1.0);
    glTranslatef(0.0, 0.0, 1.0);
    glRotatef(componentsAngleY, 0.0, 0.0, 1.0);

    drawCylinderEndBox(1.8, 0.5, 3.0);

    float cylinderRadius = 0.1;
    float cylinderHeight = 0.6;
    float spacing = 0.3;

    glPushMatrix();
    glTranslatef(0.1, 1.3, -1.7);
    glRotatef(90.0,1.0,0.0,0.0);
    drawSideCylinder(-0.5 + spacing, 0.5, 0.0, 90.0, 0.0, 0.0, 1.0, cylinderRadius, cylinderHeight);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.4, 1.3, -1.1);
    glRotatef(90.0,1.0,0.0,0.0);
    drawSideCylinder(-0.5 + 2 * spacing, 0.5, 0.0, 90.0, 0.0, 0.0, 1.0, cylinderRadius, cylinderHeight);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.9, 1.3, -0.1);
    glRotatef(90.0,1.0,0.0,0.0);
    drawSideCylinder(0.5 - 2 * spacing, 0.5, 0.0, 90.0, 0.0, 0.0, 1.0, cylinderRadius, cylinderHeight);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.3, 1.3, 0.6);
    glRotatef(90.0,1.0,0.0,0.0);
    drawSideCylinder(0.5 - spacing, 0.5, 0.0, 90.0, 0.0, 0.0, 1.0, cylinderRadius, cylinderHeight);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.6,0.0,0.1);
    glRotatef(-90.0,0.0,1.0,0.0);
    // B?t d?u v? h?nh nón c?t b?ng GLU cylinder
    GLUquadric* quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH); // T?o vector pháp tuy?n mu?t
    gluQuadricTexture(quad, GL_TRUE);    // B?t texture (n?u mu?n)

    // V? h?nh tr? (ph?n thân nón c?t)
    gluCylinder(quad, baseRadius, topRadius, height, slices, stacks);

    // V? dáy l?n (h?nh tr?n)
    gluDisk(quad, 0.0, baseRadius, slices, 1);

    // Di chuy?n d?n d?nh và v? dáy nh? (h?nh tr?n)
    glPushMatrix();
    glTranslatef(0.0, 0.0, height);
    gluDisk(quad, 0.0, topRadius, slices, 1);
    glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(2.6, 0.0, 0.1);
    glutSolidSphere(0.35,40,40);
    glPopMatrix();

    glRotatef(-90.0,0.0,1.0,0.0);
    glTranslatef(-0.2,0.0,-2.4);

    if (isRotating || isAccelerating || isDecelerating)
    {
        angleGear1 += currentRotationSpeed * 0.1;
        if (angleGear1 > 360.0) angleGear1 -= 360.0;

        // C?p nh?t t? l? truy?n d?ng d?a trên s? rang hi?n t?i
        angleGear2 = -angleGear1 * (float)gear1Props.numTeeth / gear2Props.numTeeth;
        angleGear3 = -angleGear2 * (float)gear2Props.numTeeth / gear3Props.numTeeth;
        angleGear4 = -angleGear3 * (float)gear3Props.numTeeth / gear4Props.numTeeth;
    }

    // V? bánh rang 1
    glPushMatrix();
    glTranslatef(-1.0, 0.72, 3.0);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glRotatef(angleGear1, 0.0,0.0, 1.0);
    glColor3f(0.2, 0.5, 0.2);
    drawGear(gear1Props, 0);
    glPopMatrix();

    // V? bánh rang 2
    glPushMatrix();
    glTranslatef(-0.4, 0.72, 2.4);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glRotatef(angleGear2, 0.0, 0.0, 1.0);
    glColor3f(0.75, 0.75, 0.75);
    drawGear(gear2Props, 1);
    glPopMatrix();

    // V? bánh rang 3
    glPushMatrix();
    glTranslatef(0.6, 0.72, 2.1);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glRotatef(40.0 + angleGear3, 0.0, 0.0, 1.0);
    glColor3f(0.56, 0.27, 0.97);
    drawGear(gear3Props, 2);
    glPopMatrix();

    // V? bánh rang 4
    glPushMatrix();
    glTranslatef(1.3, 0.72, 3.0);
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glRotatef(7.0 + angleGear4, 0.0, 0.0, 1.0);
    glColor3f(0.5, 0.95, 0.35);
    drawGear(gear4Props, 3);
    glPopMatrix();
}
