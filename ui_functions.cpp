#include "gear_rack_simulator.h"

extern float WIDTH ;
extern float HEIGHT;

extern GearProperties gear1Props;
extern GearProperties gear2Props;
extern GearProperties gear3Props;
extern GearProperties gear4Props;
extern RackProperties rackProps;
extern float rackColor[3];
extern int rackTeethCount;

extern bool isRotating;

extern float gearColors[4][3] ;

// UI properties
extern float uiPanelWidth ;
extern float uiPanelHeight ;
extern float uiMargin ;
extern bool showUI ;
extern bool showGearPanel ;
extern bool showRackPanel;
extern int selectedGear;
extern int selectedProperty ;
extern float gearSpeeds[4] ;
extern bool arrowPointingLeft ;
extern bool isAnimating ;
extern bool isOpening ;
extern float panelWidthTarget ;
extern float panelWidthCurrent;
extern float animationSpeed ;
extern bool isKeyboardToggle ;
extern float globalRotationSpeed ;
extern float displaySpeed;
extern bool isStopping ;
extern bool isSimulationRunning ;
extern bool isButtonPressed ;
extern int pressedButton ;
extern float buttonDepressAmount ;
extern int pressedGearButton;
extern int pressedRackButton ;
extern float gearButtonDepress;
extern float rackButtonDepress ;
extern bool isFullscreen ;
extern bool showHintsPanel ;
extern float hintsPanelHeight ;
extern bool isSoundPlaying ;
extern bool shouldPlaySound ;
extern float powerButtonX ;
extern float powerButtonY ;
extern float powerButtonSize;
extern bool isPowerButtonPressed ;
extern float powerButtonDepress ;
extern int hoveredGear ;
extern int mouseX ;
extern int mouseY ;
extern bool showTooltip ;
extern bool isSoundEnabled ;
extern float soundButtonX ;
extern float soundButtonY ;
extern float soundButtonSize ;
extern bool isSoundButtonPressed ;
extern float soundButtonDepress ;


void drawPowerButton(float x, float y, float size, bool pressed) {
    // T?t lighting n?u ?ang b?t
    GLboolean lightingEnabled = glIsEnabled(GL_LIGHTING);
    if (lightingEnabled) glDisable(GL_LIGHTING);

    // T�nh to�n v? tr� khi b? nh?n
    float offsetY = pressed ? powerButtonDepress : 0.0f;

    // V? v?ng tr?n b�n ngo�i (vi?n)
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= 360; i += 10) {
        float angle = i * M_PI / 180.0f;
        glVertex2f(x + cos(angle) * size, y + offsetY + sin(angle) * size);
    }
    glEnd();

    // V? v?ng tr?n b�n trong (n�t)
    glColor3f(0.9f, 0.1f, 0.1f); // M�u ??
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= 360; i += 10) {
        float angle = i * M_PI / 180.0f;
        glVertex2f(x + cos(angle) * (size * 0.8f), y + offsetY + sin(angle) * (size * 0.8f));
    }
    glEnd();

    // V? bi?u t??ng ngu?n (v?ng tr?n v?i ???ng th?ng)
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(4.0f);
    glBegin(GL_LINES);
    glVertex2f(x, y + offsetY - size * 0.4f);
    glVertex2f(x, y + offsetY + size *0.05f);
    glEnd();

    glBegin(GL_LINE_STRIP);
    for (int i = -45; i <= 225; i += 10) { // V? 3/4 v?ng tr?n
        float angle = i * M_PI / 180.0f;
        glVertex2f(x + cos(angle) * (size * 0.4f), y + offsetY + sin(angle) * (size * 0.4f));
    }

    glEnd();
    glLineWidth(1.0f);

    // B?t l?i lighting n?u ban ??u ?ang b?t
    if (lightingEnabled) glEnable(GL_LIGHTING);
}

