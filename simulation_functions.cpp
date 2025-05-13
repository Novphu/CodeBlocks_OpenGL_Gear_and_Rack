#include "gear_rack_simulator.h"

extern GearProperties gear1Props;
extern GearProperties gear2Props;
extern GearProperties gear3Props;
extern GearProperties gear4Props;
extern RackProperties rackProps;
extern std::vector<GearPosition> gearPositions;
extern float rackColor[3];
extern int rackTeethCount;
extern float componentsAngleY;

extern float gearColors[4][3] ;

// Gear angles and rotation
extern float angleGear1;
extern float angleGear2;
extern float angleGear3;
extern float angleGear4;
extern float rotationSpeedGear ;

extern float rotateangle ;
extern float rackPositionX ;
extern float rackSpeed;
extern bool rackMovingRight;
extern bool isRotating;

// Rotation control
extern float currentRotationSpeed ;
extern float targetRotationSpeed;
extern const float acceleration;
extern bool isAccelerating;
extern bool isDecelerating ;

extern float globalRotationSpeed ;
extern float displaySpeed;
extern bool isStopping;
extern bool isSimulationRunning ;
extern bool isSoundPlaying ;
extern bool isSoundEnabled ;

void updateRackPosition(float deltaTime) {
    if (isRotating || isAccelerating || isDecelerating) {
        float angularSpeed = currentRotationSpeed * 0.1f * deltaTime * 60.0f;

float effectiveRadius = gear4Props.outerRadius;

    // S? d?ng s? rang hi?n t?i c?a b�nh rang 4
    float teethPerRevolution = gear4Props.numTeeth;

    // S? d?ng kho?ng c�ch rang hi?n t?i c?a thanh rang
    float rackDistancePerTooth = rackProps.toothSpacing;

    float angleChangeRad = angularSpeed * M_PI / 180.0f;
    float circumferenceCovered = effectiveRadius * angleChangeRad;
    float teethPassed = circumferenceCovered / (2 * M_PI * effectiveRadius / teethPerRevolution);
    float rackMovement = teethPassed * rackDistancePerTooth;

    if (componentsAngleY <= -45.0f) {
        rackMovingRight = true;
    } else if (componentsAngleY >= 40.0f) {
        rackMovingRight = false;
    }

    if (componentsAngleY <= -45.0f || componentsAngleY >= 40.0f) {
        if (rackMovingRight) {
            rackPositionX += rackMovement;
            if (rackPositionX > 0.8f) rackPositionX = 0.8f;
        } else {
            rackPositionX -= rackMovement;
            if (rackPositionX < -5.8f) rackPositionX = -5.8f;
        }
    }
}
}

void updateRotationSpeed(float deltaTime) {
    if (currentRotationSpeed > 4.0f) {
        currentRotationSpeed = 4.0f;
    }
    if (targetRotationSpeed > 4.0f) {
        targetRotationSpeed = 4.0f;
    }

    if (isAccelerating) {
        currentRotationSpeed += acceleration * deltaTime * 60.0f;
        if (currentRotationSpeed >= targetRotationSpeed * globalRotationSpeed) {
            currentRotationSpeed = targetRotationSpeed * globalRotationSpeed;
            isAccelerating = false;
        }
        displaySpeed = std::min(4.0f, currentRotationSpeed);
    }
    else if (isDecelerating) {
        currentRotationSpeed -= acceleration;
        if (currentRotationSpeed <= 0.0f) {
            currentRotationSpeed = 0.0f;
            isDecelerating = false;
            isRotating = false;
            isStopping = false;
        }
        displaySpeed = currentRotationSpeed;
    }
    else if (isRotating) {
        displaySpeed = globalRotationSpeed;
    }
    else {
        displaySpeed = 0.0f;
    }
}

void calculateNormal(float x1, float y1, float z1,
                            float x2, float y2, float z2,
                            float x3, float y3, float z3) {
    float nx = y1*(z2-z3) + y2*(z3-z1) + y3*(z1-z2);
    float ny = z1*(x2-x3) + z2*(x3-x1) + z3*(x1-x2);
    float nz = x1*(y2-y3) + x2*(y3-y1) + x3*(y1-y2);

    float length = sqrt(nx*nx + ny*ny + nz*nz);
    if (length > 0) {
        glNormal3f(nx/length, ny/length, nz/length);
    }
}

