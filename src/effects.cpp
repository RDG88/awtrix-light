#include "effects.h"

const CRGBPalette16 palette = RainbowColors_p;

void Pacifica(FastLED_NeoMatrix *matrix, int16_t x, int16_t y)
{
    const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();
    uint8_t speed = 2;
    static uint32_t sPacificaTime = 0;
    sPacificaTime += speed;
    for (uint16_t i = 0; i < cols; i++)
    {
        for (uint16_t j = 0; j < rows; j++)
        {
            uint16_t ulx = (sPacificaTime / 8) + (i * 16);
            uint16_t uly = (sPacificaTime / 4) + (j * 16);
            uint16_t v = 0;
            v += sin16(ulx * 6 + sPacificaTime / 2) / 8 + 127;
            v += sin16(uly * 9 + sPacificaTime / 2) / 8 + 127;
            v += sin16(ulx * 7 + uly * 2 - sPacificaTime) / 16;
            v = v / 3;
            CRGB color = ColorFromPalette(OceanColors_p, v);
            matrix->drawPixel(x + i, y + j, color);
        }
    }
}

void TheaterChase(FastLED_NeoMatrix *matrix, int16_t x, int16_t y)
{
    const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();
    static uint16_t j = 0;
    static uint16_t callCount = 0;
    callCount++;

    if (callCount % 3 == 0)
    {
        j = (j + 1) % 3;
    }

    for (uint16_t i = 0; i < cols; i++)
    {
        for (uint16_t k = 0; k < rows; k++)
        {
            if ((i + j) % 3 == 0)
            {
                uint8_t colorIndex = (i * 256 / cols) & 255;
                CRGB color = ColorFromPalette(RainbowColors_p, colorIndex);
                matrix->drawPixel(x + i, y + k, color);
            }
            else
            {
                matrix->drawPixel(x + i, y + k, matrix->Color(0, 0, 0));
            }
        }
    }
}

void Plasma(FastLED_NeoMatrix *matrix, int16_t x, int16_t y)
{
    const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();
    static uint32_t time = 0;

    for (uint16_t i = 0; i < cols; i++)
    {
        for (uint16_t j = 0; j < rows; j++)
        {
            uint8_t value = sin8(i * 10 + time) + sin8(j * 10 + time / 2) + sin8((i + j) * 10 + time / 3) / 3;
            CRGB color = ColorFromPalette(RainbowColors_p, value);
            matrix->drawPixel(x + i, y + j, color);
        }
    }
    time += 1;
}

struct MatrixColumn
{
    int16_t position; // Y-Position des "fallenden" Pixels
    CRGB color;       // Farbe des Pixels
};

void Matrix(FastLED_NeoMatrix *matrix, int16_t x, int16_t y)
{
    const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();
    static uint32_t lastMove = 0;
    static CRGB spawnColor = CRGB(175, 255, 175);
    static CRGB trailColor = CRGB(27, 130, 39);
    static uint8_t intensity = 10; // Adjust to manage the frequency of new "drops"

    // Create a static matrix to hold the state of each pixel
    static CRGB ledState[32][8];

    uint8_t baseSpeed = 180;                                                          // Base value for speed calculation
    uint8_t speed = (256 - baseSpeed) >> map(min(rows, (uint16_t)150), 0, 150, 0, 3); // Slower speeds for small displays
    uint8_t fadeSpeed = 100;                                                          // Value for fade calculation
    uint8_t fade = map(fadeSpeed, 0, 255, 50, 250);                                   // equals trail size

    // Update animation based on speed
    if (millis() - lastMove >= speed)
    {
        lastMove = millis();

        // Shift rows down and update the ledState array
        for (uint16_t i = 0; i < cols; i++)
        {
            for (uint16_t j = rows - 1; j > 0; j--)
            {
                ledState[i][j] = ledState[i][j - 1];
            }
        }

        // Fade top row and spawn new pixels
        for (uint16_t i = 0; i < cols; i++)
        {
            if (ledState[i][0] == spawnColor)
            {
                ledState[i][0] = trailColor;
            }
            else
            {
                ledState[i][0].nscale8(fade);
            }

            // Randomly spawn new pixels
            if (random8() < intensity)
            {
                ledState[i][0] = spawnColor;
            }
        }
    }

    // Always draw the current state
    for (uint16_t i = 0; i < cols; i++)
    {
        for (uint16_t j = 0; j < rows; j++)
        {
            matrix->drawPixel(x + i, y + j, ledState[i][j]);
        }
    }
}

