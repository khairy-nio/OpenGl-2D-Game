#include <windows.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <stdlib.h>

// Game state
enum GameState {
    START,
    CHAR_SELECT,
    PLAYING,
    GAME_OVER
};

GameState gameState = START;

// Game variables
int score = 0;
int highScore = 0;
int selectedCharacter = 0; // 0=Anger, 1=Happy, 2=Sad
bool paused = false;
bool newHighScore = false;
float gameTime = 20.0f;
time_t lastTime;
float gameSpeed = 1.0f;
bool isBackgroundPlaying = false; // Track background music state

// Character position and scaling
float translateX = 0.0f, translateY = 0.0f;
float scaleX = 1.0f, scaleY = 1.0f;

// Anger character position for collision (when not selected)
const float ANGER_X = 0.5f;
const float ANGER_Y = 0.5f;

// Game objects
struct Point { float x, y; };

struct Fireball {
    Point center;
    float speed;
    float size;
};

struct Token {
    Point position;
    bool collected;
};

Token tokens[10];
std::vector<Fireball> fireballs;

// File handling for high score
void saveHighScore() {
    FILE* file = fopen("highscore.txt", "w");
    if (file) {
        fprintf(file, "%d", highScore);
        fclose(file);
    }
}

void loadHighScore() {
    FILE* file = fopen("highscore.txt", "r");
    if (file) {
        fscanf(file, "%d", &highScore);
        fclose(file);
    } else {
        highScore = 0;
    }
}

// Drawing functions
void drawCircle(float x, float y, float radius) {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 360; i++) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(x + radius * cos(angle), y + radius * sin(angle));
    }
    glEnd();
}

void drawFilledCircle(float x, float y, float radius) {
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(x + radius * cos(angle), y + radius * sin(angle));
    }
    glEnd();
}

void drawLine(float x1, float y1, float x2, float y2) {
    glBegin(GL_LINES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

void drawAngerCharacter(float x, float y) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(0.125f, 0.125f, 1.0f);
    glScalef(scaleX, scaleY, 1.0f);

    // Head
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-0.2f, 0.45f);
    glVertex2f(0.2f, 0.45f);
    glVertex2f(0.2f, 0.2f);
    glVertex2f(-0.2f, 0.2f);
    glEnd();

    // Eyes
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(-0.15f, 0.345f);
    glVertex2f(-0.05f, 0.345f);
    glVertex2f(-0.05f, 0.32f);
    glVertex2f(-0.15f, 0.32f);
    glVertex2f(0.05f, 0.345f);
    glVertex2f(0.15f, 0.345f);
    glVertex2f(0.15f, 0.32f);
    glVertex2f(0.05f, 0.32f);
    glEnd();

    // Mouth
    glBegin(GL_QUADS);
    glVertex2f(-0.04f, 0.28f);
    glVertex2f(0.04f, 0.28f);
    glVertex2f(0.04f, 0.24f);
    glVertex2f(-0.04f, 0.24f);
    glEnd();

    // Body
    glColor3f(0.5f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(-0.18f, 0.084f);
    glVertex2f(0.18f, 0.084f);
    glVertex2f(0.18f, -0.2f);
    glVertex2f(-0.18f, -0.2f);
    glEnd();

    // Legs
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-0.18f, -0.2f);
    glVertex2f(-0.08f, -0.2f);
    glVertex2f(-0.08f, -0.25f);
    glVertex2f(-0.18f, -0.25f);
    glVertex2f(0.08f, -0.2f);
    glVertex2f(0.18f, -0.2f);
    glVertex2f(0.18f, -0.25f);
    glVertex2f(0.08f, -0.25f);
    glEnd();

    glPopMatrix();
}