void controlSound() {
    if (!isSoundEnabled) return; // Kh�ng l�m g? n?u �m thanh b? t?t

    if ((isRotating || isAccelerating) && !isSoundPlaying) {
        // B?t ??u ph�t �m thanh khi b?t ??u quay
        PlaySound(TEXT("gears-18005.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
        isSoundPlaying = true;
    }
    else if ((!isRotating && !isAccelerating && currentRotationSpeed <= 0.01f) && isSoundPlaying) {
        // D?ng �m thanh khi ho�n to�n d?ng
        PlaySound(NULL, NULL, 0);
        isSoundPlaying = false;
    }
}

void playClickSound() {
	if (!isSoundEnabled) return; // Th�m d?ng n�y
    PlaySound(TEXT("mouse-click-331781.wav"), NULL, SND_FILENAME | SND_ASYNC);
}

void resetSimulation() {
    // Reset g�c quay c�c b�nh r?ng
    angleGear1 = 0.0f;
    angleGear2 = 0.0f;
    angleGear3 = 0.0f;
    angleGear4 = 0.0f;

    // Reset v? tr� thanh r?ng
    rackPositionX = -2.0f;
    rackMovingRight = true;

    // Reset g�c quay th�nh ph?n
    componentsAngleY = 0.0f;

    // Reset t?c ??
    globalRotationSpeed = 1.0f;
    currentRotationSpeed = 0.0f;
    targetRotationSpeed = 0.0f;
    displaySpeed = 0.0f;

    // Reset tr?ng th�i
    isRotating = false;
    isAccelerating = false;
    isDecelerating = false;
    isStopping = false;
    isSimulationRunning = false;

    // ========== RESET TH�NG S? B�NH R?NG ==========
    // B�nh r?ng 1
    gear1Props = {20, 0.1f, 0.4f, 0.05f, 0.1f, 0.15f};
    gearColors[0][0] = 0.2f;  // Red
    gearColors[0][1] = 0.5f;  // Green
    gearColors[0][2] = 0.2f;  // Blue

    // B�nh r?ng 2
    gear2Props = {16, 0.1f, 0.3f, 0.05f, 0.1f, 0.15f};
    gearColors[1][0] = 0.75f; // Red
    gearColors[1][1] = 0.75f; // Green
    gearColors[1][2] = 0.75f; // Blue

    // B�nh r?ng 3
    gear3Props = {30, 0.1f, 0.6f, 0.05f, 0.1f, 0.15f};
    gearColors[2][0] = 0.8f;  // Red
    gearColors[2][1] = 0.5f;  // Green
    gearColors[2][2] = 0.2f;  // Blue

    // B�nh r?ng 4
    gear4Props = {20, 0.1f, 0.4f, 0.05f, 0.1f, 0.15f};
    gearColors[3][0] = 0.5f;  // Red
    gearColors[3][1] = 0.9f;  // Green
    gearColors[3][2] = 0.2f;  // Blue

    // ========== RESET THANH R?NG ==========
    rackProps = {8.0f, 0.15f, 0.05f, 0.15f, 0.15f, 0.2f, 0.02f};
    rackTeethCount = 50;
    rackProps.toothSpacing = rackProps.length / rackTeethCount;
    rackColor[0] = 0.7f;  // Red
    rackColor[1] = 0.7f;  // Green
    rackColor[2] = 0.7f;  // Blue

    // Reset l?i v? tr� c�c b�nh r?ng
    gearPositions.clear();
    gearPositions.push_back({-1.0f, 0.72f, 3.0f, gear1Props.outerRadius, 1});
    gearPositions.push_back({-0.4f, 0.72f, 2.4f, gear2Props.outerRadius, 2});
    gearPositions.push_back({0.6f, 0.72f, 2.1f, gear3Props.outerRadius, 3});
    gearPositions.push_back({1.3f, 0.72f, 3.0f, gear4Props.outerRadius, 4});

    // D?ng �m thanh khi reset
    if (isSoundPlaying) {
        PlaySound(NULL, NULL, 0);
        isSoundPlaying = false;
    }

    // Th�m v�o h�m resetSimulation()
isSoundEnabled = true; // M?c ??nh b?t �m thanh khi reset
}
