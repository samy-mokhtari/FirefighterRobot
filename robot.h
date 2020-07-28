#ifndef ROBOT_H
#define ROBOT_H

#include "includes.h"

#define TILE_ROBOT 'R'
#define TILE_ROBOT_WITH_EXTINGUISHER 'P'

#define STATUS_GO_TO_EXTINGUISHER 1
#define STATUS_DETERMINE_INTERESTING_POINT 2
#define STATUS_SEARCH_FIRE 3
#define STATUS_GO_TO_FIRE 4
#define STATUS_EXTINGUISH_FIRE 5
#define STATUS_WAIT_TO_EXIT 6

#define DEFAULT_HP 10


s_robot initRobot(int x, int y);
char displayRobot(s_robot* robot);
int isAtPos(s_robot* robot, int x, int y);

vector getPath(s_room* room, s_pos origin, s_pos dest);
vector getBestPath(s_room* room, s_pos origin, s_pos dest);
int mustBeIgnored(vector* op, vector* cl, s_node* node);
void addToRobotVision(s_room* room, s_pos pos);
int moveTo(s_room* room, vector* vect, int idx);
int getDistance(s_room* room, s_pos pos);

void setEmptyTilesInteresting(s_room* room);
vector nextNodePath(s_room* room);

#endif