void drawUI() {
    // V? m?i t�n khi panel d�ng ho�n to�n (ch? sang ph?i)
    if (!showUI && !isAnimating) {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, WIDTH, HEIGHT, 0);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_LIGHTING);
        drawArrow(10, HEIGHT/2, 20.0f, true); // M?i t�n hu?ng ph?i (m? panel)
        glEnable(GL_LIGHTING);

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        return;
    }



    // V? panel v?i animation
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, HEIGHT, 0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_LIGHTING);

    float fadeFactor = panelWidthCurrent / panelWidthTarget;
    float controlButtonY = uiMargin + 230;

    // V? n?n panel
    glColor4f(0.1f, 0.1f, 0.1f, 0.7f * fadeFactor);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(panelWidthCurrent, 0);
    glVertex2f(panelWidthCurrent, uiPanelHeight);
    glVertex2f(0, uiPanelHeight);
    glEnd();

    // V? m?i t�n d�ng panel (ch? sang tr�i khi panel dang m?)
    if (panelWidthCurrent > 30) {
        drawArrow(panelWidthCurrent-20, HEIGHT/2, 20.0f, false); // M?i t�n hu?ng tr�i (d�ng panel)
    }

    // Ch? v? n?i dung khi panel m? d? l?n
    if (panelWidthCurrent > 50 && fadeFactor > 0.3f) {


    	 // C?p nh?t v? tr� n�t ngu?n
    powerButtonX = panelWidthCurrent - powerButtonSize - uiMargin;
    powerButtonY = uiMargin + powerButtonSize/2;

    // V? n�t ngu?n
    drawPowerButton(powerButtonX-270, powerButtonY+10, powerButtonSize, isPowerButtonPressed);

    // C?p nh?t v? tr� n�t �m thanh (th�m v�o ??u h�m drawUI)
soundButtonX = panelWidthCurrent - soundButtonSize - uiMargin;
soundButtonY = uiMargin + soundButtonSize/2;

// V? n�t �m thanh (th�m sau ph?n v? n�t power)
drawSoundButton(soundButtonX, soundButtonY+10, soundButtonSize, isSoundButtonPressed, isSoundEnabled);

        // Ti�u d? panel v?i hi?u ?ng m?
        glColor4f(1.0f, 1.0f, 1.0f, fadeFactor);
        glRasterPos2f(uiMargin+50, uiMargin + 15);
        std::string title1 = "GEAR AND RACK";
        for (size_t i = 0; i < title1.length(); ++i) {
            glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, title1[i]);
        }

        glColor4f(1.0f, 1.0f, 1.0f, fadeFactor);
        glRasterPos2f(uiMargin + 70, uiMargin + 45);
        std::string title2 = "CONTROL PANEL";
        for (size_t i = 0; i < title2.length(); ++i) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, title2[i]);
        }

        // ===== N�T TOGGLE HINTS PANEL =====
	float toggleButtonY = controlButtonY + 320;
	float toggleButtonCurrentY = toggleButtonY + (isButtonPressed && pressedButton == 2 ? buttonDepressAmount : 0);

// Th�n n�t
glColor4f(showHintsPanel ? 0.3f : 0.5f, 0.3f, 0.7f, fadeFactor);
glBegin(GL_QUADS);
glVertex2f(uiMargin + 60, toggleButtonCurrentY+10);
glVertex2f(uiMargin + 230, toggleButtonCurrentY+10);
glVertex2f(uiMargin + 230, toggleButtonCurrentY + 50);
glVertex2f(uiMargin + 60, toggleButtonCurrentY + 50);
glEnd();

// Vi?n n�t
glLineWidth(1.5f);
glBegin(GL_LINE_LOOP);
glColor4f(0.7f, 0.7f, 0.7f, fadeFactor);
glVertex2f(uiMargin + 60, toggleButtonCurrentY+10);
glVertex2f(uiMargin + 230, toggleButtonCurrentY+10);
glColor4f(0.4f, 0.4f, 0.4f, fadeFactor);
glVertex2f(uiMargin + 230, toggleButtonCurrentY + 50);
glVertex2f(uiMargin + 60, toggleButtonCurrentY + 50);
glEnd();
glLineWidth(1.0f);

// Nh?n n�t
glColor4f(1.0f, 1.0f, 1.0f, fadeFactor);
glRasterPos2f(uiMargin + 100, toggleButtonCurrentY + 35);
std::string toggleLabel = showHintsPanel ? "Hide Hints" : "Show Hints";
for (size_t j = 0; j < toggleLabel.length(); ++j) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, toggleLabel[j]);
}

        // ========== PANEL G?I ? ==========
