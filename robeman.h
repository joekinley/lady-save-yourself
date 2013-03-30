#ifndef __ROBEMAN_H__
#define __ROBEMAN_H__

class Timer;

struct Robeman {
  int x;
  int y;
  int image;
  Robeman( int, int );
};

// yeah this is dirty, better should be an own include for that, don't care here
struct Bullet {
  int x;
  int y;
  int image;
  Bullet( int, int );
};

struct Particle {
  int x;
  int y;
  int image;
  int vX;
  int vY;
  Timer spriteTimer;
  Particle( int, int );
};

#endif