#pragma once

#include <pebble.h>

// SETTINGS
#define KEY_INVERT 0
#define KEY_STAR_SPEED 1
#define KEY_FRAMERATE 2
#define KEY_STAR_DENSITY 3

#define STAR_SPEED_SLOW 1
#define STAR_SPEED_MEDIUM 5
#define STAR_SPEED_FAST 10
#define STAR_SPEED_MAX 10

#define FRAMERATE_POWER 1000
#define FRAMERATE_SMOOTH 100

// CONSTS
#define SCREEN_WIDTH 144
#define SCREEN_HEIGHT 168
#define SCREEN_CENTER_X 72
#define SCREEN_CENTER_Y 84

#define STARS_PER_FRAME 2
#define MAX_STARS 60
#define STAR_SIZE_START 1
#define STAR_SIZE_MAX 5
#define DISTANCE_TO_GROW 30
#define STAR_OFFSET_X 24
#define STAR_OFFSET_Y 12

#define FONT_SIZE 24
#define TIME_X (SCREEN_CENTER_X - (FONT_SIZE*2.5))
#define TIME_Y (SCREEN_CENTER_Y - (FONT_SIZE/2))
#define TIME_HEIGHT (FONT_SIZE*2)
#define TIME_WIDTH (FONT_SIZE*5)
    
struct Star
{
    int x;
    int y;
    float velx;
    float vely;
    short radius;
    bool visible;
};

static Window* window;
static Layer* canvas;
static AppTimer* timer;

static GFont mFont;
static char* mTimeText;

struct Star* stars[MAX_STARS];
GColor mBackColor;
GColor mForeColor;

int mVisibleStars;
int mStarSpeed;
int mFramerate;

bool mInvertColors;