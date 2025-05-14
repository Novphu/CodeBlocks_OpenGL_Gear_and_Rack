#include "gear_rack_simulator.h"

int rackTeethCount = 50;

float rackColor[3] = {0.7, 0.7, 0.7};
float componentsAngleY = 0.0;

// Global variables
float cameraAngleX = 0.0;
float cameraAngleY = 0.0;
float cameraX = 0.0;
float cameraY = 0.0;
float cameraZ = 5.0;
float WIDTH = 1200;
float HEIGHT = 900;
float rotationSpeed = 0.2;
float zoomSpeed = 0.05;
int lastX = -1;
int lastY = -1;
bool isDraggingLeft = false;
bool isDraggingRight = false;
bool isDraggingMiddle = false;
float lightPosition[] = {1.0, 1.0, 1.0, 0.0};
float lightColor[] = {1.0, 1.0, 1.0, 1.0};

float rotateangle = 0.0;
float rackPositionX = -2.0;
float rackSpeed = 0.02;
bool rackMovingRight = true;
bool isRotating = false;

// Rotation control
float currentRotationSpeed = 0.0;
float targetRotationSpeed = 0.0;
float acceleration = 0.005;
bool isAccelerating = false;
bool isDecelerating = false;

extern float gearColors[4][3];
// UI properties
float uiPanelWidth = 350;
float uiPanelHeight = 500;
float uiMargin = 20;
bool showUI = true;
bool showGearPanel = false;
bool showRackPanel = false;
int selectedGear = -1;
int selectedProperty = -1;
float gearSpeeds[4] = {1.0, 1.0, 1.0, 1.0};
bool arrowPointingLeft = false;
bool isAnimating = false;
bool isOpening = false;
float panelWidthTarget = 350.0;
float panelWidthCurrent = panelWidthTarget;
float animationSpeed = 20.0;
bool isKeyboardToggle = false;
float globalRotationSpeed = 4.0;
float displaySpeed = 0.0;
bool isStopping = false;
bool isSimulationRunning = false;
bool isButtonPressed = false;
int pressedButton = -1;
float buttonDepressAmount = 0.0;
int pressedGearButton = -1;
int pressedRackButton = -1;
float gearButtonDepress = 0.0;
float rackButtonDepress = 0.0;
bool isFullscreen = false;
bool showHintsPanel = true;
float hintsPanelHeight = 120.0;
bool isSoundPlaying = false;
bool shouldPlaySound = false;
float powerButtonX = 0.0;
float powerButtonY = 0.0;
float powerButtonSize = 25.0;
bool isPowerButtonPressed = false;
float powerButtonDepress = 0.0;
int hoveredGear = -1;
int mouseX = 0, mouseY = 0;
bool showTooltip = true;
bool isSoundEnabled = true;
float soundButtonX = 0.0;
float soundButtonY = 0.0;
float soundButtonSize = 25.0;
bool isSoundButtonPressed = false;
float soundButtonDepress = 0.0;


// Gear properties
GearProperties gear1Props = {20, 0.1, 0.4, 0.05, 0.1, 0.15};
GearProperties gear2Props = {16, 0.1, 0.3, 0.05, 0.1, 0.15};
GearProperties gear3Props = {30, 0.1, 0.6, 0.05, 0.1, 0.15};
GearProperties gear4Props = {20, 0.1, 0.4, 0.05, 0.1, 0.15};

// Rack properties
RackProperties rackProps = {8.0, 0.15, 0.05, 0.15, 0.15, 0.2, 0.02};

// UI properties

std::vector<GearPosition> gearPositions;

// Th�m v? tr� b�nh rang v�o danh s�ch
void initGearPositions()
{
    gearPositions.push_back({-1.0, 0.72, 3.0, gear1Props.outerRadius, 1}); // B�nh rang 1
    gearPositions.push_back({-0.4, 0.72, 2.4, gear2Props.outerRadius, 2}); // B�nh rang 2
    gearPositions.push_back({0.6, 0.72, 2.1, gear3Props.outerRadius, 3});  // B�nh rang 3
    gearPositions.push_back({1.3, 0.72, 3.0, gear4Props.outerRadius, 4});  // B�nh rang 4
}

int checkGearHit(int x, int y)
{
    GLint viewport[4];
    GLdouble modelview[16];
    GLdouble projection[16];
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;

    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);

    winX = (float)x;
    winY = (float)viewport[3] - (float)y;
    glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

    // Ki?m tra t?ng b�nh rang
    for (size_t i = 0; i < gearPositions.size(); ++i)
    {
        GearPosition gear = gearPositions[i];
        float distance = sqrt(pow(posX - gear.x, 2) + pow(posY - gear.y, 2) + pow(posZ - gear.z, 2));
        if (distance <= gear.radius + 0.3)
        {
            return gear.id; // Tr? v? ID c?a b�nh rang b? hit
        }
    }
    return false; // Kh�ng hit b�nh rang n�o
}

