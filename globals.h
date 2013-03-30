#ifndef __GLOBALS_H__
#define __GLOBALS_H__

// overall game related
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;
const int FRAMES_PER_SECOND = 30;
const char GAME_TITLE[] = "LD20";

const int TILE_WIDTH = 32;
const int TILE_HEIGHT = 32;
const int IMAGE_WIDTH = 320;
const int IMAGE_HEIGHT = 320;

const int PLAYER_MAX_HEALTH = 10;
const int PLAYER_JUMP_DIST = 200; // in pixels
const int PLAYER_SPEED = 300; // in px per second
const int PLAYER_JUMP_SPEED = 100;
const int PLAYER_ANIMATION_TIME = 50; // in ms
enum PLAYER_MOVEMENT {
  PM_LEFT,
  PM_RIGHT,
  PM_STILL_LEFT,
  PM_STILL_RIGHT,
  PM_JUMP_LEFT,
  PM_JUMP_RIGHT
};
const int PLAYER_GRAVITATION = 300; // in px per second
const int PLAYER_BULLET_SPEED = 1000; // in px per second

const int ENEMY_SPEED = 150; // in px per second
const int PARTICLE_SPEED = 200; // in px per second

const int CAMERA_SPEED = 200; // in px per second

#endif