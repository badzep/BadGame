#ifndef BADGAME_CONSTANTS_H
#define BADGAME_CONSTANTS_H


#include <ode/ode.h>
#include <raylib.h>
#include "Config.h"


#define MIN(a, b) ((a)<(b)? (a) : (b))
#define MAX(a, b) ((a)>(b)? (a) : (b))


const static int RENDER_WIDTH = 1280;
const static int RENDER_HEIGHT = 720;

const static float GAME_WIDTH = 16;
const static float GAME_HEIGHT = 9;

const static int DEFAULT_WINDOW_WIDTH = 800;
const static int DEFAULT_WINDOW_HEIGHT = 450;

const static  Vector2 ZERO_ZERO{0, 0};
const static Vector3 ZERO_ZERO_ZERO{0, 0, 0};

const float PLAYER_MOVEMENT_SPEED = 1250;

#endif //BADGAME_CONSTANTS_H