if (showHintsPanel && panelWidthCurrent > 100) {
    float hintsPanelY = controlButtonY + 350;

    // N?n panel
    glColor4f(0.15f, 0.15f, 0.2f, 0.8f * fadeFactor);
    glBegin(GL_QUADS);
    glVertex2f(uiMargin, hintsPanelY +30);
    glVertex2f(panelWidthCurrent - uiMargin, hintsPanelY+30);
    glVertex2f(panelWidthCurrent - uiMargin, hintsPanelY + hintsPanelHeight+30);
    glVertex2f(uiMargin, hintsPanelY + hintsPanelHeight+30);
    glEnd();

    // Ti�u d? panel
    glColor4f(0.8f, 0.8f, 1.0f, fadeFactor);
    glRasterPos2f(uiMargin + 120, hintsPanelY + 50);
    std::string hintsTitle = "HINTS:";
    for (size_t j = 0; j < hintsTitle.length(); ++j) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, hintsTitle[j]);
    }

    // N?i dung g?i ?
    glColor4f(0.9f, 0.9f, 0.9f, fadeFactor);

    // D?ng 1
    glRasterPos2f(uiMargin + 10, hintsPanelY + 70);
    std::string hint1 = "- Click gears to start/stop";
    for (size_t j = 0; j < hint1.length(); ++j) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, hint1[j]);
    }

    // D?ng 2
    glRasterPos2f(uiMargin + 10, hintsPanelY + 90);
    std::string hint2 = "- Drag left/right to rotate";
    for (size_t j = 0; j < hint2.length(); ++j) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, hint2[j]);
    }

    // D?ng 3
    glRasterPos2f(uiMargin + 10, hintsPanelY + 110);
    std::string hint3 = "- Drag up/down to zoom";
    for (size_t j = 0; j < hint3.length(); ++j) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, hint3[j]);
    }

    // D?ng 4
    glRasterPos2f(uiMargin + 10, hintsPanelY + 130);
    std::string hint4 = "- Middle drag to rotate components";
    for (size_t j = 0; j < hint4.length(); ++j) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, hint4[j]);
    }
}

        // ========== N�T START/STOP ==========
    float buttonY = controlButtonY + (pressedButton == 0 ? buttonDepressAmount : 0);

    glColor4f(isSimulationRunning ? 1.0f : 0.3f,
              isSimulationRunning ? 0.3f : 0.7f,
              isSimulationRunning ? 0.3f : 0.3f,
              fadeFactor);

    // Th�n n�t
    glBegin(GL_QUADS);
    glVertex2f(uiMargin , buttonY + 280);
    glVertex2f(uiMargin + 120, buttonY+280);
    glVertex2f(uiMargin + 120, buttonY +320 -(pressedButton == 0 ? buttonDepressAmount/2 : 0));
    glVertex2f(uiMargin , buttonY +320 - (pressedButton == 0 ? buttonDepressAmount/2 : 0));
    glEnd();

    // Vi?n n�t (ph?n tr�n s�ng h?n)
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glColor4f(0.9f, 0.9f, 0.9f, fadeFactor);
    glVertex2f(uiMargin , buttonY + 280);
    glVertex2f(uiMargin + 120, buttonY+280);
    glVertex2f(uiMargin + 120, buttonY +320 -(pressedButton == 0 ? buttonDepressAmount/2 : 0));
    glVertex2f(uiMargin , buttonY +320 - (pressedButton == 0 ? buttonDepressAmount/2 : 0));
    glEnd();
    glLineWidth(1.0f);

    // Nh?n n�t
    glColor4f(1.0f, 1.0f, 1.0f, fadeFactor);
    glRasterPos2f(uiMargin + 30, buttonY + 305);
    std::string startStopLabel = isSimulationRunning ? "STOP" : "START";
    for (size_t j = 0; j < startStopLabel.length(); ++j) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, startStopLabel[j]);
    }

    // ========== N�T RESET ==========
    buttonY = controlButtonY + (pressedButton == 1 ? buttonDepressAmount : 0);

    glColor4f(0.3f, 0.3f, 0.7f, fadeFactor);

    // Th�n n�t
    glBegin(GL_QUADS);
    glVertex2f(uiMargin + 160, buttonY+280);
    glVertex2f(uiMargin + 290, buttonY+280);
    glVertex2f(uiMargin + 290, buttonY + 320 - (pressedButton == 1 ? buttonDepressAmount/2 : 0));
    glVertex2f(uiMargin + 160, buttonY + 320 - (pressedButton == 1 ? buttonDepressAmount/2 : 0));
    glEnd();

    // Vi?n n�t
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glColor4f(0.9f, 0.9f, 0.9f, fadeFactor);
    glVertex2f(uiMargin + 160, buttonY+280);
    glVertex2f(uiMargin + 290, buttonY+280);
    glVertex2f(uiMargin + 290, buttonY + 320 - (pressedButton == 1 ? buttonDepressAmount/2 : 0));
    glVertex2f(uiMargin + 160, buttonY + 320 - (pressedButton == 1 ? buttonDepressAmount/2 : 0));
    glEnd();
    glLineWidth(1.0f);

    // Nh?n n�t
    glColor4f(1.0f, 1.0f, 1.0f, fadeFactor);
    glRasterPos2f(uiMargin + 200, buttonY + 305);
    std::string resetLabel = "RESET";
    for (size_t j = 0; j < resetLabel.length(); ++j) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, resetLabel[j]);
    }

        // ========== PH?N B�NH RANG ==========
    int buttonsPerRow = 2;
    float buttonWidth = 80;
    float buttonHeight = 25;
    float verticalSpacing = 30;
    float horizontalSpacing = 80;

    for (int i = 0; i < 4; i++) {
        int row = i / buttonsPerRow;
        int col = i % buttonsPerRow;
        float x = uiMargin + col * (buttonWidth + horizontalSpacing);
        float y = uiMargin + 40 + row * (buttonHeight + verticalSpacing);

        // T�nh to�n ?? l�n cho n�t hi?n t?i
        float currentDepress = (pressedGearButton == i) ? gearButtonDepress : 0.0f;
        float buttonY = y + 40 + currentDepress;
        float buttonHeightActual = buttonHeight + 10 - currentDepress/2;

        // M�u n�t v?i hi?u ?ng m?
        if (i == selectedGear) {
            glColor4f(0.5f, 0.7f, 1.0f, fadeFactor);
        } else {
            glColor4f(0.3f, 0.3f, 0.3f, fadeFactor);
        }

        // Th�n n�t v?i hi?u ?ng ?n xu?ng
        glBegin(GL_QUADS);
        glVertex2f(x, buttonY);
        glVertex2f(x +50+ buttonWidth, buttonY);
        glVertex2f(x +50 + buttonWidth, buttonY + buttonHeightActual);
        glVertex2f(x, buttonY + buttonHeightActual);
        glEnd();

        // Vi?n n�t
        glLineWidth(1.5f);
        glBegin(GL_LINE_LOOP);
        glColor4f(0.7f, 0.7f, 0.7f, fadeFactor);
        glVertex2f(x, buttonY);
        glVertex2f(x +50+ buttonWidth, buttonY);
        glColor4f(0.4f, 0.4f, 0.4f, fadeFactor);
        glVertex2f(x +50 + buttonWidth, buttonY + buttonHeightActual);
        glVertex2f(x, buttonY + buttonHeightActual);
        glEnd();
        glLineWidth(1.0f);

        // Nh?n n�t
        glColor4f(1.0f, 1.0f, 1.0f, fadeFactor);
        glRasterPos2f(x + 40, buttonY + buttonHeightActual/2 + 5);
        char label[20];
        sprintf(label, "Gear %d", i + 1);
        for (size_t j = 0; j < strlen(label); ++j) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, label[j]);
        }
    }

    // ========== PH?N THANH RANG ==========
    float rackButtonY = uiMargin + 40 + 4 * 30;
    float rackButtonCurrentY = rackButtonY + 30 + (pressedRackButton >= 0 ? rackButtonDepress : 0.0f);
    float rackButtonHeight = 35 - (pressedRackButton >= 0 ? rackButtonDepress/2 : 0.0f);

    // N�t ?i?u khi?n thanh r?ng
    if (showRackPanel) {
        glColor4f(0.5f, 0.7f, 1.0f, fadeFactor);
    } else {
        glColor4f(0.3f, 0.3f, 0.3f, fadeFactor);
    }

    // Th�n n�t v?i hi?u ?ng ?n
    glBegin(GL_QUADS);
    glVertex2f(uiMargin+60, rackButtonCurrentY);
    glVertex2f(uiMargin + 230, rackButtonCurrentY);
    glVertex2f(uiMargin + 230, rackButtonCurrentY + rackButtonHeight);
    glVertex2f(uiMargin+60, rackButtonCurrentY + rackButtonHeight);
    glEnd();

    // Vi?n n�t
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glColor4f(0.7f, 0.7f, 0.7f, fadeFactor);
    glVertex2f(uiMargin+60, rackButtonCurrentY);
    glVertex2f(uiMargin + 230, rackButtonCurrentY);
    glColor4f(0.4f, 0.4f, 0.4f, fadeFactor);
    glVertex2f(uiMargin + 230, rackButtonCurrentY + rackButtonHeight);
    glVertex2f(uiMargin+60, rackButtonCurrentY + rackButtonHeight);
    glEnd();
    glLineWidth(1.0f);

    // Nh?n n�t
    glColor4f(1.0f, 1.0f, 1.0f, fadeFactor);
    glRasterPos2f(uiMargin + 85, rackButtonCurrentY + rackButtonHeight/2 + 5);
    std::string rackLabel = "Rack Controls";
    for (size_t j = 0; j < rackLabel.length(); ++j) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, rackLabel[j]);
    }

        // Panel di?u khi?n thanh rang
        if (showRackPanel) {
            float rackPanelY = rackButtonY + 35;

            // N?n panel
            glColor4f(0.2f, 0.2f, 0.2f, 0.8f * fadeFactor);
            glBegin(GL_QUADS);
            glVertex2f(uiMargin, rackPanelY+60);
            glVertex2f(panelWidthCurrent - uiMargin, rackPanelY+60);
            glVertex2f(panelWidthCurrent - uiMargin, rackPanelY + 250);
            glVertex2f(uiMargin, rackPanelY + 250);
            glEnd();

            // Ti�u d? panel
            glColor4f(1.0f, 1.0f, 1.0f, fadeFactor);
            glRasterPos2f(uiMargin + 90, rackPanelY + 80);
            std::string rackTitle = "Rack Settings";
            for (size_t j = 0; j < rackTitle.length(); ++j) {
                glutBitmapCharacter(GLUT_BITMAP_9_BY_15, rackTitle[j]);
            }

            // C�c thanh tru?t di?u khi?n
            drawSlider(uiMargin + 10, rackPanelY + 110, panelWidthCurrent - 60,
          (rackTeethCount - MIN_RACK_TEETH) / (float)(MAX_RACK_TEETH - MIN_RACK_TEETH),
          "Teeth: ", rackTeethCount);

            drawSlider(uiMargin + 10, rackPanelY + 140, panelWidthCurrent - 60,
                     (rackProps.thickness - 0.05f) / 0.45f, "Thickness: ", rackProps.thickness);

            drawColorSlider(uiMargin + 10, rackPanelY + 170, panelWidthCurrent - 60,
                          rackColor[0], "Red: ");
            drawColorSlider(uiMargin + 10, rackPanelY + 200, panelWidthCurrent - 60,
                          rackColor[1], "Green: ");
            drawColorSlider(uiMargin + 10, rackPanelY + 230, panelWidthCurrent - 60,
                          rackColor[2], "Blue: ");
        }

        // ========== PANEL B�NH RANG CHI TI?T ==========
        if (selectedGear >= 0 && showGearPanel) {
            float gearPanelY = rackButtonY + (showRackPanel ? 135 : 35);

            // N?n panel
            glColor4f(0.2f, 0.2f, 0.2f, 0.8f * fadeFactor);
            glBegin(GL_QUADS);
            glVertex2f(uiMargin, gearPanelY+60);
            glVertex2f(panelWidthCurrent - uiMargin, gearPanelY+60);
            glVertex2f(panelWidthCurrent - uiMargin, gearPanelY + 290);
            glVertex2f(uiMargin, gearPanelY + 290);
            glEnd();

            // Ti�u d? panel
            glColor4f(1.0f, 1.0f, 1.0f, fadeFactor);
            glRasterPos2f(uiMargin + 85, gearPanelY + 85);
            char gearTitle[20];
            sprintf(gearTitle, "Gear %d Settings", selectedGear + 1);
            for (size_t j = 0; j < strlen(gearTitle); ++j) {
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, gearTitle[j]);
            }

            // L?y th�ng s? b�nh rang hi?n t?i
            GearProperties* currentGear = NULL;
            switch(selectedGear) {
                case 0: currentGear = &gear1Props; break;
                case 1: currentGear = &gear2Props; break;
                case 2: currentGear = &gear3Props; break;
                case 3: currentGear = &gear4Props; break;
            }

            // V? thanh tru?t t?c d?
