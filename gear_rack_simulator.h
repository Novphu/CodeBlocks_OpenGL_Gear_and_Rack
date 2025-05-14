#ifndef GEAR_RACK_SIMULATOR_H
#define GEAR_RACK_SIMULATOR_H

#include <GL/glut.h>
#include <GL/gl.h>
#include <cmath>
#include <vector>
#include <cstdio>
#include <string>
#include <cstring>
#include <windows.h>
#include <mmsystem.h>

// Constants
const float PI = 3.14159265;
const int MIN_RACK_TEETH = 45;
const int MAX_RACK_TEETH = 65;
const float DEFAULT_ROTATION_SPEED = 4.0;
const float SPEED_LERP_FACTOR = 0.05;
const float MAX_DEPRESS = 5.0;
const float MAX_POWER_DEPRESS = 3.0;
const float MAX_SOUND_DEPRESS = 3.0;
const float bevelAmount = 0.02;

// Structs
struct GearProperties
{
    int numTeeth;
    float innerRadius;
    float outerRadius;
    float toothWidthTop;
    float toothHeight;
    float thickness;
};

struct RackProperties
{
    float length;
    float toothWidthBase;
    float toothWidthTop;
    float toothHeight;
    float toothSpacing;
    float thickness;
    float bevelSize;
};

struct GearPosition
{
    float x, y, z;
    float radius;
    int id;
};

// Function declarations
void init();
void display();
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void passiveMotion(int x, int y);
void keyboard(unsigned char key, int x, int y);
void update(int value);
void resetSimulation();
void controlSound();
void playClickSound();

// Drawing functions
void drawGear(GearProperties props, int gearIndex);
void drawBox();
void drawGround();
void drawCylinderEndBox(float width, float height, float depth);
void drawCylinder(float radius, float height, int slices, int stacks);
void drawCircle(float radius, int slices);
void drawSideCylinder(float offsetX, float offsetY, float offsetZ, float rotationAngle,
                     float rotationX, float rotationY, float rotationZ, float radius, float height);
void drawRack(RackProperties rackProps);
void drawRackBevel(RackProperties rackProps);
void drawTrapezoidalTooth(float x, RackProperties rackProps);
void drawRotatingComponents();
void drawUI();
void drawHelp();

// UI drawing functions
void drawSlider(float x, float y, float width, float value, const char* label, float displayValue);
void drawColorSlider(float x, float y, float width, float value, const char* label);
void drawArrow(float x, float y, float size, bool pointingLeft);
void drawPowerButton(float x, float y, float size, bool pressed);
void drawSoundButton(float x, float y, float size, bool pressed, bool soundEnabled);
void drawGearTooltip();

// Helper functions
float lerp(float a, float b, float f);
bool isPointInRect(float x, float y, float rectX, float rectY, float rectW, float rectH);
void calculateNormal(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3);
void drawBeveledEdge(float x1, float y1, float z1, float x2, float y2, float z2,
                    float x3, float y3, float z3, float x4, float y4, float z4);
void drawTooth(float angle, float nextAngle, float outerRadiusBase, float toothHeight,
              float toothWidthTop, float thickness, float bevelAmount);

// Update functions
void updateRotationSpeed(float deltaTime);
void updateRackPosition(float deltaTime);

#endif // GEAR_RACK_SIMULATOR_H