void drawHappyCharacter(float x, float y) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(0.125f, 0.125f, 1.0f);
    glScalef(scaleX, scaleY, 1.0f);

    // Head (yellow)
    glColor3f(1.0f, 1.0f, 0.0f);
    drawFilledCircle(0.0f, 0.0f, 0.5f);

    // Eyes
    glColor3f(0.0f, 0.0f, 0.0f);
    drawFilledCircle(-0.2f, 0.1f, 0.08f);
    drawFilledCircle(0.2f, 0.1f, 0.08f);

    // Eyebrows
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 20; i++) {
        float t = i / 20.0f;
        float angle = -0.3f + t * 0.6f;
        glVertex2f(-0.25f + t * 0.2f, 0.2f + 0.05f * sin(angle));
    }
    glEnd();
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 20; i++) {
        float t = i / 20.0f;
        float angle = -0.3f + t * 0.6f;
        glVertex2f(0.05f + t * 0.2f, 0.2f + 0.05f * sin(angle));
    }
    glEnd();

    // Blush
    glColor3f(1.0f, 0.6f, 0.6f);
    drawFilledCircle(-0.2f, -0.05f, 0.05f);
    drawFilledCircle(0.2f, -0.05f, 0.05f);

    // Smile
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, -0.1f);
    for (int i = 0; i <= 180; i += 5) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(0.3f * cos(angle), -0.1f + 0.15f * sin(angle));
    }
    glEnd();

    glPopMatrix();
}

void drawSadCharacter(float x, float y) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glScalef(0.125f, 0.125f, 1.0f);
    glScalef(scaleX, scaleY, 1.0f);

    // Head (blue)
    glColor3f(0.0f, 0.6f, 1.0f);
    drawFilledCircle(0.0f, 0.0f, 0.5f);

    // Eyes
    glColor3f(0.0f, 0.0f, 0.0f);
    drawFilledCircle(-0.2f, 0.1f, 0.1f);
    drawFilledCircle(0.2f, 0.1f, 0.1f);

    // Eyebrows
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 20; i++) {
        float t = i / 20.0f;
        float angle = 0.3f - t * 0.6f;
        glVertex2f(-0.25f + t * 0.2f, 0.2f - 0.05f * sin(angle));
    }
    glEnd();
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 20; i++) {
        float t = i / 20.0f;
        float angle = 0.3f - t * 0.6f;
        glVertex2f(0.05f + t * 0.2f, 0.2f - 0.05f * sin(angle));
    }
    glEnd();

    // Tears
    glColor3f(0.0f, 0.8f, 1.0f);
    drawFilledCircle(-0.2f, -0.05f, 0.05f);
    drawFilledCircle(0.2f, -0.05f, 0.05f);

    // Frown
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, -0.1f);
    for (int i = 180; i >= 0; i -= 5) {
        float angle = i * 3.14159f / 180.0f;
        glVertex2f(0.3f * cos(angle), -0.1f - 0.15f * sin(angle));
    }
    glEnd();

    glPopMatrix();
}

void drawBackground() {
    glPushMatrix();
    glColor3f(0.1f, 0.1f, 0.3f);
    for (float x = -1.0f; x <= 1.0f; x += 0.2f) {
        for (float y = -1.0f; y <= 1.0f; y += 0.2f) {
            drawFilledCircle(x, y, 0.03f);
        }
    }
    glPopMatrix();
}

void drawTimeMeter() {
    glColor3f(0.0f, 0.7f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(-0.9f, 0.8f);
    glVertex2f(-0.9f + (gameTime / 20.0f) * 0.8f, 0.8f);
    glVertex2f(-0.9f + (gameTime / 20.0f) * 0.8f, 0.75f);
    glVertex2f(-0.9f, 0.75f);
    glEnd();
}

void drawText(float x, float y, const char* text) {
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);
    }
}

void drawStartScreen() {
    drawBackground();
    drawText(-0.3f, 0.3f, "Starting Game ...");
    drawText(-0.3f, 0.1f, "Press S to Start");
    drawText(-0.3f, -0.1f, "Use Arrows to Move");
    drawText(-0.3f, -0.3f, "Collect Blue Tokens");
    char highScoreText[50];
    sprintf(highScoreText, "High Score: %d", highScore);
    drawText(-0.3f, -0.5f, highScoreText);
}