void displayHelp()
{
glMatrixMode(GL_PROJECTION);
glPushMatrix();
glLoadIdentity();
gluOrtho2D(0, 800, 0, 600);
glMatrixMode(GL_MODELVIEW);
glPushMatrix();
glLoadIdentity();

// T?t lighting n?u dang b?t
    GLboolean lightingEnabled = glIsEnabled(GL_LIGHTING);
    if (lightingEnabled) glDisable(GL_LIGHTING);


    glColor3f(1.0, 0.0, 0.0);

    glRasterPos2f(10, 580); // D?ng m?i
    const char* helpText3 = "UI Panel: U/u - Toggle UI ";
    for (const char* c = helpText3; *c != '\0'; c++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }

    glRasterPos2f(10, 560); // Th�m d?ng m?i
    const char* helpText4 = "Fullscreen: F/f - Toggle fullscreen mode";
    for (const char* c = helpText4; *c != '\0'; c++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }

    glRasterPos2f(10, 540); // Th�m d?ng m?i
    const char* helpText5 = (showTooltip) ? "Tooltip: T/t - turn off Tooltip" : "Tooltip: T/t - turn on Tooltip";
    for (const char* c = helpText5; *c != '\0'; c++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }

     // B?t l?i lighting n?u ban d?u dang b?t
    if (lightingEnabled) glEnable(GL_LIGHTING);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // V? 3D
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)WIDTH/HEIGHT , 0.1, 100.0); // Thi?t l?p ph�p chi?u ph?i c?nh
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); // ??t ma tr?n v? ma tr?n don v?

    gluLookAt(cameraX, cameraY, cameraZ,0.0, 0.0, 0.0,0.0, 1.0, 0.0);

    glRotatef(cameraAngleX, 1.0, 0.0, 0.0);
    glRotatef(cameraAngleY, 0.0, 1.0, 0.0);

    // V? m?t ??t
    drawGround();

    glPushMatrix();
    glColor3f(1.0, 1.0, 1.0);
    drawBox();

        glPushMatrix();
        glTranslatef(0.5, 0.0, 0.0);
        glRotatef(90.0, 0.0 ,0.0, 1.0);
        glColor3f(0.0, 0.0, 1.0);
        drawCylinder(0.2, 1.0, 200, 200);

            glPushMatrix();
            glTranslatef(0.0, 0.0, 0.0);
            glColor3f(0.5, 0.5, 1.0);
            drawCircle(0.2, 200);
            glPopMatrix();
        glPopMatrix();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(rackPositionX - 2.23, -2.0, 2.2);
    drawRack(rackProps);
    drawRackBevel(rackProps); // V? ph?n v�t m�p
    glPopMatrix();

    drawRotatingComponents();

    displayHelp();

    // V? 2D
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, HEIGHT, 0);  // Thi?t l?p v? 2D
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST); // T?t ki?m tra d? s�u

    if (showTooltip)
    { // Th�m ?i?u ki?n n�y
        drawGearTooltip();
    }

    drawUI(); // V? giao di?n panel

    glEnable(GL_DEPTH_TEST); // B?t ki?m tra d? s�u
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix(); // Kh�i ph?c tr?ng th�i ma tr?n

    glutSwapBuffers();
    glutPostRedisplay();
}