if (isRotating || isStopping) {
    // Ch? cho ph�p di?u ch?nh khi dang quay ho?c dang d?ng
    drawSlider(uiMargin + 10, gearPanelY + 110, panelWidthCurrent - 60,
              displaySpeed/4.0f , "Speed: ", displaySpeed); // Chia cho 2.0f v? gi� tr? t?i da l� 2.0
} else {
    // Khi d? d?ng ho�n to�n, v? thanh tru?t ? 0
    drawSlider(uiMargin + 10, gearPanelY + 110, panelWidthCurrent - 60,
              0.0f, "Speed: ", 0.0f);
}

            drawSlider(uiMargin + 10, gearPanelY + 140, panelWidthCurrent - 60,
                     (currentGear->numTeeth - 15) / 20.0f, "Teeth: ", currentGear->numTeeth);

            drawSlider(uiMargin + 10, gearPanelY + 170, panelWidthCurrent - 60,
                     (currentGear->thickness - 0.05f) / 0.25f, "Thickness: ", currentGear->thickness);

            drawColorSlider(uiMargin + 10, gearPanelY + 200, panelWidthCurrent - 60,
                          gearColors[selectedGear][0], "Red: ");
            drawColorSlider(uiMargin + 10, gearPanelY + 230, panelWidthCurrent - 60,
                          gearColors[selectedGear][1], "Green: ");
            drawColorSlider(uiMargin + 10, gearPanelY + 260, panelWidthCurrent - 60,
                          gearColors[selectedGear][2], "Blue: ");
        }
    }

    glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