void SwirlIn(FastLED_NeoMatrix *matrix, int16_t x, int16_t y)
{
    const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();
    static uint32_t lastUpdate = 0;
    static uint16_t angle = 0;

    uint8_t speed = 40;

    if (millis() - lastUpdate > speed)
    {
        lastUpdate = millis();
        angle += 4;
    }

    float centerX = 16.0;
    float centerY = 4.0;

    for (uint16_t i = 0; i < cols; i++)
    {
        for (uint16_t j = 0; j < rows; j++)
        {
            float xDiff = centerX - i;
            float yDiff = centerY - j;
            uint16_t dist = sqrt(xDiff * xDiff + yDiff * yDiff);
            uint8_t hue = map(dist, 0, sqrt(centerX * centerX + centerY * centerY), 0, 255) + angle;
            CRGB color = ColorFromPalette(palette, hue);
            matrix->drawPixel(x + i, y + j, color);
        }
    }
}

void SwirlOut(FastLED_NeoMatrix *matrix, int16_t x, int16_t y)
{
    const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();
    static uint32_t lastUpdate = 0;
    static uint16_t angle = 0;

    uint8_t speed = 40;

    if (millis() - lastUpdate > speed)
    {
        lastUpdate = millis();
        angle += 4;
    }

    float centerX = 16.0;
    float centerY = 4.0;

    for (uint16_t i = 0; i < cols; i++)
    {
        for (uint16_t j = 0; j < rows; j++)
        {
            float xDiff = centerX - i;
            float yDiff = centerY - j;
            uint16_t dist = sqrt(xDiff * xDiff + yDiff * yDiff);
            uint8_t hue = 255 - map(dist, 0, sqrt(centerX * centerX + centerY * centerY), 0, 255) + angle;
            CRGB color = ColorFromPalette(palette, hue);
            matrix->drawPixel(x + i, y + j, color);
        }
    }
}

void ColorWaves(FastLED_NeoMatrix *matrix, int16_t x, int16_t y)
{
    const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();
    static uint32_t lastUpdate = 0;

    uint8_t speed = 4;

    if (millis() - lastUpdate > (uint32_t)1000 / speed)
    {
        lastUpdate = millis();
    }

    for (uint16_t i = 0; i < cols; i++)
    {
        for (uint16_t j = 0; j < rows; j++)
        {
            uint8_t hue = (uint32_t)i * 255 / cols + millis() / 10;
            uint8_t sat = 255;
            uint8_t val = max((sin16(lastUpdate + i * 4096) + 32768) >> 8, 100); // 85 is around 1/3 of 255
            CHSV color = CHSV(hue, sat, val);
            matrix->drawPixel(x + i, y + j, color);
        }
    }
}

#define MAX_BRIGHTNESS 255
#define FADE_RATE 0.02

struct Star
{
    CRGB color;
    float brightness;
};

Star stars[32][8]; // Create a buffer to store the state of the LEDs

void TwinklingStars(FastLED_NeoMatrix *matrix, int16_t x, int16_t y)
{
    const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();
    static uint32_t lastUpdate = 0;
    uint8_t speed = 4;

    // Fade all LEDs each frame
    for (uint16_t i = 0; i < cols; i++)
    {
        for (uint16_t j = 0; j < rows; j++)
        {
            stars[i][j].brightness -= 0.01;
            if (stars[i][j].brightness < 0)
                stars[i][j].brightness = 0;
            matrix->drawPixel(x + i, y + j, stars[i][j].color.nscale8_video(stars[i][j].brightness * 255));
        }
    }

    // Randomly twinkle new stars each frame
    if (millis() - lastUpdate > (uint32_t)1000 / speed)
    {
        lastUpdate = millis();
        uint8_t numStars = random(1, 5); // Create between 1-5 new stars per frame
        for (uint8_t i = 0; i < numStars; i++)
        {
            uint16_t starX = random(cols);
            uint16_t starY = random(rows);
            // Star color - varying the saturation and value for shades of blue and white
            stars[starX][starY].color = CHSV(160, random8(100, 255), MAX_BRIGHTNESS);
            stars[starX][starY].brightness = 1.0;
            matrix->drawPixel(x + starX, y + starY, stars[starX][starY].color);
        }
    }
}

// ######## LookingEyes ############

int blinkIndex[] = {1, 2, 3, 4, 3, 2, 1};
int blinkCountdown = 60;
int gazeCountdown = 30;
int gazeFrames = 5;
int eyeX = 9;
int eyeY = 3;
int newX = 9;
int newY = 3;
int dX = 6;
int dY = 0;
int PET_MOOD = 0;