void passiveMotion(int x, int y)
{
    mouseX = x;
    mouseY = y;

    int hitGearId = checkGearHit(x, y);
    hoveredGear = (hitGearId != -1) ? (hitGearId - 1) : -1;
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
    // X? l? hi?u ?ng ?n n�t

    float controlButtonY = uiMargin + 40 + (showRackPanel ? 290 : 190);
    if (button == GLUT_LEFT_BUTTON)
    {
    	if (!isAnimating)
    	{
            // Click v�o m?i t�n m? panel (khi panel d�ng)
            if (!showUI && x >= 10 && x <= 30 && y >= HEIGHT/2 - 10 && y <= HEIGHT/2 + 10)
            {
            	playClickSound();  // Th�m d?ng n�y
                isAnimating = true;
                isOpening = true;
                panelWidthCurrent = 0;
            }
            // Click v�o m?i t�n d�ng panel (khi panel m?)
            else if (showUI && x >= panelWidthCurrent - 20 && x <= panelWidthCurrent &&
                     y >= HEIGHT/2 - 10 && y <= HEIGHT/2 + 10)
            {
                playClickSound();  // Th�m d?ng n�y
                isAnimating = true;
                isOpening = false;
                panelWidthCurrent = panelWidthTarget;
            }
        }

        if (state == GLUT_DOWN)
        {
            // Ki?m tra click trong v�ng UI
            if (showUI && x < panelWidthCurrent)
            {
                lastX = x;
                lastY = y;

                // Ki?m tra click v�o n�t �m thanh
                if (showUI && x >= soundButtonX - soundButtonSize && x <= soundButtonX + soundButtonSize  &&
                    y >= soundButtonY - soundButtonSize+9 && y <= soundButtonY + soundButtonSize +10)
                {
                    playClickSound();
                    isSoundButtonPressed = true;
                    return;
                }

                if (showUI && x >= powerButtonX - powerButtonSize -275 && x <= powerButtonX + powerButtonSize -265 &&
                    y >= powerButtonY - powerButtonSize +10&& y <= powerButtonY + powerButtonSize +15)
                {
                    playClickSound();
                    isPowerButtonPressed = true;
                    exit(0);
                    return;
                }

                // X? l? click n�t toggle hints panel
                if (x >= uiMargin +60  && x <= uiMargin + 230 &&y >= controlButtonY +330 && y <= controlButtonY + 370)
                {
                    playClickSound();  // Th�m d?ng n�y
                    isButtonPressed = true;
                    pressedButton = 2;
                    return;
                }

                // ===== X? l? click n�t START/STOP =====
                if (x >= uiMargin  && x <= uiMargin + 120 &&y >= controlButtonY+280 && y <= controlButtonY + 320)
                {
                    playClickSound();  // Th�m d?ng n�y
                    isButtonPressed = true;
                    pressedButton = 0;
                    return;
                }

                // ===== X? l? click n�t RESET =====
                if (x >= uiMargin + 160 && x <= uiMargin + 290 &&y >= controlButtonY + 280&& y <= controlButtonY + 320)
                {
                    playClickSound();  // Th�m d?ng n�y
                    isButtonPressed = true;
                    pressedButton = 1;
                    return;
                }

                // ===== X? l? click n�t b�nh r?ng =====
                int buttonsPerRow = 2;
                float buttonWidth = 80;
                float buttonHeight = 25;
                float verticalSpacing = 30;
                float horizontalSpacing = 80;

                for (int i = 0; i < 4; i++)
                {
                    int row = i / buttonsPerRow;
                    int col = i % buttonsPerRow;
                    float buttonX = uiMargin + col * (buttonWidth + horizontalSpacing);
                    float buttonY = uiMargin + 40 + row * (buttonHeight + verticalSpacing);

                    if (x >= buttonX && x <= buttonX + buttonWidth+50 &&y >= buttonY+30 && y <= buttonY + buttonHeight+50)
                    {
                        playClickSound();  // Th�m d?ng n�y
                        pressedGearButton = i;
                        return;
                    }
                }

                    // ===== X? l? click n�t thanh r?ng =====
                float rackButtonY = uiMargin + 40 + 4 * 30;
                if (x >= uiMargin +60 && x <= uiMargin + 230 &&y >= rackButtonY +30 && y <= rackButtonY + 65)
                {
                    playClickSound();  // Th�m d?ng n�y
                    pressedRackButton = 0;
                    return;
                }


                // ===== X? l? thanh tr??t thanh r?ng =====
                if (showRackPanel)
                {
                    float rackPanelY = rackButtonY + 35;

                    // Thanh tr??t s? r?ng thanh r?ng
                    if (y >= rackPanelY + 105 && y <= rackPanelY + 125)
                    {
                        selectedProperty = 10;
                        float sliderX = uiMargin + 10;
                        float sliderWidth = panelWidthCurrent - 60;
                        float percent = (x - sliderX) / sliderWidth;
                        percent = std::max(0.0f, std::min(1.0f, percent));
                        rackTeethCount = MIN_RACK_TEETH + percent * (MAX_RACK_TEETH - MIN_RACK_TEETH);
                        rackProps.toothSpacing = rackProps.length / rackTeethCount;
                        return;
                    }

                    // Thanh tr??t ?? d�y thanh r?ng
                    if (y >= rackPanelY + 135 && y <= rackPanelY + 155)
                    {
                        selectedProperty = 11;
                        float sliderX = uiMargin + 10;
                        float sliderWidth = panelWidthCurrent - 60;
                        float percent = (x - sliderX) / sliderWidth;
                        percent = std::max(0.0f, std::min(1.0f, percent));
                        rackProps.thickness = 0.05 + percent * (0.5 - 0.05);
                        return;
                    }

                                     // Thanh tru?t m�u d?
                    if (y >= rackPanelY + 165 && y <= rackPanelY + 185)
                    {
                        selectedProperty = 12; // M? ri�ng cho m�u d? thanh rang
                        float sliderX = uiMargin + 10;
                        float sliderWidth = uiPanelWidth - 60;
                        float percent = (x - sliderX) / sliderWidth;
                        percent = std::max(0.0f, std::min(1.0f, percent));
                        rackColor[0] = percent;
                        return;
                    }

                    // Thanh tru?t m�u xanh l�
                    if (y >= rackPanelY + 195 && y <= rackPanelY + 215)
                    {
                        selectedProperty = 13; // M? ri�ng cho m�u xanh l� thanh rang
                        float sliderX = uiMargin + 10;
                        float sliderWidth = uiPanelWidth - 60;
                        float percent = (x - sliderX) / sliderWidth;
                        percent = std::max(0.0f, std::min(1.0f, percent));
                        rackColor[1] = percent;
                        return;
                    }

                    // Thanh tru?t m�u xanh duong
                    if (y >= rackPanelY + 225 && y <= rackPanelY + 245)
                    {
                        selectedProperty = 14; // M? ri�ng cho m�u xanh duong thanh rang
                        float sliderX = uiMargin + 10;
                        float sliderWidth = uiPanelWidth - 60;
                        float percent = (x - sliderX) / sliderWidth;
                        percent = std::max(0.0f, std::min(1.0f, percent));
                        rackColor[2] = percent;
                        return;
                    }
                }

                // ========== X? L? THANH TRU?T B�NH RANG ==========
                if (selectedGear >= 0)
                {
                    float gearPanelY = rackButtonY + (showRackPanel ? 135 : 35);

                    // L?y th�ng tin b�nh rang hi?n t?i
                    GearProperties* currentGear = NULL;
                    switch(selectedGear)
                    {
                        case 0: currentGear = &gear1Props; break;
                        case 1: currentGear = &gear2Props; break;
                        case 2: currentGear = &gear3Props; break;
                        case 3: currentGear = &gear4Props; break;
                    }

                    // Thay ph?n x? l? thanh tru?t t?c d? b?ng:
                    if (y >= gearPanelY + 105 && y <= gearPanelY + 125 && isRotating)
                    {
                        selectedProperty = 5; // T?c d?
                        float sliderX = uiMargin + 10;
                        float sliderWidth = uiPanelWidth - 60;
                        float percent = (x - sliderX) / sliderWidth;
                        percent = std::max(0.0f, std::min(1.0f, percent)); // Gi?i h?n 0-1
                        globalRotationSpeed = percent * 4.0; // T?c d? t?i da ch�nh x�c l� 4.0
                        return;
                    }

                    // Thanh tru?t s? rang
                    if (y >= gearPanelY + 135 && y <= gearPanelY + 155)
                    {
                        selectedProperty = 0;
                        float sliderX = uiMargin + 10;
                        float sliderWidth = uiPanelWidth - 60;
                        float percent = (x - sliderX) / sliderWidth;
                        percent = std::max(0.0f, std::min(1.0f, percent));
                        currentGear->numTeeth = 15 + percent * (35 - 15);
                        return;
                    }

                    // Thanh tru?t d? d�y (0.05-0.3)
                    if (y >= gearPanelY + 165 && y <= gearPanelY + 185)
                    {
                        selectedProperty = 1;
                        float sliderX = uiMargin + 10;
                        float sliderWidth = uiPanelWidth - 60;
                        float percent = (x - sliderX) / sliderWidth;
                        percent = std::max(0.0f, std::min(1.0f, percent));
                        currentGear->thickness = 0.05 + percent * (0.3 - 0.05);
                        return;
                    }

                    // Thanh tru?t m�u d?
                    if (y >= gearPanelY + 195 && y <= gearPanelY + 215)
                    {
                        selectedProperty = 2;
                        float sliderX = uiMargin + 10;
                        float sliderWidth = uiPanelWidth - 60;
                        float percent = (x - sliderX) / sliderWidth;
                        percent = std::max(0.0f, std::min(1.0f, percent));
                        gearColors[selectedGear][0] = percent;
                        return;
                    }

                    // Thanh tru?t m�u xanh l�
                    if (y >= gearPanelY + 225 && y <= gearPanelY + 245)
                    {
                        selectedProperty = 3;
                        float sliderX = uiMargin + 10;
                        float sliderWidth = uiPanelWidth - 60;
                        float percent = (x - sliderX) / sliderWidth;
                        percent = std::max(0.0f, std::min(1.0f, percent));
                        gearColors[selectedGear][1] = percent;
                        return;
                    }

                    // Thanh tru?t m�u xanh duong
                    if (y >= gearPanelY + 255 && y <= gearPanelY + 275)
                    {
                        selectedProperty = 4;
                        float sliderX = uiMargin + 10;
                        float sliderWidth = uiPanelWidth - 60;
                        float percent = (x - sliderX) / sliderWidth;
                        percent = std::max(0.0f, std::min(1.0f, percent));
                        gearColors[selectedGear][2] = percent;
                        return;
                    }
                }
            }
            else if (checkGearHit(x, y))
            {
                // X? l? click v�o b�nh rang
                if (isRotating)
                {
                    targetRotationSpeed = 0.0;
                    isDecelerating = true;
                    isAccelerating = false;
                    isStopping = true;
                    isSimulationRunning = false;

                // ??m b?o d?ng �m thanh ngay khi nh?n stop
                if (isSoundPlaying)
                {
                    PlaySound(NULL, NULL, 0);
                    isSoundPlaying = false;
                }

                }
                else
                {
                    isRotating = true;
                    targetRotationSpeed = 4.0;
                    isAccelerating = true;
                    isDecelerating = false;
                    isStopping = false;
                    globalRotationSpeed = 4.0;
                    isSimulationRunning = true; // Th�m d?ng n�
                }
            }
            else
            {
                // B?t ??u k�o camera
                lastX = x;
                lastY = y;
                isDraggingLeft = true;
            }
        }
        else if (state == GLUT_UP)
        {
            // K?t th�c c�c tr?ng th�i nh?n
            isButtonPressed = false;
            isDraggingLeft = false;
            selectedProperty = -1;

            if (pressedButton == 2)
            {
                showHintsPanel = !showHintsPanel;
            }

            // X? l? th? n�t �m thanh
            if (isSoundButtonPressed)
            {
                isSoundEnabled = !isSoundEnabled;
                if (!isSoundEnabled && isSoundPlaying)
                {
                    PlaySound(NULL, NULL, 0); // T?t �m thanh n?u ?ang ph�t
                    isSoundPlaying = false;
                }
                isSoundButtonPressed = false;
            }
        }
    }
    else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        // Reset tr?ng th�i khi th? chu?t
        pressedGearButton = -1;
        pressedRackButton = -1;

        // X? l? th? n�t b?m UI
        if (isButtonPressed)
        {
            isButtonPressed = false;

            // Th?c hi?n h�nh d?ng tuong ?ng v?i n�t du?c nh?n
            if (pressedButton == 0)
            { // N�t Start/Stop
                isSimulationRunning = !isSimulationRunning;
                if (isSimulationRunning)
                {
                    isRotating = true;
                    targetRotationSpeed = globalRotationSpeed;
                    isAccelerating = true;
                    isDecelerating = false;
                }
                else
                {
                    targetRotationSpeed = 0.0;
                    isDecelerating = true;
                    isAccelerating = false;
                }
            }
            else if (pressedButton == 1)
            { // N�t Reset
                resetSimulation();
            }
        }

        // Reset tr?ng th�i k�o camera
        isDraggingLeft = false;

        // Reset tr?ng th�i ch?n thu?c t�nh
        selectedProperty = -1;

        // Reset tr?ng th�i n�t b?m
        pressedButton = -1;

        // C?p nh?t l?i m�n h?nh
        glutPostRedisplay();
    }
    else if (button == GLUT_RIGHT_BUTTON)
    {
        // X? l? zoom camera
        if (state == GLUT_DOWN)
        {
            lastY = y;
            isDraggingRight = true;
        }
        else if (state == GLUT_UP)
        {
            isDraggingRight = false;
        }
    }
    else if (button == GLUT_MIDDLE_BUTTON)
    {
        // X? l? xoay th�nh ph?n
        if (state == GLUT_DOWN)
        {
            lastX = x;
            isDraggingMiddle = true;
        }
        else if (state == GLUT_UP)
        {
            isDraggingMiddle = false;
        }
    }

     // Ki?m tra hover khi di chu?t (kh�ng nh?n)
    if (button == -1 && state == GLUT_LEFT)
    {
        int hitGearId = checkGearHit(x, y);
        hoveredGear = (hitGearId != -1) ? (hitGearId - 1) : -1;
        glutPostRedisplay();
    }

    glutPostRedisplay();
}