// H�m h? tr? v? thanh tru?t
void drawSlider(float x, float y, float width, float value, const char* label, float displayValue) {
    // V? nh?n
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y - 5);
    for (const char* c = label; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }

    // X? l? d?c bi?t cho thanh tru?t t?c d?
    if (strcmp(label, "Speed: ") == 0) {
        // Hi?n th? gi� tr? v?i 1 ch? s? th?p ph�n
        char valueText[20];
        sprintf(valueText, "%.1f", displayValue);

        // ??i m�u n?u d?t t?c d? t?i da (4.0)
        if (displayValue >= 4.0f) {
            glColor3f(1.0f, 0.0f, 0.0f); // M�u d?
        } else if (!isRotating) {
            glColor3f(0.3f, 0.3f, 0.3f); // M�u x�m khi d?ng
        } else {
            glColor3f(0.0f, 0.7f, 1.0f); // M�u xanh duong b?nh thu?ng
        }

        glRasterPos2f(x + 100, y - 5);
        for (size_t j = 0; j < strlen(valueText); ++j) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, valueText[j]);
        }
    }
    else { // C�c thanh tru?t kh�c
        // Hi?n th? gi� tr?
        char valueText[20];
        if (displayValue == (int)displayValue) {
            sprintf(valueText, "%d", (int)displayValue);
        } else {
            sprintf(valueText, "%.2f", displayValue);
        }

        glColor3f(1.0f, 1.0f, 1.0f);
        glRasterPos2f(x + 100, y - 5);
        for (size_t j = 0; j < strlen(valueText); ++j) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, valueText[j]);
        }
    }

    // V? thanh tru?t n?n
    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + 10);
    glVertex2f(x, y + 10);
    glEnd();

    // V? ph?n m�u n?n (cho thanh tru?t m�u)
    if (strstr(label, "Red:") || strstr(label, "Green:") || strstr(label, "Blue:")) {
        if (strstr(label, "Red:")) {
            glColor3f(value, 0.0f, 0.0f);
        } else if (strstr(label, "Green:")) {
            glColor3f(0.0f, value, 0.0f);
        } else {
            glColor3f(0.0f, 0.0f, value);
        }

        glBegin(GL_QUADS);
        glVertex2f(x + 2, y + 2);
        glVertex2f(x + width - 2, y + 2);
        glVertex2f(x + width - 2, y + 8);
        glVertex2f(x + 2, y + 8);
        glEnd();
    }

    // V? con tr? thanh tru?t
    float thumbPosition = x + value * width;

    // ??i m�u con tr? n?u l� thanh t?c d? v� d?t t?i da
    if (strcmp(label, "Speed: ") == 0 && displayValue >= 4.0f) {
        glColor3f(1.0f, 0.0f, 0.0f); // M�u d?
    } else {
        glColor3f(0.0f, 0.7f, 1.0f); // M�u xanh duong
    }

    glBegin(GL_QUADS);
    glVertex2f(thumbPosition - 5, y - 5);
    glVertex2f(thumbPosition + 5, y - 5);
    glVertex2f(thumbPosition + 5, y + 15);
    glVertex2f(thumbPosition - 5, y + 15);
    glEnd();
}