uint16_t eye[5][64] = {
    {0, 0, 65535, 65535, 65535, 65535, 0, 0, 0, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 0, 0, 0, 65535, 65535, 65535, 65535, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 0, 0, 0, 65535, 65535, 65535, 65535, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 65535, 65535, 65535, 65535, 0, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 65535, 65535, 65535, 65535, 0, 0, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 0, 0, 0, 0, 65535, 65535, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 65535, 0, 0, 0, 0, 0, 0, 65535, 0, 65535, 65535, 65535, 65535, 65535, 65535, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

void LookingEyes(FastLED_NeoMatrix *matrix, int16_t x, int16_t y)
{
    if (blinkCountdown < sizeof(blinkIndex) / sizeof(blinkIndex[0]) - 1)
    {
        matrix->drawRGBBitmap(6 + x, 0 + y, eye[blinkIndex[blinkCountdown]], 8, 8);
        matrix->drawRGBBitmap(18 + x, 0 + y, eye[blinkIndex[blinkCountdown]], 8, 8);
    }
    else
    {
        matrix->drawRGBBitmap(6 + x, 0 + y, eye[0], 8, 8);
        matrix->drawRGBBitmap(18 + x, 0 + y, eye[0], 8, 8);
    }

    blinkCountdown = blinkCountdown - 0.1;
    if (blinkCountdown == 0)
    {
        blinkCountdown = random(60, 350);
    }

    if (gazeCountdown <= gazeFrames)
    {
        gazeCountdown -= 0.5;
        matrix->fillRect(newX - (dX * gazeCountdown / gazeFrames) + 6 + x, newY - (dY * gazeCountdown / gazeFrames) + y, 2, 2, 0);
        matrix->fillRect(newX - (dX * gazeCountdown / gazeFrames) + 18 + x, newY - (dY * gazeCountdown / gazeFrames) + y, 2, 2, 0);
        if (gazeCountdown == 0)
        {
            eyeX = newX;
            eyeY = newY;
            do
            {
                switch (PET_MOOD)
                {
                case 0:
                    newX = random(0, 6);
                    newY = random(0, 6);
                    dX = newX - 4;
                    dY = newY - 4;
                    break;
                case 1:
                    newX = random(0, 7);
                    newY = random(0, 7);
                    dX = newX - 3;
                    dY = newY - 3;
                    break;
                case 2:
                    newX = random(1, 7);
                    newY = random(1, 4);
                    dX = newX - 3;
                    dY = newY - 3;
                    break;
                case 3:
                    newX = random(0, 7);
                    newY = random(3, 7);
                    dX = newX - 3;
                    dY = newY - 3;
                    break;
                }
            } while (((dX * dX + dY * dY) <= 3));
            dX = newX - eyeX;
            dY = newY - eyeY;
            gazeFrames = random(15, 40);
            gazeCountdown = random(gazeFrames, 120);
        }
    }
    else
    {
        gazeCountdown -= 0.5;
        matrix->fillRect(eyeX + 6 + x, eyeY + y, 2, 2, 0);
        matrix->fillRect(eyeX + 18 + x, eyeY + y, 2, 2, 0);
    }
}

// ######## Snake ############
#define MAX_SNAKE_LENGTH 64
#define INITIAL_SNAKE_LENGTH 2
#define SNAKE_SPEED 60

struct Point
{
    int16_t x, y;
    uint8_t colorIndex; // add a color index to each segment
};

Point snake[MAX_SNAKE_LENGTH];
bool shouldMove[MAX_SNAKE_LENGTH]; // new array to track whether each segment should move
uint8_t snakeLength = INITIAL_SNAKE_LENGTH;
uint8_t snakeDirection = 0; // 0=up, 1=right, 2=down, 3=left
uint32_t lastMove = 0;
bool isGameOver = false;
Point apple;
CRGB colors[] = {CRGB::Red, CRGB::Green, CRGB::Blue, CRGB::Yellow, CRGB::Purple};
uint8_t colorIndex = 0;

bool willCollide(int16_t x, int16_t y)
{
    // Check if the given position will collide with the snake's body
    for (uint8_t i = 1; i < snakeLength; i++)
    {
        if (x == snake[i].x && y == snake[i].y)
        {
            return true;
        }
    }
    return false;
}

void updateDirection()
{
    // Simple strategy to move towards apple
    if (snakeDirection % 2 == 0)
    { // currently moving vertically
        // Only change direction if snake is horizontally aligned with the apple
        if (apple.y == snake[0].y)
        {
            if (apple.x > snake[0].x && snakeDirection != 3)
            {                       // prevent moving left if currently moving right
                snakeDirection = 1; // move right
            }
            else if (apple.x < snake[0].x && snakeDirection != 1)
            {                       // prevent moving right if currently moving left
                snakeDirection = 3; // move left
            }
        }
    }
    else
    { // currently moving horizontally
        // Only change direction if snake is vertically aligned with the apple
        if (apple.x == snake[0].x)
        {
            if (apple.y > snake[0].y && snakeDirection != 0)
            {                       // prevent moving up if currently moving down
                snakeDirection = 2; // move down
            }
            else if (apple.y < snake[0].y && snakeDirection != 2)
            {                       // prevent moving down if currently moving up
                snakeDirection = 0; // move up
            }
        }
    }
    // Predict next position of snake's head
    Point nextPos = snake[0];
    if (snakeDirection == 0)
    {
        nextPos.y--;
    }
    else if (snakeDirection == 1)
    {
        nextPos.x++;
    }
    else if (snakeDirection == 2)
    {
        nextPos.y++;
    }
    else if (snakeDirection == 3)
    {
        nextPos.x--;
    }
    // Check if next position will collide with snake's body
    for (uint8_t i = 1; i < snakeLength; i++)
    {
        if (nextPos.x == snake[i].x && nextPos.y == snake[i].y)
        {
            // If snake is moving vertically, try to turn left or right
            if (snakeDirection % 2 == 0)
            {
                if (snakeDirection != 1 && snake[0].x + 1 < 32 && !willCollide(snake[0].x + 1, snake[0].y))
                {
                    snakeDirection = 1; // turn right
                }
                else if (snakeDirection != 3 && snake[0].x - 1 >= 0 && !willCollide(snake[0].x - 1, snake[0].y))
                {
                    snakeDirection = 3; // turn left
                }
            }
            else
            { // If snake is moving horizontally, try to turn up or down
                if (snakeDirection != 0 && snake[0].y + 1 < 8 && !willCollide(snake[0].x, snake[0].y + 1))
                {
                    snakeDirection = 2; // turn down
                }
                else if (snakeDirection != 2 && snake[0].y - 1 >= 0 && !willCollide(snake[0].x, snake[0].y - 1))
                {
                    snakeDirection = 0; // turn up
                }
            }
        }
    }
}

void SnakeGame(FastLED_NeoMatrix *matrix, int16_t x, int16_t y)
{
    const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();
    if (isGameOver)
    {
        for (uint8_t i = 0; i < snakeLength; i++)
        {
            snake[i] = {0, i};
            shouldMove[i] = true; // all segments should move initially
        }
        snakeDirection = 1; // start moving right
        isGameOver = false;
        snakeLength = INITIAL_SNAKE_LENGTH;
        apple = {(int16_t)random(32), (int16_t)random(8)}; // spawn apple at random position
                                                           // Initialize color index for each segment
        for (uint8_t i = 0; i < snakeLength; i++)
        {
            snake[i].colorIndex = colorIndex;
        }
    }
    if (millis() - lastMove >= SNAKE_SPEED)
    {
        lastMove = millis();
        // Update direction
        updateDirection();
        if (snake[0].x == apple.x && snake[0].y == apple.y)
        {
            if (snakeLength < MAX_SNAKE_LENGTH)
            {
                snake[snakeLength] = apple;      // add new tail segment
                shouldMove[snakeLength] = false; // new segment should not move initially
                snakeLength++;
            }
            apple = {(int16_t)random(32), (int16_t)random(8)}; // spawn new apple
        }
        // Move snake
        for (uint8_t i = snakeLength - 1; i > 0; i--)
        {
            if (shouldMove[i])
            {
                snake[i] = snake[i - 1];
            }
            else
            {
                shouldMove[i] = true; // new segment should start moving from next frame
            }
        }
        // Update head position based on direction
        if (snakeDirection == 0)
        {
            snake[0].y--;
        }
        else if (snakeDirection == 1)
        {
            snake[0].x++;
        }
        else if (snakeDirection == 2)
        {
            snake[0].y++;
        }
        else if (snakeDirection == 3)
        {
            snake[0].x--;
        }

        // Check for wall collision and wrap around
        if (snake[0].x < 0)
        {
            snake[0].x = cols - 1;
            colorIndex = (colorIndex + 1) % (sizeof(colors) / sizeof(colors[0]));
            snake[0].colorIndex = colorIndex; // Update color index for head segment
        }
        else if (snake[0].x >= cols)
        {
            snake[0].x = 0;
            colorIndex = (colorIndex + 1) % (sizeof(colors) / sizeof(colors[0]));
            snake[0].colorIndex = colorIndex; // Update color index for head segment
        }
        else if (snake[0].y < 0)
        {
            snake[0].y = rows - 1;
            colorIndex = (colorIndex + 1) % (sizeof(colors) / sizeof(colors[0]));
            snake[0].colorIndex = colorIndex; // Update color index for head segment
        }
        else if (snake[0].y >= rows)
        {
            snake[0].y = 0;
            colorIndex = (colorIndex + 1) % (sizeof(colors) / sizeof(colors[0]));
            snake[0].colorIndex = colorIndex; // Update color index for head segment
        }
        // Check for self-collision
        for (uint8_t i = 1; i < snakeLength; i++)
        {
            if (snake[0].x == snake[i].x && snake[0].y == snake[i].y)
            {
                isGameOver = true;
            }
        }
    }
    // Draw the snake
    for (uint8_t i = 0; i < snakeLength; i++)
    {
        matrix->drawPixel(x + snake[i].x, y + snake[i].y, colors[snake[i].colorIndex]);
    }
    // Draw the apple
    matrix->drawPixel(x + apple.x, y + apple.y, matrix->Color(255, 0, 0));
}

// ######## Fireworks ############
struct Firework
{
    float x, y;
    uint8_t life;
    bool exploded;
    CHSV color;
    uint16_t peak;
    float speed; // Add a speed variable to each firework
};

#define MAX_FIREWORKS 5
Firework fireworks[MAX_FIREWORKS];
uint32_t lastFireworkTime = 0;
uint32_t fireworkInterval = 350; // milliseconds between new fireworks

void Fireworks(FastLED_NeoMatrix *matrix, int16_t x, int16_t y)
{
    const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();

    // Spawn new firework if enough time has passed
    if (millis() - lastFireworkTime >= fireworkInterval && random(100) < 50)
    { // 50% chance to spawn firework
        for (uint8_t i = 0; i < MAX_FIREWORKS; i++)
        {
            if (fireworks[i].life == 0)
            { // find an unused firework
                fireworks[i].x = random(cols);
                fireworks[i].y = rows - 1;
                fireworks[i].life = 255;
                fireworks[i].exploded = false;
                fireworks[i].color = CHSV(0, 0, 255); // The unexploded firework is white
                fireworks[i].peak = random(1, 5);     // Set a random peak height for the firework
                fireworks[i].speed = 0.3;             // Set a speed for the firework
                lastFireworkTime = millis();
                break;
            }
        }
    }

    // Move, explode and fade out fireworks
    for (uint8_t i = 0; i < MAX_FIREWORKS; i++)
    {
        if (fireworks[i].life > 0)
        {
            fireworks[i].life -= 5; // decrease life by 5 each frame
            if (fireworks[i].y > fireworks[i].peak && !fireworks[i].exploded)
            {
                fireworks[i].y -= fireworks[i].speed; // Move the firework up by its speed
            }
            else
            {
                fireworks[i].exploded = true;
                fireworks[i].color = CHSV(random(255), 255, fireworks[i].life * 4); // The exploded firework has a random color
            }
            matrix->drawPixel(x + fireworks[i].x, y + fireworks[i].y, fireworks[i].color);
            // Draw a few pixels around the firework to simulate explosion
            if (fireworks[i].exploded)
            {
                if (fireworks[i].x > 0)
                    matrix->drawPixel(x + fireworks[i].x - 1, y + fireworks[i].y, fireworks[i].color);
                if (fireworks[i].x < cols - 1)
                    matrix->drawPixel(x + fireworks[i].x + 1, y + fireworks[i].y, fireworks[i].color);
                if (fireworks[i].y > 0)
                    matrix->drawPixel(x + fireworks[i].x, y + fireworks[i].y - 1, fireworks[i].color);
                if (fireworks[i].y < rows - 1)
                    matrix->drawPixel(x + fireworks[i].x, y + fireworks[i].y + 1, fireworks[i].color);
            }
        }
    }
}

// ######## Ripple ############

#define RIPPLE_SIZE 30

struct Ripple
{
    uint16_t x, y;
    float life;
    CHSV color;
};

Ripple ripple;

CRGB RippleLeds[32][8]; // temporary LEDs state

void RippleEffect(FastLED_NeoMatrix *matrix, int16_t x, int16_t y)
{
    const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();

    // If ripple has finished, spawn a new one at a random location
    if (ripple.life > RIPPLE_SIZE)
    {
        ripple.x = random(cols);
        ripple.y = random(rows);
        ripple.life = 0;
        ripple.color = CHSV(random(255), 255, 255); // random color with full saturation and value
    }

    // Apply pseudo-blur
    for (uint16_t i = 0; i < cols; i++)
    {
        for (uint16_t j = 0; j < rows; j++)
        {
            RippleLeds[i][j].fadeToBlackBy(45);
        }
    }

    // Draw the ripple
    for (uint16_t i = 0; i < cols; i++)
    {
        for (uint16_t j = 0; j < rows; j++)
        {
            // Calculate distance from the ripple center
            uint16_t dx = abs(i - ripple.x);
            uint16_t dy = abs(j - ripple.y);
            uint16_t dist = sqrt(dx * dx + dy * dy);

            // If this pixel is on the ripple ring, color it
            if (dist >= ripple.life && dist < ripple.life + 2)
            {
                RippleLeds[i][j] = ripple.color;
            }

            // Apply the temporary LEDs to the actual matrix
            matrix->drawPixel(x + i, y + j, RippleLeds[i][j]);
        }
    }

    // Increase the ripple size
    ripple.life += 0.5;
}

// ######## PlasmaCloud ############

void PlasmaCloudEffect(FastLED_NeoMatrix *matrix, int16_t x, int16_t y)
{
    const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();
    static float plasmaTime = 0;
    static float hueShift = 0;
    // For each pixel, calculate a noise value based on its position and the current time
    for (uint16_t i = 0; i < cols; i++)
    {
        for (uint16_t j = 0; j < rows; j++)
        {
            float noise = inoise8(i * 16, j * 16, plasmaTime);
            uint8_t hue = noise * 255 / 1024 + hueShift; // Map noise value to hue (0-255) and add hueShift
            CRGB color = CHSV(hue, 255, 255);
            matrix->drawPixel(x + i, y + j, color);
        }
    }

    // Increase time to animate the plasma clouds
    ++plasmaTime;

    // Increase hueShift to gradually change color over time
    hueShift += 0.1;
}

void CheckerboardEffect(FastLED_NeoMatrix *matrix, int16_t x, int16_t y)
{
    const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();

    static float colorIndex1 = 0;
    static float colorIndex2 = 0;
    CRGB color1 = ColorFromPalette(palette, colorIndex1);
    CRGB color2 = ColorFromPalette(palette, colorIndex2);

    // For each pixel, check if its x and y coordinates are both even or both odd
    for (uint16_t i = 0; i < cols; i++)
    {
        for (uint16_t j = 0; j < rows; j++)
        {
            if ((i % 2 == 0 && j % 2 == 0) || (i % 2 != 0 && j % 2 != 0))
            {
                matrix->drawPixel(x + i, y + j, color1);
            }
            else
            {
                matrix->drawPixel(x + i, y + j, color2);
            }
        }
    }

    // Increase colorIndex to change colors over time
    colorIndex1 += 0.6;
    colorIndex2 += 1; // Increment at a different rate for more variation
}

// ######## Raindrops ############

#define MAX_DROPS 10
#define DROP_SPEED 0.5

struct Drop
{
    float pos;  // Current position
    CRGB color; // Drop's color
};

Drop drops[MAX_DROPS];

void RaindropsEffect(FastLED_NeoMatrix *matrix, int16_t x, int16_t y)
{
    const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();
    matrix->fillScreen(0); // Clear the matrix each frame

    // Initialize drops with random positions and colors
    for (uint8_t i = 0; i < MAX_DROPS; i++)
    {
        if ((drops[i].color.r == 0 && drops[i].color.g == 0 && drops[i].color.b == 0) || drops[i].pos > rows)
        {
            // Check if the color is black
            drops[i].pos = 0;
            drops[i].color = CHSV(random(255), 255, 255);
        }
    }

    // Move each drop and draw it on the matrix
    for (uint8_t i = 0; i < MAX_DROPS; i++)
    {
        drops[i].pos += DROP_SPEED;
        if (drops[i].pos <= rows)
        {
            matrix->drawPixel(x + i, y + (int)drops[i].pos, drops[i].color);
        }
    }
}

uint8_t beamAngle = 0;
CRGB leds1[32][8];

void RadarEffect(FastLED_NeoMatrix *matrix, int16_t x, int16_t y)
{
    const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();
    const float centerX = (cols - 1) / 2.0;
    const float centerY = (rows - 1) / 2.0;

    // Fade the previous state of the leds
    for (uint16_t i = 0; i < cols; i++)
    {
        for (uint16_t j = 0; j < rows; j++)
        {
            leds1[i][j].fadeToBlackBy(20);
        }
    }

    // Draw the beam
    for (uint8_t r = 0; r <= max(cols, rows); r++)
    {
        uint16_t i = centerX + r * (cos8(beamAngle) - 128) / 128.0;
        uint16_t j = centerY - r * (sin8(beamAngle) - 128) / 128.0;
        if (i < cols && j < rows)
        {
            leds1[i][j] = CHSV(beamAngle, 255, 255);
        }
    }

    // Copy the temporary leds array to the actual matrix
    for (uint16_t i = 0; i < cols; i++)
    {
        for (uint16_t j = 0; j < rows; j++)
        {
            matrix->drawPixel(x + i, y + j, leds1[i][j]);
        }
    }

    // Increment the beam angle
    beamAngle += 2;
}

// ######## Balls ############
#define PADDLE_HEIGHT 3 // Height of the paddles
#define BALL_SIZE 1     // Size of the ball

struct Paddle
{
    int y;  // Y position of the paddle
    int dy; // Speed of the paddle
};

struct Ball
{
    int x, y;   // Position of the ball
    int dx, dy; // Speed of the ball
};

void PingPongEffect(FastLED_NeoMatrix *matrix, int16_t x, int16_t y)
{
    const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();

    static Paddle paddle1 = {rows / 2, 1};         // Paddle on the left side
    static Paddle paddle2 = {rows / 2, -1};        // Paddle on the right side
    static Ball ball = {cols / 2, rows / 2, 1, 1}; // Ball in the middle of the matrix
    static uint32_t lastUpdate = 0;
    uint8_t speed = 80; // Geschwindigkeit des Spiels
    // Move the paddles

    if (millis() - lastUpdate > speed)
    {
        lastUpdate = millis();
        paddle1.y += paddle1.dy;
        paddle2.y += paddle2.dy;

        // If a paddle has reached the top or bottom edge, change its direction
        if (paddle1.y <= 0 || paddle1.y + PADDLE_HEIGHT >= rows)
        {
            paddle1.dy = -paddle1.dy;
        }
        if (paddle2.y <= 0 || paddle2.y + PADDLE_HEIGHT >= rows)
        {
            paddle2.dy = -paddle2.dy;
        }

        // Move the ball
        ball.x += ball.dx;
        ball.y += ball.dy;

        // If the ball has reached the top or bottom edge, change its direction
        if (ball.y <= 0 || ball.y + BALL_SIZE >= rows)
        {
            ball.dy = -ball.dy;
        }

        // If the ball has reached the left or right edge without hitting a paddle, restart the game
        if ((ball.x < 0 && (ball.y < paddle1.y || ball.y > paddle1.y + PADDLE_HEIGHT)) ||
            (ball.x + BALL_SIZE > cols && (ball.y < paddle2.y || ball.y > paddle2.y + PADDLE_HEIGHT)))
        {
            ball.x = cols / 2; // Reset ball position
            ball.y = rows / 2;
            ball.dx = 1; // Reset ball direction
            ball.dy = 1;
        }

        // If the ball has reached a paddle, change its direction
        if (ball.x <= 0 && ball.y >= paddle1.y && ball.y < paddle1.y + PADDLE_HEIGHT)
        {
            ball.dx = -ball.dx;
        }
        if (ball.x + BALL_SIZE >= cols && ball.y >= paddle2.y && ball.y < paddle2.y + PADDLE_HEIGHT)
        {
            ball.dx = -ball.dx;
        }
    }
    // Draw the paddles and the ball
    for (int i = 0; i < PADDLE_HEIGHT; i++)
    {
        matrix->drawPixel(x, y + paddle1.y + i, matrix->Color(255, 255, 255));
        matrix->drawPixel(x + cols - 1, y + paddle2.y + i, matrix->Color(255, 255, 255));
    }
    for (int i = 0; i < BALL_SIZE; i++)
    {
        matrix->drawPixel(x + ball.x, y + ball.y + i, matrix->Color(255, 0, 0));
    }
}

// ######## Bricks ############
#define PADDLE_WIDTH 3 // Width of the paddle
#define BALL_SIZE 1    // Size of the ball
#define BRICK_COLS 16  // Number of bricks in a row
#define BRICK_ROWS 3   // Number of brick rows

struct BricksPaddle
{
    int x;  // X position of the paddle
    int dx; // Speed of the paddle
};

struct BricksBall
{
    int x, y;   // Position of the ball
    int dx, dy; // Speed of the ball
};

bool bricks[BRICK_ROWS][BRICK_COLS]; // State of the bricks

void resetBricks()
{
    for (int i = 0; i < BRICK_ROWS; i++)
    {
        for (int j = 0; j < BRICK_COLS; j++)
        {
            bricks[i][j] = true;
        }
    }
}

bool bricksRemain()
{
    for (int i = 0; i < BRICK_ROWS; i++)
    {
        for (int j = 0; j < BRICK_COLS; j++)
        {
            if (bricks[i][j])
            {
                return true;
            }
        }
    }
    return false;
}

void BrickBreakerEffect(FastLED_NeoMatrix *matrix, int16_t x, int16_t y)
{
    const uint16_t cols = matrix->width();
    const uint16_t rows = matrix->height();

    static BricksPaddle paddle = {cols / 2, 1};          // Paddle in the middle of the matrix
    static BricksBall ball = {cols / 2, rows / 2, 1, 1}; // Ball in the middle of the matrix

    // Initialize the bricks
    static bool firstTime = true;
    if (firstTime)
    {
        for (int i = 0; i < BRICK_ROWS; i++)
        {
            for (int j = 0; j < BRICK_COLS; j++)
            {
                bricks[i][j] = true; // All bricks exist at the beginning
            }
        }
        firstTime = false;
    }

    static uint32_t lastUpdate = 0;
    if (millis() - lastUpdate > 100)
    {
        lastUpdate = millis();

        // Move the ball
        ball.x += ball.dx;
        ball.y += ball.dy;

        // Move the paddle
        paddle.x = ball.x-1;

        // If the paddle has reached the left or right edge, change its direction
        if (paddle.x <= 0 || paddle.x + PADDLE_WIDTH >= cols)
        {
            paddle.dx = -paddle.dx;
        }

        // If the ball has reached the top, bottom, left, or right edge, change its direction
        if (ball.x <= 0 || ball.x + BALL_SIZE >= cols)
        {
            ball.dx = -ball.dx;
        }
        if (ball.y <= 0 || ball.y + BALL_SIZE >= rows)
        {
            ball.dy = -ball.dy;
        }

        for (int i = 0; i < BRICK_ROWS; i++)
        {
            for (int j = 0; j < BRICK_COLS; j++)
            {
                if (bricks[i][j] && ball.y == i && ball.x >= j * (cols / BRICK_COLS) && ball.x < (j + 1) * (cols / BRICK_COLS))
                {
                    bricks[i][j] = false;
                    ball.dy = -ball.dy;
                }
            }
        }

        // If no bricks remain, reset the bricks
        if (!bricksRemain())
        {
            resetBricks();
        }
    }
    // Draw the paddle, the ball, and the bricks
    for (int i = 0; i < PADDLE_WIDTH; i++)
    {
        matrix->drawPixel(x + paddle.x + i, y + rows - 1, matrix->Color(255, 255, 255));
    }
    for (int i = 0; i < BALL_SIZE; i++)
    {
        matrix->drawPixel(x + ball.x, y + ball.y + i, matrix->Color(255, 0, 0));
    }
    for (int i = 0; i < BRICK_ROWS; i++)
    {
        for (int j = 0; j < BRICK_COLS; j++)
        {
            if (bricks[i][j])
            {
                matrix->drawPixel(x + j * (cols / BRICK_COLS), y + i, matrix->Color(0, 0, 255));
            }
        }
    }
}

Effect effects[] = {
    {"BrickBreaker", BrickBreakerEffect},
    {"PingPong", PingPongEffect},
    {"Radar", RadarEffect},
    {"Raindrops", RaindropsEffect},
    {"Checkerboard", CheckerboardEffect},
    {"Fireworks", Fireworks},
    {"PlasmaCloud", PlasmaCloudEffect},
    {"Ripple", RippleEffect},
    {"Snake", SnakeGame},
    {"Pacifica", Pacifica},
    {"TheaterChase", TheaterChase},
    {"Plasma", Plasma},
    {"Matrix", Matrix},
    {"SwirlIn", SwirlIn},
    {"SwirlOut", SwirlOut},
    {"LookingEyes", LookingEyes},
    {"TwinklingStars", TwinklingStars},
    {"ColorWaves", ColorWaves}};

void callEffect(FastLED_NeoMatrix *matrix, int16_t x, int16_t y, int index)
{
    if (index >= 0)
    {
        effects[index].func(matrix, x, y);
    }
}

int getEffectIndex(String name)
{
    for (uint8_t i = 0; i < sizeof(effects) / sizeof(effects[0]); i++)
    {
        if (effects[i].name == name)
        {
            return i;
        }
    }
    return -1;
}