void motion(int x, int y)
{

    mouseX = x;
    mouseY = y;
    if (isDraggingLeft && selectedProperty >= 0)
    {
        float sliderX = uiMargin + 10;
        float sliderWidth = panelWidthCurrent - 20;
        float percent = (x - sliderX) / sliderWidth;

        // Gi?i h?n t?c ?? t? 0.1 ??n 1.0 (thay v? 2.0 nh? tr??c)
        percent = std::max(0.1f, std::min(1.0f, percent * 1.0f));  // Nh�n v?i 1.0 thay v? 2.0

        globalRotationSpeed = 0.0+ percent*4.0;

        // N?u ?ang quay, c?p nh?t t?c ?? m?c ti�u
        if (isRotating)
        {
            targetRotationSpeed = percent;  // S? d?ng percent tr?c ti?p v? ?? gi?i h?n
            isAccelerating = true;
            isDecelerating = false;
        }
        // ========== X? L? THANH RANG ==========
        if (selectedProperty == 10)
        {
             // S? rang thanh rang
            rackTeethCount = 15 + percent * (35 - 15);
            rackProps.toothSpacing = rackProps.length / rackTeethCount;
        }
        else if (selectedProperty == 11)
        {
             // ?? d�y thanh rang
            rackProps.thickness = 0.05f + percent * (0.5f - 0.05f);
        }
        else if (selectedProperty >= 12 && selectedProperty <= 14)
        {
            // X? l? k�o thanh tru?t m�u thanh rang
            float sliderX = uiMargin + 10;
            float sliderWidth = uiPanelWidth - 20;
            float percent = (x - sliderX) / sliderWidth;
            percent = std::max(0.0f, std::min(1.0f, percent));

            switch(selectedProperty)
            {
                case 12: // M�u d?
                    rackColor[0] = percent;
                    break;
                case 13: // M�u xanh l�
                    rackColor[1] = percent;
                    break;
                case 14: // M�u xanh duong
                    rackColor[2] = percent;
                    break;
            }

            glutPostRedisplay();
        }

        // ========== X? L? B�NH RANG ==========
        else if (selectedGear >= 0)
        {
            GearProperties* currentGear = NULL;
            switch(selectedGear)
            {
                case 0: currentGear = &gear1Props; break;
                case 1: currentGear = &gear2Props; break;
                case 2: currentGear = &gear3Props; break;
                case 3: currentGear = &gear4Props; break;
            }

            switch(selectedProperty)
            {
                case 0: // S? rang
                    currentGear->numTeeth = 10 + percent * (50 - 10);
                    break;
                case 1: // ?? d�y
                    currentGear->thickness = 0.05 + percent * (0.3 - 0.05);
                    break;
                case 2: // M�u d?
                    gearColors[selectedGear][0] = percent;
                    break;
                case 3: // M�u xanh l�
                    gearColors[selectedGear][1] = percent;
                    break;
                case 4: // M�u xanh duong
                    gearColors[selectedGear][2] = percent;
                    break;
                // Th�m v�o ph?n x? l? thanh tru?t b�nh rang
				case 5:
					if (isRotating)
                    {
                        float sliderX = uiMargin + 10;
                        float sliderWidth = uiPanelWidth - 20;
                        float percent = (x - sliderX) / sliderWidth;
                        percent = std::max(0.0f, std::min(1.0f, percent));
                        globalRotationSpeed = percent * 4.0;
                    }
                    break;
            }
        }

        glutPostRedisplay();
    }
    else if (isDraggingLeft)
    {
        // X? l? xoay camera
        int deltaX = x - lastX;
        int deltaY = y - lastY;
        cameraAngleY += deltaX * rotationSpeed;
        cameraAngleX += deltaY * rotationSpeed;

        if(cameraAngleX <= -5.0) cameraAngleX= -5.0;
        if(cameraAngleX >= 170.0) cameraAngleX= 170.0;
        lastX = x;
        lastY = y;
        glutPostRedisplay();
    }
    else if (isDraggingRight)
    {
        // X? l? zoom
        int deltaY = y - lastY;
        cameraZ += deltaY * zoomSpeed;
        cameraZ = std::max(5.0f, std::min(12.0f, cameraZ));
        lastY = y;
        glutPostRedisplay();
    }
    else if (isDraggingMiddle)
    {
        // X? l? xoay th�nh ph?n
        int deltaX = x - lastX;
        componentsAngleY -= deltaX * rotationSpeed;
        componentsAngleY = std::max(-45.0f, std::min(40.0f, componentsAngleY));
        lastX = x;
        glutPostRedisplay();
    }
}