void drawCharSelectScreen() {
    drawBackground();
    drawText(-0.4f, 0.8f, "Select Your Character");

    // Anger character option
    if (selectedCharacter == 0) {
        glColor3f(0.0f, 1.0f, 0.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(-0.8f, 0.5f);
        glVertex2f(-0.3f, 0.5f);
        glVertex2f(-0.3f, -0.1f);
        glVertex2f(-0.8f, -0.1f);
        glEnd();
    }
    drawAngerCharacter(-0.55f, 0.2f);
    drawText(-0.65f, -0.2f, "Anger");

    // Happy character option
    if (selectedCharacter == 1) {
        glColor3f(0.0f, 1.0f, 0.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(-0.2f, 0.5f);
        glVertex2f(0.3f, 0.5f);
        glVertex2f(0.3f, -0.1f);
        glVertex2f(-0.2f, -0.1f);
        glEnd();
    }
    drawHappyCharacter(0.05f, 0.2f);
    drawText(-0.05f, -0.2f, "Happy");

    // Sad character option
    if (selectedCharacter == 2) {
        glColor3f(0.0f, 1.0f, 0.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(0.4f, 0.5f);
        glVertex2f(0.9f, 0.5f);
        glVertex2f(0.9f, -0.1f);
        glVertex2f(0.4f, -0.1f);
        glEnd();
    }
    drawSadCharacter(0.65f, 0.2f);
    drawText(0.6f, -0.2f, "Sad");

    drawText(-0.3f, -0.6f, "Press ENTER to Select");
    drawText(-0.3f, -0.8f, "Use LEFT/RIGHT to Choose");
    char highScoreText[50];
    sprintf(highScoreText, "High Score: %d", highScore);
    drawText(-0.3f, -1.0f, highScoreText);
}

void drawGameOverScreen() {
    drawBackground();
    char scoreText[50];
    sprintf(scoreText, "Final Score: %d", score);
    drawText(-0.3f, 0.2f, scoreText);
    char highScoreText[50];
    sprintf(highScoreText, "High Score: %d", highScore);
    drawText(-0.3f, 0.0f, highScoreText);
    drawText(-0.3f, -0.2f, "Game Over!");
    if (newHighScore) {
        drawText(-0.3f, -0.4f, "New High Score!");
    }
    drawText(-0.3f, -0.6f, "Press R to Restart");
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (gameState == START) {
        drawStartScreen();
    }
    else if (gameState == CHAR_SELECT) {
        drawCharSelectScreen();
    }
    else if (gameState == PLAYING) {
        drawBackground();
        if (paused) {
            drawText(-0.2f, 0.0f, "Paused - Press R to Resume");
        } else {
            // Draw selected character
            if (selectedCharacter == 0) {
                drawAngerCharacter(translateX, translateY);
            } else if (selectedCharacter == 1) {
                drawHappyCharacter(translateX, translateY);
            } else if (selectedCharacter == 2) {
                drawSadCharacter(translateX, translateY);
            }

            // Draw tokens
            glColor3f(0.0f, 0.0f, 1.0f);
            for (int i = 0; i < 10; i++) {
                if (!tokens[i].collected) {
                    drawFilledCircle(tokens[i].position.x, tokens[i].position.y, 0.05f);
                }
            }

            // Draw fireballs
            glColor3f(1.0f, 0.0f, 0.0f);
            for (size_t i = 0; i < fireballs.size(); i++) {
                glPushMatrix();
                glTranslatef(fireballs[i].center.x, fireballs[i].center.y, 0.0f);
                glScalef(fireballs[i].size, fireballs[i].size, 1.0f);
                drawFilledCircle(0.0f, 0.0f, 0.05f);
                glPopMatrix();
            }

            // Draw time meter
            drawTimeMeter();

            // Display time and score
            char timeText[20];
            sprintf(timeText, "Time: %.1f", gameTime);
            drawText(0.7f, 0.9f, timeText);

            char scoreText[20];
            sprintf(scoreText, "Score: %d", score);
            drawText(0.7f, 0.85f, scoreText);
        }
    } else if (gameState == GAME_OVER) {
        drawGameOverScreen();
    }

    glutSwapBuffers();
}

void update(int value) {
    if (gameState == PLAYING && !paused) {
        // Start background music if not playing
        if (!isBackgroundPlaying) {
            PlaySound(TEXT("background.wav"), NULL, SND_ASYNC | SND_LOOP | SND_FILENAME);
            isBackgroundPlaying = true;
        }

        // Update game time
        time_t currentTime = time(NULL);
        if (lastTime != 0) {
            gameTime -= difftime(currentTime, lastTime) * gameSpeed;
        }
        lastTime = currentTime;

        if (gameTime <= 0) {
            gameTime = 0;
            gameState = GAME_OVER;
            // Stop background music
            PlaySound(NULL, NULL, 0);
            isBackgroundPlaying = false;
            // Check for new high score
            if (score > highScore) {
                highScore = score;
                newHighScore = true;
                saveHighScore();
            }
        }

        // Move fireballs downward
        for (size_t i = 0; i < fireballs.size(); i++) {
            fireballs[i].center.y -= fireballs[i].speed * 0.016f; // 60 FPS
            if (fireballs[i].center.y < -1.0f) {
                fireballs[i].center.y = 1.0f;
                fireballs[i].center.x = ((float)rand() / RAND_MAX) * 1.8f - 0.9f;
            }
        }

        // Check token collection
        for (int i = 0; i < 10; i++) {
            if (!tokens[i].collected) {
                float dx = tokens[i].position.x - translateX;
                float dy = tokens[i].position.y - translateY;
                float distance = sqrt(dx * dx + dy * dy);
                if (distance < 0.1f) {
                    tokens[i].collected = true;
                    score += 10;
                    gameTime += 1.0f;
                    scaleX = 1.2f;
                    scaleY = 1.2f;
                    // Increase fireball speed
                    for (size_t j = 0; j < fireballs.size(); j++) {
                        fireballs[j].speed += 0.01f;
                    }
                    // Add new fireball (up to 20)
                    if (fireballs.size() < 20) {
                        Fireball newFireball;
                        newFireball.center.x = ((float)rand() / RAND_MAX) * 1.8f - 0.9f;
                        newFireball.center.y = 1.0f;
                        newFireball.speed = 0.5f + ((float)rand() / RAND_MAX) * 0.5f;
                        newFireball.size = 0.8f + ((float)rand() / RAND_MAX) * 0.4f;
                        fireballs.push_back(newFireball);
                    }
                    // Respawn token
                    tokens[i].position.x = ((float)rand() / RAND_MAX) * 1.6f - 0.8f;
                    tokens[i].position.y = ((float)rand() / RAND_MAX) * 1.6f - 0.8f;
                    tokens[i].collected = false;
                }
            }
        }

        // Check fireball collision
        for (size_t i = 0; i < fireballs.size(); i++) {
            float dx = fireballs[i].center.x - translateX;
            float dy = fireballs[i].center.y - translateY;
            float distance = sqrt(dx * dx + dy * dy);
            if (distance < 0.05f + (0.05f * fireballs[i].size)) {
                gameState = GAME_OVER;
                // Stop background music
                PlaySound(NULL, NULL, 0);
                isBackgroundPlaying = false;
                // Check for new high score
                if (score > highScore) {
                    highScore = score;
                    newHighScore = true;
                    saveHighScore();
                }
            }
        }

        // Check collision with Anger character (if not selected)
        if (selectedCharacter != 0) {
            float dx = ANGER_X - translateX;
            float dy = ANGER_Y - translateY;
            float distance = sqrt(dx * dx + dy * dy);
            if (distance < 0.2f) {
                gameState = GAME_OVER;
                // Stop background music
                PlaySound(NULL, NULL, 0);
                isBackgroundPlaying = false;
                // Check for new high score
                if (score > highScore) {
                    highScore = score;
                    newHighScore = true;
                    saveHighScore();
                }
            }
        }

        // Handle scaling animation
        if (scaleX > 1.0f) {
            scaleX -= 0.01f;
            scaleY -= 0.01f;
            if (scaleX < 1.0f) scaleX = scaleY = 1.0f;
        }

        // Clamp character position
        if (translateX > 0.9f) translateX = 0.9f;
        if (translateX < -0.9f) translateX = -0.9f;
        if (translateY > 0.9f) translateY = 0.9f;
        if (translateY < -0.9f) translateY = -0.9f;
    } else {
        // Stop background music if not in PLAYING state or paused
        if (isBackgroundPlaying && (gameState != PLAYING || paused)) {
            PlaySound(NULL, NULL, 0);
            isBackgroundPlaying = false;
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 27: // ESC key
            PlaySound(NULL, NULL, 0); // Stop all sounds
            isBackgroundPlaying = false;
            exit(0);
            break;
        case 's':
        case 'S':
            if (gameState == START) {
                gameState = CHAR_SELECT;
            }
            break;
        case 13: // Enter key
            if (gameState == CHAR_SELECT) {
                gameState = PLAYING;
                lastTime = time(NULL);
            }
            break;
        case 'r':
        case 'R':
            if (gameState == GAME_OVER) {
                gameState = START;
                score = 0;
                gameTime = 20.0f;
                translateX = translateY = 0.0f;
                scaleX = scaleY = 1.0f;
                newHighScore = false;
                isBackgroundPlaying = false; // Reset background music
                fireballs.clear();
                display(); // Reinitialize game objects
                glutPostRedisplay();
            } else if (gameState == PLAYING && paused) {
                paused = false;
                lastTime = time(NULL);
            }
            break;
        case 'p':
        case 'P':
            if (gameState == PLAYING) {
                paused = !paused;
                if (!paused) {
                    lastTime = time(NULL);
                } else {
                    lastTime = 0;
                }
            }
            break;
    }
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    if (gameState == CHAR_SELECT) {
        if (key == GLUT_KEY_LEFT) {
            selectedCharacter = (selectedCharacter - 1 + 3) % 3;
        } else if (key == GLUT_KEY_RIGHT) {
            selectedCharacter = (selectedCharacter + 1) % 3;
        }
    }
    else if (gameState == PLAYING && !paused) {
        switch(key) {
            case GLUT_KEY_UP:
                translateY += 0.05f;
                break;
            case GLUT_KEY_DOWN:
                translateY -= 0.05f;
                break;
            case GLUT_KEY_LEFT:
                translateX -= 0.05f;
                break;
            case GLUT_KEY_RIGHT:
                translateX += 0.05f;
                break;
        }
    }
    glutPostRedisplay();
}

void initGame() {
    for (int i = 0; i < 10; i++) {
        tokens[i].position.x = ((float)rand() / RAND_MAX) * 1.6f - 0.8f;
        tokens[i].position.y = ((float)rand() / RAND_MAX) * 1.6f - 0.8f;
        tokens[i].collected = false;
    }
    fireballs.clear();
    for (int i = 0; i < 5; i++) {
        Fireball fireball;
        fireball.center.x = ((float)rand() / RAND_MAX) * 1.8f - 0.9f;
        fireball.center.y = 1.0f;
        fireball.speed = 0.5f + ((float)rand() / RAND_MAX) * 0.5f;
        fireball.size = 0.8f + ((float)rand() / RAND_MAX) * 0.4f;
        fireballs.push_back(fireball);
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Character Escape Game");

    srand(time(NULL));
    loadHighScore();
    initGame();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(0, update, 0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);

    glutMainLoop();
    return 0;
}