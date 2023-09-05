#pragma once


#include "Config.h"


#include <ode/ode.h>
#include <raylib.h>


#define MIN(a, b) ((a)<(b)? (a) : (b))
#define MAX(a, b) ((a)>(b)? (a) : (b))


const static int RENDER_WIDTH = 1280;
const static int RENDER_HEIGHT = 720;
//const static int RENDER_WIDTH = 256;
//const static int RENDER_HEIGHT = 144;

const static float GAME_WIDTH = 16;
const static float GAME_HEIGHT = 9;

const static int DEFAULT_WINDOW_WIDTH = 1280;
const static int DEFAULT_WINDOW_HEIGHT = 720;

const static  Vector2 ZERO_ZERO{0, 0};
const static Vector3 ZERO_ZERO_ZERO{0, 0, 0};

const float PLAYER_MOVEMENT_SPEED = 1250;
