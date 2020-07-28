#ifndef ROOM_H
#define ROOM_H

#include "includes.h"
#include "robot.h"
#include "display.h"

#define DEFAULT_ROOM "./plans/plan1.txt"
#define TILE_EMPTY ' '
#define TILE_WALL 'x'
#define TILE_START 'D'
#define TILE_EXTINGUISHER 'E'
#define TILE_FIRE_LVL1 '1'
#define TILE_FIRE_LVL2 '2'
#define TILE_FIRE_LVL3 '3'

#define TILE_VISITED '#'
#define TILE_NOFIRE '-'
#define TILE_POTENTIAL_FIRE '.'
#define TILE_INTERESTING '*'
#define TILE_EXTINGUISHED_FIRE '_'
#define TILE_BLACK 'B'

s_room loadRoom(char* mapFilename);
void displayRoom(s_room* room, int drawRobotVision);
int moveRobot(s_room* room, e_direction dir);
void resetFire(s_room* room);

#endif
