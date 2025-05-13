#include "gear_rack_simulator.h"

extern float WIDTH;
extern float HEIGHT ;

// UI properties
extern float globalRotationSpeed;
extern float displaySpeed;
extern bool isStopping ;
extern bool isSimulationRunning ;
extern bool isSoundPlaying;
extern int hoveredGear;
extern int mouseX ;
extern int mouseY ;
extern bool showTooltip ;
extern bool isSoundEnabled;


void drawGearTooltip() {
    if (hoveredGear == -1 || !showTooltip) return; // Thêm ?i?u ki?n !showTooltip

    const char* gearNames[] = {"Gear 1", "Gear 2", "Gear 3", "Gear 4"};

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, HEIGHT, 0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);

    // V? trí tooltip (theo v? trí chu?t + offset ?? không che chu?t)
    float x = mouseX + 15;
    float y = mouseY + 15;

    // ??m b?o tooltip không v??t ra kh?i màn h?nh
    float tooltipWidth = 100;
    float tooltipHeight = 30;

    if (x + tooltipWidth > WIDTH) {
        x = WIDTH - tooltipWidth;
    }
    if (y + tooltipHeight > HEIGHT) {
        y = HEIGHT - tooltipHeight;
    }

    // N?n tooltip
    glColor4f(0.3, 0.1f, 0.65f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + tooltipWidth, y);
    glVertex2f(x + tooltipWidth, y + tooltipHeight);
    glVertex2f(x, y + tooltipHeight);
    glEnd();

    // Vi?n tooltip
    glColor3f(0.5f, 0.5f, 0.5f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + tooltipWidth, y);
    glVertex2f(x + tooltipWidth, y + tooltipHeight);
    glVertex2f(x, y + tooltipHeight);
    glEnd();

    // Ch?
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x + 30, y + 20);
    for (const char* c = gearNames[hoveredGear]; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }

    glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

float lerp(float a, float b, float f) {
    return a + f * (b - a);
}

bool isPointInRect(float x, float y, float rectX, float rectY, float rectW, float rectH)
{
    return x >= rectX && x <= rectX + rectW && y >= rectY && y <= rectY + rectH;
}