void keyboard(unsigned char key, int x, int y)
{
    switch(key)
    {
    	  case ' ': // Ph�m space ?? start/stop
            playClickSound();  // Th�m d?ng n�y
            if (!isButtonPressed && pressedButton == -1)
            {
                isButtonPressed = true;
                pressedButton = 0;
                targetRotationSpeed = 4.0;
                globalRotationSpeed = 4.0;
            }
            break;
        case 'r': // Ph�m 'r' ?? reset
        case 'R':
        	playClickSound();  // Th�m d?ng n�y
            if (!isButtonPressed && pressedButton == -1)
            {
                isButtonPressed = true;
                pressedButton = 1;
            }
            break;
        case 'D':
        case 'd':
            componentsAngleY -= 5.0;
            if(componentsAngleY <= -45.0) componentsAngleY = -45.0;
            break;
        case 'A':
        case 'a':
            componentsAngleY += 5.0;
            if(componentsAngleY >= 40.0) componentsAngleY = 40.0;
            break;
        case 'u':
        case 'U':
            isKeyboardToggle = true; // ?�nh d?u l� dang toggle b?ng b�n ph�m
            if (!isAnimating)
            {
                isAnimating = true;
                isOpening = !showUI; // N?u panel dang d�ng th� m?, v� ngu?c l?i
                if (isOpening)
                {
                    panelWidthCurrent = 0;
                }
                else
                {
                    panelWidthCurrent = panelWidthTarget;
                }
            }
            break;
        case 'f': // Ph�m 'f' ?? b?t/t?t to�n m�n h?nh
        case 'F':
            isFullscreen = !isFullscreen;
            if (isFullscreen)
            {
                glutFullScreen();
            }
            else
            {
                glutReshapeWindow(800, 600); // K�ch th??c c?a s? ban ??u
                glutPositionWindow(100, 100); // V? tr� c?a s? khi tho�t fullscreen
            }
            break;
        case 't': // Ph�m 't' ?? toggle tooltip
        case 'T':
            showTooltip = !showTooltip;
            break;
        case 'm': // Ph�m 'm' �? toggle �m thanh
        case 'M':
            playClickSound();  // Ph�t �m thanh click
            isSoundEnabled = !isSoundEnabled; // �?o tr?ng th�i �m thanh

            // N?u �ang t?t �m thanh v� c� �m thanh �ang ph�t th? d?ng l?i
            if (!isSoundEnabled && isSoundPlaying)
            {
                PlaySound(NULL, NULL, 0);
                isSoundPlaying = false;
            }
            break;
        case 27:
            exit(0);
            break;
    }

    glutPostRedisplay();
}