// H�m h? tr? v? thanh tru?t m�u
void drawColorSlider(float x, float y, float width, float value, const char* label) {
    // Nhan
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y - 5);
    for (const char* c = label; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }

    // Gi� tr?
    char valueText[20];
    sprintf(valueText, "%.2f", value);
    glRasterPos2f(x + 100, y - 5);
    for (size_t j = 0; j < strlen(valueText); ++j) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, valueText[j]);
    }

    // Thanh tru?t m�u
    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width , y);
    glVertex2f(x + width, y + 10);
    glVertex2f(x, y + 10);
    glEnd();

    // M�u n?n cho thanh tru?t m�u
    if (strcmp(label, "Red: ") == 0) {
        glColor3f(value, 0.0f, 0.0f);
    } else if (strcmp(label, "Green: ") == 0) {
        glColor3f(0.0f, value, 0.0f);
    } else {
        glColor3f(0.0f, 0.0f, value);
    }
    glBegin(GL_QUADS);
    glVertex2f(x + 2, y + 2);
    glVertex2f(x + width - 2, y + 2);
    glVertex2f(x + width - 2, y + 8);
    glVertex2f(x + 2, y + 8);
    glEnd();

    // Con tr?
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(x + value * width - 3, y - 3);
    glVertex2f(x + value * width + 3, y - 3);
    glVertex2f(x + value * width + 3, y + 13);
    glVertex2f(x + value * width - 3, y + 13);
    glEnd();
}

