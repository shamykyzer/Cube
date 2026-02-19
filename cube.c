#include <stdio.h>
#include <math.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// Rotation angles
double A, B, C;

// Cube and screen parameters
float cubeWidth = 20.8f; // 30% bigger than 16.0
enum { MAX_WIDTH = 160, MAX_HEIGHT = 44 };
float zBuffer[MAX_WIDTH * MAX_HEIGHT];
char buffer[MAX_WIDTH * MAX_HEIGHT];
int backgroundASCIICode = ' ';
int distanceFromCam = 100;
float horizontalOffset;
float K1 = 40;

// Increment speed for the cube rotation
float incrementSpeed = 0.6f;

// Animation tuning
static const unsigned int frameDelayMs = 8; // lower = faster FPS
static const double rotSpeedA = 0.0306;    // 15% slower vs 0.036
static const double rotSpeedB = 0.0204;
static const double rotSpeedC = 0.0102;

// Variables for calculations
float x, y, z;
float ooz;
int xp, yp;
int idx;

// Function to calculate the X coordinate after rotation
static float calculateX(float i, float j, float k) {
  return j * sin(A) * sin(B) * cos(C) - k * cos(A) * sin(B) * cos(C) +
         j * cos(A) * sin(C) + k * sin(A) * sin(C) + i * cos(B) * cos(C);
}

// Function to calculate the Y coordinate after rotation
static float calculateY(float i, float j, float k) {
  return j * cos(A) * cos(C) + k * sin(A) * cos(C) -
         j * sin(A) * sin(B) * sin(C) + k * cos(A) * sin(B) * sin(C) -
         i * cos(B) * sin(C);
}

// Function to calculate the Z coordinate after rotation
static float calculateZ(float i, float j, float k) {
  return k * cos(A) * cos(B) - j * sin(A) * cos(B) + i * sin(B);
}

// Function to calculate the surface coordinates and update the buffer
void calculateForSurface(
    float cubeX,
    float cubeY,
    float cubeZ,
    int ch,
    int renderWidth,
    int renderHeight
) {
    x = calculateX(cubeX, cubeY, cubeZ);
    y = calculateY(cubeX, cubeY, cubeZ);
    z = calculateZ(cubeX, cubeY, cubeZ) + distanceFromCam;

    ooz = 1 / z;

    xp = (int)(renderWidth / 2 + horizontalOffset + K1 * ooz * x * 2);
    yp = (int)(renderHeight / 2 + K1 * ooz * y);

    idx = xp + yp * renderWidth;
    if (idx >= 0 && idx < renderWidth * renderHeight) {
        if (ooz > zBuffer[idx]) {
            zBuffer[idx] = ooz;
            buffer[idx] = ch;
        }
    }
}

static void sleep_ms(unsigned int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

#ifdef _WIN32
static void enable_virtual_terminal_processing(void) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
#endif

int main() {
#ifdef _WIN32
    enable_virtual_terminal_processing();
#endif
    setvbuf(stdout, NULL, _IONBF, 0);

    int cols = MAX_WIDTH, rows = MAX_HEIGHT;
    int renderWidth = MAX_WIDTH, renderHeight = MAX_HEIGHT;
    int leftMargin = 0, topMargin = 0;

#ifdef _WIN32
    {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE && GetConsoleScreenBufferInfo(hOut, &csbi)) {
            cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
            rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
            if (cols <= 0) cols = MAX_WIDTH;
            if (rows <= 0) rows = MAX_HEIGHT;
        }
    }
#endif

    if (cols < renderWidth) renderWidth = cols;
    if (rows < renderHeight) renderHeight = rows;
    if (renderWidth < 1) renderWidth = 1;
    if (renderHeight < 1) renderHeight = 1;
    if (cols > renderWidth) leftMargin = (cols - renderWidth) / 2;
    if (rows > renderHeight) topMargin = (rows - renderHeight) / 2;

    // Clear the screen
    printf("\x1b[2J");
    while (1) {
        // Reset the buffer and zBuffer
        memset(buffer, backgroundASCIICode, (size_t)renderWidth * (size_t)renderHeight);
        memset(zBuffer, 0, (size_t)renderWidth * (size_t)renderHeight * sizeof(zBuffer[0]));

        // Loop over the range of cubeX from -cubeWidth to cubeWidth with steps of incrementSpeed
        for (float cubeX = -cubeWidth; cubeX < cubeWidth; cubeX += incrementSpeed) {
            // Loop over the range of cubeY from -cubeWidth to cubeWidth with steps of incrementSpeed
            for (float cubeY = -cubeWidth; cubeY < cubeWidth; cubeY += incrementSpeed) {
                //  front surface of the cube
                calculateForSurface(cubeX, cubeY, -cubeWidth, '.', renderWidth, renderHeight);
                //  right surface of the cube
                calculateForSurface(cubeWidth, cubeY, cubeX, '$', renderWidth, renderHeight);
                //  left surface of the cube
                calculateForSurface(-cubeWidth, cubeY, -cubeX, '~', renderWidth, renderHeight);
                //  back surface of the cube
                calculateForSurface(-cubeX, cubeY, cubeWidth, '#', renderWidth, renderHeight);
                //  bottom surface of the cube
                calculateForSurface(cubeX, -cubeWidth, -cubeY, ';', renderWidth, renderHeight);
                //  top surface of the cube
                calculateForSurface(cubeX, cubeWidth, cubeY, '+', renderWidth, renderHeight);
            }
        }

        // Print the buffer to the screen, centered in the terminal (when larger than render area)
        for (int yRow = 0; yRow < renderHeight; yRow++) {
            // ANSI cursor position is 1-based: row;col
            printf("\x1b[%d;%dH", yRow + 1 + topMargin, 1 + leftMargin);
            fwrite(&buffer[yRow * renderWidth], 1, (size_t)renderWidth, stdout);
        }

        // Increment the rotation angles
        A += rotSpeedA;
        B += rotSpeedB;
        C += rotSpeedC;

        sleep_ms(frameDelayMs);
    }
    return 0;
}