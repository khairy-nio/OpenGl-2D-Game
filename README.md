# Character Escape Game

## Overview

**Character Escape Game is a 2D OpenGL-based game developed in C++. Players choose one of three characters (Anger, Happy, or Sad) and navigate a 2D space to collect blue tokens while avoiding red fireballs. The game features a time limit, scoring system, high score tracking, and background music. The objective is to collect as many tokens as possible within 20 seconds to achieve a high score.**

## Setup Instructions

### Prerequisites:
- A C++ compiler (e.g., GCC, MSVC) with support for Windows.
- OpenGL and GLUT libraries installed.
- Windows operating system (due to Windows.h and PlaySound usage).
- A `background.wav` audio file in the same directory as the executable for background music.

### Running the Game:
1. Place `background.wav` in the same directory as the compiled executable.

## Gameplay

The game progresses through four states:
1. **Start**: Displays instructions and high score. Press `S` to proceed.
2. **Character Select**: Choose a character (Anger, Happy, or Sad) using `LEFT`/`RIGHT` arrow keys. Press `ENTER` to start playing.
3. **Playing**: Control the selected character to collect blue tokens (adds 10 points and 1 second to the timer) while avoiding fireballs and the Anger character (if not selected). The game ends if the timer reaches 0 or a collision occurs.
4. **Game Over**: Shows the final score and high score. Press `R` to restart.

## Objectives
- Collect blue tokens to increase score and extend game time.
- Avoid red fireballs and the Anger character (if not selected) to prevent game over.
- Achieve a high score, which is saved to `highscore.txt`.

## Controls

### Start Screen:
- `S`: Start the game.

### Character Select:
- `LEFT`/`RIGHT`: Switch between characters.
- `ENTER`: Confirm selection and start playing.

### Playing:
- `UP`/`DOWN`/`LEFT`/`RIGHT`: Move the character.
- `P`: Pause/resume the game.
- `R`: Resume from pause.

### Game Over:
- `R`: Restart the game.

### General:
- `ESC`: Exit the game.

## Dependencies
- `Windows.h`: For Windows-specific functions (e.g., `PlaySound`).
- `OpenGL/GLUT`: For rendering graphics (`GL/glut.h`).
- `C++ Standard Library`: For `vector`, `stdio.h`, `time.h`, `stdlib.h`.
- `winmm.lib`: For audio playback (linked with `-lwinmm`).

## File Handling
- `highscore.txt`: Stores the high score. Created/updated when a new high score is achieved, loaded at startup.
- `background.wav`: Audio file for background music, played in a loop during the Playing state. Ensure this file exists in the executable's directory.

## Notes
- The game is designed for a Windows environment due to the use of `Windows.h` and `PlaySound`.
- Fireballs increase in speed and number as tokens are collected, increasing difficulty.
- The game window is 800x600 pixels, with a 2D orthographic projection.

### Collaborator
[Mohamed khiary](https://github.com/khairy-nio/)
[Abdelhamed Farhat](https://github.com/MideoHub)