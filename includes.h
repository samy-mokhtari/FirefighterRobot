#ifndef INCLUDES_H
#define INCLUDES_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>

//Library Vector by Ed Mann
//https://eddmann.com/posts/implementing-a-dynamic-vector-array-in-c/
#include "vector.h"

typedef enum e_direction {
  UP, RIGHT, DOWN, LEFT
} e_direction;

typedef struct s_pos {
  int x,y;
} s_pos;

typedef struct s_node s_node;
struct s_node {
  char symb;
  char robotVision;
  s_pos pos;

  int G;
  float H;
  s_node* parent;
};

typedef struct s_robot {
  s_pos pos;
  int healthPoints;

  int hasExtinguisher;
  int moves;
  int fireDetected;
  int status;
  s_pos lastFire;

  e_direction direction;
} s_robot;

typedef struct s_room {
  int sizeX, sizeY;
  s_node** nodes;

  s_pos startPos;
  s_pos extinguisherPos;

  s_robot robot;
} s_room;

#endif
