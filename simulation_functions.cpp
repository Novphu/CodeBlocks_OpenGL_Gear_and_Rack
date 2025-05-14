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

void updateRackPosition(float deltaTime)
{
    if (isRotating || isAccelerating || isDecelerating)
    {
        float angularSpeed = currentRotationSpeed * 0.1 * deltaTime * 60.0;
        float effectiveRadius = gear4Props.outerRadius;

        // S? d?ng s? rang hi?n t?i c?a bánh rang 4
        float teethPerRevolution = gear4Props.numTeeth;

        // S? d?ng kho?ng cách rang hi?n t?i c?a thanh rang
        float rackDistancePerTooth = rackProps.toothSpacing;

        float angleChangeRad = angularSpeed * M_PI / 180.0;
        float circumferenceCovered = effectiveRadius * angleChangeRad;
        float teethPassed = circumferenceCovered / (2 * M_PI * effectiveRadius / teethPerRevolution);
        float rackMovement = teethPassed * rackDistancePerTooth;

        if (componentsAngleY <= -45.0)
        {
            rackMovingRight = true;
        }
        else if (componentsAngleY >= 40.0)
        {
            rackMovingRight = false;
        }

        if (componentsAngleY <= -45.0 || componentsAngleY >= 40.0)
        {
            if (rackMovingRight)
            {
                rackPositionX += rackMovement;
                if (rackPositionX > 0.8) rackPositionX = 0.8;
            }
            else
            {
                rackPositionX -= rackMovement;
                if (rackPositionX < -5.8) rackPositionX = -5.8;
            }
        }
    }
}

void updateRotationSpeed(float deltaTime)
{
    if (currentRotationSpeed > 4.0)
    {
        currentRotationSpeed = 4.0;
    }
    if (targetRotationSpeed > 4.0)
    {
        targetRotationSpeed = 4.0;
    }

    if (isAccelerating)
    {
        currentRotationSpeed += acceleration * deltaTime * 60.0;
        if (currentRotationSpeed >= targetRotationSpeed * globalRotationSpeed)
        {
            currentRotationSpeed = targetRotationSpeed * globalRotationSpeed;
            isAccelerating = false;
        }
        displaySpeed = std::min(4.0f, currentRotationSpeed);
    }
    else if (isDecelerating)
    {
        currentRotationSpeed -= acceleration;
        if (currentRotationSpeed <= 0.0)
        {
            currentRotationSpeed = 0.0;
            isDecelerating = false;
            isRotating = false;
            isStopping = false;
        }
        displaySpeed = currentRotationSpeed;
    }
    else if (isRotating)
    {
        displaySpeed = globalRotationSpeed;
    }
    else
    {
        displaySpeed = 0.0;
    }
}

void calculateNormal(float x1, float y1, float z1,
                            float x2, float y2, float z2,
                            float x3, float y3, float z3)
{
    float nx = y1*(z2-z3) + y2*(z3-z1) + y3*(z1-z2);
    float ny = z1*(x2-x3) + z2*(x3-x1) + z3*(x1-x2);
    float nz = x1*(y2-y3) + x2*(y3-y1) + x3*(y1-y2);

    float length = sqrt(nx*nx + ny*ny + nz*nz);
    if (length > 0)
    {
        glNormal3f(nx/length, ny/length, nz/length);
    }
}

void controlSound()
{
    if (!isSoundEnabled) return; // Không làm g? n?u âm thanh b? t?t

    if ((isRotating || isAccelerating) && !isSoundPlaying)
    {
        // B?t ??u phát âm thanh khi b?t ??u quay
        PlaySound(TEXT("gears-18005.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
        isSoundPlaying = true;
    }
    else if ((!isRotating && !isAccelerating && currentRotationSpeed <= 0.01f) && isSoundPlaying)
    {
        // D?ng âm thanh khi hoàn toàn d?ng
        PlaySound(NULL, NULL, 0);
        isSoundPlaying = false;
    }
}

void playClickSound()
{
	if (!isSoundEnabled) return; // Thêm d?ng này
    PlaySound(TEXT("mouse-click-331781.wav"), NULL, SND_FILENAME | SND_ASYNC);
}

void resetSimulation()
{
    // Reset góc quay các bánh r?ng
    angleGear1 = 0.0;
    angleGear2 = 0.0;
    angleGear3 = 0.0;
    angleGear4 = 0.0;

    // Reset v? trí thanh r?ng
    rackPositionX = -2.0;
    rackMovingRight = true;

    // Reset góc quay thành ph?n
    componentsAngleY = 0.0;

    // Reset t?c ??
    globalRotationSpeed = 1.0;
    currentRotationSpeed = 0.0;
    targetRotationSpeed = 0.0;
    displaySpeed = 0.0;

    // Reset tr?ng thái
    isRotating = false;
    isAccelerating = false;
    isDecelerating = false;
    isStopping = false;
    isSimulationRunning = false;

    // ========== RESET THÔNG S? BÁNH R?NG ==========
    // Bánh r?ng 1
    gear1Props = {20, 0.1, 0.4, 0.05, 0.1, 0.15};
    gearColors[0][0] = 0.2;  // Red
    gearColors[0][1] = 0.5;  // Green
    gearColors[0][2] = 0.2;  // Blue

    // Bánh r?ng 2
    gear2Props = {16, 0.1, 0.3, 0.05, 0.1, 0.15};
    gearColors[1][0] = 0.75; // Red
    gearColors[1][1] = 0.75; // Green
    gearColors[1][2] = 0.75; // Blue

    // Bánh r?ng 3
    gear3Props = {30, 0.1, 0.6, 0.05, 0.1, 0.15};
    gearColors[2][0] = 0.8;  // Red
    gearColors[2][1] = 0.5;  // Green
    gearColors[2][2] = 0.2;  // Blue

    // Bánh r?ng 4
    gear4Props = {20, 0.1, 0.4, 0.05, 0.1, 0.15};
    gearColors[3][0] = 0.5;  // Red
    gearColors[3][1] = 0.9;  // Green
    gearColors[3][2] = 0.2;  // Blue

    // ========== RESET THANH R?NG ==========
    rackProps = {8.0, 0.15, 0.05, 0.15, 0.15, 0.2, 0.02};
    rackTeethCount = 50;
    rackProps.toothSpacing = rackProps.length / rackTeethCount;
    rackColor[0] = 0.7;  // Red
    rackColor[1] = 0.7;  // Green
    rackColor[2] = 0.7;  // Blue

    // Reset l?i v? trí các bánh r?ng
    gearPositions.clear();
    gearPositions.push_back({-1.0, 0.72, 3.0, gear1Props.outerRadius, 1});
    gearPositions.push_back({-0.4, 0.72, 2.4, gear2Props.outerRadius, 2});
    gearPositions.push_back({0.6, 0.72, 2.1, gear3Props.outerRadius, 3});
    gearPositions.push_back({1.3, 0.72, 3.0, gear4Props.outerRadius, 4});

    // D?ng âm thanh khi reset
    if (isSoundPlaying)
    {
        PlaySound(NULL, NULL, 0);
        isSoundPlaying = false;
    }

    // Thêm vào hàm resetSimulation()
    isSoundEnabled = true; // M?c ??nh b?t âm thanh khi reset
}