void drawSoundButton(float x, float y, float size, bool pressed, bool soundEnabled) {
    // T?t lighting n?u �ang b?t
    GLboolean lightingEnabled = glIsEnabled(GL_LIGHTING);
    if (lightingEnabled) glDisable(GL_LIGHTING);

    // T�nh to�n v? tr� khi b? nh?n
    float offsetY = pressed ? soundButtonDepress : 0.0f;

    // V? n?n n�t
    glColor3f(0.8f, 0.8f, 0.1f);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= 360; i += 10) {
        float angle = i * M_PI / 180.0f;
        glVertex2f(x + cos(angle) * size, y + offsetY + sin(angle) * size);
    }
    glEnd();

    // V? bi?u t�?ng �m thanh
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);

    // �i?u ch?nh v? tr� v� k�ch th�?c bi?u t�?ng loa
    float speakerX = x - size * 0.3f;  // D?ch sang tr�i
    float speakerSize = size * 1.0f;   // Ph�ng to bi?u t�?ng

    if (soundEnabled) {
        // Bi?u t�?ng loa khi �m thanh b?t (l?n h�n v� d?ch tr�i)
        glBegin(GL_LINE_STRIP);
        glVertex2f(speakerX - speakerSize*0.4f, y + offsetY - speakerSize*0.2f);
        glVertex2f(speakerX - speakerSize*0.4f, y + offsetY + speakerSize*0.2f);
        glVertex2f(speakerX, y + offsetY + speakerSize*0.2f);
        glVertex2f(speakerX + speakerSize*0.4f, y + offsetY + speakerSize*0.55f);
        glVertex2f(speakerX + speakerSize*0.4f, y + offsetY - speakerSize*0.55f);
        glVertex2f(speakerX, y + offsetY - speakerSize*0.2f);
        glVertex2f(speakerX - speakerSize*0.4f, y + offsetY - speakerSize*0.2f);
       glVertex2f(speakerX - speakerSize*0.1f, y + offsetY - speakerSize*0.2f);
            glVertex2f(speakerX - speakerSize*0.1f, y + offsetY + speakerSize*0.2f);
        glEnd();

// V? 3 ��?ng parabol n?m d?c
     float centerY = y + offsetY;  // T�m theo tr?c Y
    float baseX = x + speakerSize*0.25f; // V? tr� b?t �?u theo tr?c X
    float horizontalSpacing = speakerSize*0.2f; // Kho?ng c�ch ngang

    // Th�ng s? cho 3 parabol (chi?u cao t�ng d?n)
    float waveHeights[3] = {
        speakerSize*0.2f,  // Th?p
        speakerSize*0.35f,  // Trung b?nh
        speakerSize*0.5f   // Cao
    };

    float waveWidths[3] = {
        speakerSize*0.15f,  // H?p
        speakerSize*0.2f,   // Trung b?nh
        speakerSize*0.25f   // R?ng
    };

    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.5f); // L�m �?m h�n m?t ch�t

    // V? l?n l�?t 3 parabol
    for (int wave = 0; wave < 3; wave++) {
        float currentX = baseX + wave * horizontalSpacing;

        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= 20; i++) { // T�ng s? �o?n �? m�?t h�n
            float t = (float)i/20 * 2.0f - 1.0f; // t t? -1 �?n 1

            // V? tr� Y (t? d�?i l�n tr�n)
            float yPos = centerY + t * waveHeights[wave];

            // V? tr� X theo parabol: x = a*(1 - t^2)
            float xPos = currentX + (1.0f - t*t) * waveWidths[wave];

            glVertex2f(xPos, yPos);
        }
        glEnd();
    }

    } else {
        // Bi?u t�?ng loa g?ch ch�o khi �m thanh t?t (l?n h�n v� d?ch tr�i)
        glBegin(GL_LINE_STRIP);
        glVertex2f(speakerX - speakerSize*0.4f, y + offsetY - speakerSize*0.2f);
        glVertex2f(speakerX - speakerSize*0.4f, y + offsetY + speakerSize*0.2f);
        glVertex2f(speakerX, y + offsetY + speakerSize*0.2f);
        glVertex2f(speakerX + speakerSize*0.4f, y + offsetY + speakerSize*0.55f);
        glVertex2f(speakerX + speakerSize*0.4f, y + offsetY - speakerSize*0.55f);
        glVertex2f(speakerX, y + offsetY - speakerSize*0.2f);
        glVertex2f(speakerX - speakerSize*0.4f, y + offsetY - speakerSize*0.2f);
        glVertex2f(speakerX - speakerSize*0.1f, y + offsetY - speakerSize*0.2f);
            glVertex2f(speakerX - speakerSize*0.1f, y + offsetY + speakerSize*0.2f);
        glEnd();

         glLineWidth(3.0f); // L�m �?m d?u X
        glBegin(GL_LINES);
        // ��?ng ch�o t? tr�i sang ph?i
        glVertex2f(speakerX + speakerSize*0.6f, y + offsetY - speakerSize*0.3f);
        glVertex2f(speakerX + speakerSize*1.1f, y + offsetY + speakerSize*0.3f);
        // ��?ng ch�o t? ph?i sang tr�i
        glVertex2f(speakerX + speakerSize*1.1f, y + offsetY - speakerSize*0.3f);
        glVertex2f(speakerX + speakerSize*0.6f, y + offsetY + speakerSize*0.3f);

        glEnd();

    }

    glLineWidth(1.0f);

    // B?t l?i lighting n?u ban �?u �ang b?t
    if (lightingEnabled) glEnable(GL_LIGHTING);
}

void drawArrow(float x, float y, float size, bool pointingLeft) {
    // T?t lighting n?u dang b?t
    GLboolean lightingEnabled = glIsEnabled(GL_LIGHTING);
    if (lightingEnabled) glDisable(GL_LIGHTING);

    glColor3f(1.0f, 1.0f, 1.0f); // M�u tr?ng

    glBegin(GL_TRIANGLES);
    if (pointingLeft) {
        // M?i t�n hu?ng sang tr�i (d�ng panel)
        glVertex2f(x + size, y);
        glVertex2f(x, y + size/2);
        glVertex2f(x, y - size/2);
    } else {
        // M?i t�n hu?ng sang ph?i (m? panel)
        glVertex2f(x, y);
        glVertex2f(x + size, y + size/2);
        glVertex2f(x + size, y - size/2);
    }
    glEnd();

    // B?t l?i lighting n?u ban d?u dang b?t
    if (lightingEnabled) glEnable(GL_LIGHTING);
}