void init()
{
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    // T�nh to�n t? l? s? rang ban d?u
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);

    float materialAmbient[] = { 0.2, 0.2, 0.2, 1.0 };
    float materialDiffuse[] = { 0.8, 0.8, 0.8, 1.0 };
    float materialSpecular[] = { 0.5, 0.5, 0.5, 1.0 };
    float materialShininess = 32.0;

    glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
    glMaterialf(GL_FRONT,GL_SHININESS, materialShininess);

    initGearPositions();

    rackTeethCount = 50;
    rackProps.toothSpacing = rackProps.length / rackTeethCount;

    // ??m b?o panel m? khi kh?i d?ng
    panelWidthCurrent = panelWidthTarget;
    showUI = true;
}

void reshape(int w, int h)
{
    WIDTH = w;
    HEIGHT = h;
    uiPanelHeight = HEIGHT * 1.0; // C?p nh?t chi?u cao c?a panel

    if(h == 0) h = 1; // Ngan ch?n l?i chia cho 0
    glViewport(0, 0, w, h); // Thi?t l?p v�ng b�n trong c?a s? du?c v? l�n
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (float)w/h, 0.1, 100); // Thi?t l?p ph�p chi?u ph?i c?nh
    glMatrixMode(GL_MODELVIEW);
}

void update(int value)
{

    static int lastTime = glutGet(GLUT_ELAPSED_TIME);
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = (currentTime - lastTime) / 1000.0; // Th?i gian th?c gi?a c�c frame
    lastTime = currentTime;

    // Gi?i h?n deltaTime �? tr�nh hi?u ?ng "b�ng n?" khi ch��ng tr?nh b? treo t?m th?i
    if (deltaTime > 0.1) deltaTime = 0.1;

    updateRotationSpeed(deltaTime); // Th�m tham s? deltaTime
    updateRackPosition(deltaTime);
    controlSound();

    // X? l? animation panel
    if (isAnimating)
    {
        if (isOpening)
        {
            panelWidthCurrent = lerp(panelWidthCurrent, panelWidthTarget, 0.2);
            if (panelWidthTarget - panelWidthCurrent < 1.0)
            {
                panelWidthCurrent = panelWidthTarget;
                isAnimating = false;
                showUI = true;
                if (isKeyboardToggle)
                {
                    isKeyboardToggle = false;
                }
            }
        }
        else
        {
            panelWidthCurrent = lerp(panelWidthCurrent, 0.0, 0.2);
            if (panelWidthCurrent < 1.0)
            {
                panelWidthCurrent = 0;
                isAnimating = false;
                showUI = false;
                if (isKeyboardToggle)
                {
                    isKeyboardToggle = false;
                }
            }
        }

        glutPostRedisplay();
    }

    // X? l? hi?u ?ng n�t ?n xu?ng
    if (isButtonPressed)
        {
            buttonDepressAmount += 1.0;
            if (buttonDepressAmount >= MAX_DEPRESS)
            {
                buttonDepressAmount = MAX_DEPRESS;

                // Th?c hi?n h�nh ??ng sau khi ?n ho�n to�n
                if (pressedButton == 0)
                {
                    isSimulationRunning = !isSimulationRunning;
                    if (isSimulationRunning)
                    {
                        isRotating = true;
                        targetRotationSpeed = globalRotationSpeed;
                        isAccelerating = true;
                        isDecelerating = false;
                    }
                    else
                    {
                        targetRotationSpeed = 0.0;
                        isDecelerating = true;
                        isAccelerating = false;
                    }
                }
                else if (pressedButton == 1)
                {
                    resetSimulation();
                }

            // B?t ??u hi?u ?ng tr? l?i
                isButtonPressed = false;
            }
    }
    else if (buttonDepressAmount > 0)
    {
        buttonDepressAmount -= 1.5; // T?c ?? tr? l?i nhanh h?n
        if (buttonDepressAmount <= 0)
        {
            buttonDepressAmount = 0.0;
            pressedButton = -1;
        }
    }

    // X? l? hi?u ?ng n�t b�nh r?ng
    if (pressedGearButton >= 0)
    {
        gearButtonDepress += 1.0;
        if (gearButtonDepress >= MAX_DEPRESS)
        {
            gearButtonDepress = MAX_DEPRESS;
            // Th?c hi?n h�nh ??ng khi nh?n ho�n to�n
            if (selectedGear == pressedGearButton)
            {
                showGearPanel = !showGearPanel;
            }
            else
            {
                selectedGear = pressedGearButton;
                showGearPanel = true;
            }

            showRackPanel = false;
            selectedProperty = -1;
            pressedGearButton = -1;
        }
    }
    else if (gearButtonDepress > 0)
    {
        gearButtonDepress -= 1.5;
        if (gearButtonDepress <= 0)
        {
            gearButtonDepress = 0.0;
        }
    }

    // X? l? hi?u ?ng n�t thanh r?ng
    if (pressedRackButton >= 0)
    {
        rackButtonDepress += 1.0;
        if (rackButtonDepress >= MAX_DEPRESS)
        {
            rackButtonDepress = MAX_DEPRESS;
            // Th?c hi?n h�nh ??ng khi nh?n ho�n to�n
            showRackPanel = !showRackPanel;
            selectedGear = -1;
            selectedProperty = -1;
            pressedRackButton = -1;
        }
    }
    else if (rackButtonDepress > 0)
    {
        rackButtonDepress -= 1.5;
        if (rackButtonDepress <= 0)
        {
            rackButtonDepress = 0.0;
        }
    }

    // X? l? hi?u ?ng n�t ngu?n
    if (isPowerButtonPressed)
    {
        powerButtonDepress += 0.5;
        if (powerButtonDepress >= MAX_POWER_DEPRESS)
        {
            powerButtonDepress = MAX_POWER_DEPRESS;
        }
    }
    else if (powerButtonDepress > 0)
    {
        powerButtonDepress -= 1.0;
        if (powerButtonDepress <= 0)
        {
            powerButtonDepress = 0.0;
        }
    }

// X? l? hi?u ?ng n�t �m thanh
    if (isSoundButtonPressed)
    {
        soundButtonDepress += 0.5;
        if (soundButtonDepress >= MAX_SOUND_DEPRESS)
        {
            soundButtonDepress = MAX_SOUND_DEPRESS;
        }
    }
    else if (soundButtonDepress > 0)
    {
        soundButtonDepress -= 1.0;
        if (soundButtonDepress <= 0)
        {
            soundButtonDepress = 0.0;
        }
    }

    glutTimerFunc(16, update, 0);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("RACK AND GEAR");
    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutPassiveMotionFunc(passiveMotion);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}
