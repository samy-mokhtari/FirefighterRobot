#include "robot.h"
#include "room.h"

s_robot initRobot(int x, int y) {
  s_robot robot;
  robot.pos = (s_pos){x, y};
  robot.moves = 0;
  robot.hasExtinguisher = 0;
  robot.fireDetected = 0;

  robot.status = STATUS_GO_TO_EXTINGUISHER;
  robot.healthPoints = DEFAULT_HP;
  robot.direction = UP;
  robot.lastFire = (s_pos) {-1,-1};

  return robot;
}

char displayRobot(s_robot* robot) {
  if(robot->hasExtinguisher == 1)
    return TILE_ROBOT_WITH_EXTINGUISHER;

  return TILE_ROBOT;
}

int isAtPos(s_robot* robot, int x, int y) {
  if(robot->pos.x == x && robot->pos.y == y)
    return 1;

  return 0;
}

vector getPath(s_room* room, s_pos origin, s_pos dest) {
  int pathFound = 0;
  s_node* last = 0;
  s_node* currentNode = &(room->nodes[origin.y][origin.x]);

  for(int i=0; i<room->sizeX; i++)
    for(int j=0; j<room->sizeY; j++)
      room->nodes[j][i].parent = NULL;

  for(int i=0; i<room->sizeX; i++) {
    for(int j=0; j<room->sizeY; j++) {
      s_node* node = &(room->nodes[j][i]);
      node->H = sqrt(pow(fabs(dest.x - i)*1.0, 2) + pow(fabs(dest.y - j)*1.0, 2));
    }
  }

  vector openedList;
  vector closedList;

  vector_init(&openedList);
  vector_init(&closedList);
  vector_add(&openedList, currentNode);

  while(!pathFound) {
    int currentIdx=0;
    currentNode = (s_node*)vector_get(&openedList, 0);

    for(int i=1; i < vector_total(&openedList); i++) {
      if(currentNode->H > ((s_node*)vector_get(&openedList, i))->H) {
        currentNode = (s_node*)vector_get(&openedList, i);
        currentIdx = i;
      }
    }
    vector_add(&closedList, currentNode);
    vector_delete(&openedList, currentIdx);

    if(currentNode->pos.x == dest.x && currentNode->pos.y == dest.y) {
      pathFound = 1;
      last = currentNode;
      break;
    }

    for(int i=-1; i<=1; i++) {
      for(int j=-1; j<=1; j++) {
        if(currentNode->pos.y + i < 0 || currentNode->pos.y + i >= room->sizeY || currentNode->pos.x + j < 0 || currentNode->pos.x +j >= room->sizeX)
          continue;

        if(fabs(i) == fabs(j))
          continue;

        int status = mustBeIgnored(&openedList, &closedList, &(room->nodes[currentNode->pos.y + i][currentNode->pos.x + j]));
        if(status == -1) {
          room->nodes[currentNode->pos.y + i][currentNode->pos.x + j].parent = currentNode;
          room->nodes[currentNode->pos.y + i][currentNode->pos.x + j].G = currentNode->G+1;
          vector_add(&openedList, &(room->nodes[currentNode->pos.y + i][currentNode->pos.x + j]));
        } else if(status != -2) {
          s_node* node = (s_node*)vector_get(&openedList, status);
          if(node->G > currentNode->G + 1) {
            node->G = currentNode->G + 1;
            node->parent = currentNode;
            vector_set(&openedList, status, node);
          }
        }
      }
    }

    if(vector_total(&openedList) == 0)
      pathFound = 2;
  }

  vector path;
  vector_init(&path);
  if(pathFound == 2) {
    vector_set(&path, 0, NULL);
    return path;
  }

  while(last != NULL) {
    vector_add(&path, last);
    last = last->parent;
  }

  vector_free(&openedList);
  vector_free(&closedList);

  return path;
}

vector getBestPath(s_room* room, s_pos origin, s_pos dest) {
  vector originToDest = getPath(room, origin, dest);
  vector destToOrigin = getPath(room, dest, origin);

  if(vector_total(&originToDest) <= vector_total(&destToOrigin)) {
    vector_free(&destToOrigin);
    return originToDest;
  }

  vector temp;
  vector_init(&temp);
  for(int i=vector_total(&destToOrigin)-1; i>=0; i--)
    vector_add(&temp, vector_get(&destToOrigin, i));

  vector_free(&originToDest);
  vector_free(&destToOrigin);

  return temp;
}

int mustBeIgnored(vector* op, vector* cl, s_node* node) {
  for(int i=0; i<vector_total(cl); i++)
    if(vector_get(cl, i) == node)
      return -2;

  if(node->symb == TILE_WALL)
    return -2;

  for(int i=0; i<vector_total(op); i++)
    if(vector_get(op, i) == node)
      return i;

  return -1;
}

int moveTo(s_room* room, vector* vect, int idx) {
  if(vector_get(vect, 0) == NULL) {
    printf("[CRITICAL] No path found\n");
    exit(EXIT_FAILURE);
  }

  s_pos currentPos = room->robot.pos;
  s_pos nextPos = ((s_node*)vector_get(vect, idx))->pos;

  if(currentPos.y > nextPos.y)
    moveRobot(room, UP);
  else if(currentPos.x < nextPos.x)
    moveRobot(room, RIGHT);
  else if(currentPos.y < nextPos.y)
    moveRobot(room, DOWN);
  else if(currentPos.x > nextPos.x)
    moveRobot(room, LEFT);


  //IF A SUPPRIMER
  if(nextPos.x != -1 && nextPos.y != -1) {
    if(room->nodes[nextPos.y][nextPos.x].symb == TILE_EMPTY) {
      addToRobotVision(room, (s_pos){nextPos.x, nextPos.y});
      for(int i=-1; i<=1; i++)
        for(int j=-1; j<=1; j++)
          if(fabs(i) != fabs(j))
            addToRobotVision(room, (s_pos){nextPos.x + i, nextPos.y + j});
    }

    room->robot.moves++;
    if(room->nodes[nextPos.y][nextPos.x].symb == TILE_EXTINGUISHER) {
      room->nodes[nextPos.y][nextPos.x].symb = ' ';
      room->robot.hasExtinguisher = 1;
      setEmptyTilesInteresting(room);
    }

    if(room->robot.hasExtinguisher && room->nodes[nextPos.y][nextPos.x].symb >= '1' && room->nodes[nextPos.y][nextPos.x].symb < '3') {
      room->robot.status = STATUS_WAIT_TO_EXIT;
      room->robot.healthPoints -= room->nodes[nextPos.y][nextPos.x].symb - '0';
      return -1;
    }

    if(room->nodes[nextPos.y][nextPos.x].symb >= '1' && room->nodes[nextPos.y][nextPos.x].symb <= '3') {
      room->nodes[nextPos.y][nextPos.x].robotVision = room->nodes[nextPos.y][nextPos.x].symb;
      if(room->robot.status == STATUS_SEARCH_FIRE && room->robot.hasExtinguisher) {
        room->robot.fireDetected = room->nodes[nextPos.y][nextPos.x].symb - '0';
        room->robot.status = STATUS_EXTINGUISH_FIRE;
        return -1;
      }
    }

    if(room->robot.hasExtinguisher && room->nodes[nextPos.y][nextPos.x].symb == TILE_FIRE_LVL3) {
      room->robot.status = STATUS_WAIT_TO_EXIT;
      room->nodes[nextPos.y][nextPos.x].symb = TILE_EXTINGUISHED_FIRE;

      return -1;
    }

    if(room->robot.hasExtinguisher
      && (room->nodes[nextPos.y][nextPos.x].symb == TILE_FIRE_LVL1
      || room->nodes[nextPos.y][nextPos.x].symb == TILE_FIRE_LVL2
      || room->nodes[nextPos.y][nextPos.x].symb == TILE_FIRE_LVL3)) {

      room->nodes[nextPos.y][nextPos.x].symb = TILE_EXTINGUISHED_FIRE;

      if(room->robot.hasExtinguisher && nextPos.x == room->robot.lastFire.x && nextPos.y == room->robot.lastFire.y) {
        if(room->robot.fireDetected == 3)
          room->robot.status = STATUS_WAIT_TO_EXIT;
        else
          room->robot.status = STATUS_EXTINGUISH_FIRE;

        return -1;
      } else {
        room->robot.status = STATUS_EXTINGUISH_FIRE;
      }
      return idx-1;
    }

    if(room->nodes[nextPos.y][nextPos.x].symb == TILE_EMPTY)
      room->nodes[nextPos.y][nextPos.x].robotVision = TILE_VISITED;
    else if(room->nodes[nextPos.y][nextPos.x].symb == TILE_FIRE_LVL1) {
      room->nodes[nextPos.y][nextPos.x].robotVision = TILE_FIRE_LVL1;
      room->robot.healthPoints -= 1;
      if(room->robot.fireDetected <= 1) {
        room->robot.fireDetected = 1;
        room->robot.lastFire = (s_pos) {nextPos.x, nextPos.y};
      }
    } else if(room->nodes[nextPos.y][nextPos.x].symb == TILE_FIRE_LVL2) {
      room->nodes[nextPos.y][nextPos.x].robotVision = TILE_FIRE_LVL2;
      room->robot.healthPoints -= 2;
      if(room->robot.fireDetected <= 2) {
        room->robot.lastFire = (s_pos) {nextPos.x, nextPos.y};
        room->robot.fireDetected = 2;
      }
    } else if(room->nodes[nextPos.y][nextPos.x].symb == TILE_FIRE_LVL3) {
      room->nodes[nextPos.y][nextPos.x].robotVision = TILE_FIRE_LVL3;
      room->robot.healthPoints -= 3;
      room->robot.lastFire = (s_pos) {nextPos.x, nextPos.y};
      room->robot.fireDetected = 3;
    }

  }

  if(idx == 0) {
    if(room->robot.fireDetected == 0) {
      room->robot.status = STATUS_DETERMINE_INTERESTING_POINT;
    } else
      room->robot.status = STATUS_GO_TO_FIRE;
  }

  return idx-1;
}


void addToRobotVision(s_room* room, s_pos pos) {
  if(pos.x < 0 || pos.y < 0 || pos.x >= room->sizeX || pos.y >= room->sizeY)
    return;

  if(room->nodes[pos.y][pos.x].symb == TILE_WALL)
    return;

  for(int i=-1; i<= 1; i++) {
    for(int j=-1; j<= 1; j++) {
      if(fabs(i) == fabs(j))
        continue;

      if(pos.x + i < 0 || pos.x + i >= room->sizeX || pos.y + j < 0 || pos.y + j >= room->sizeY)
        continue;

      if(room->nodes[pos.y + j][pos.x + i].robotVision == TILE_EMPTY)
        room->nodes[pos.y + j][pos.x + i].robotVision = TILE_NOFIRE;
    }
  }
}

//pos={x,y}
int getDistance(s_room* room, s_pos pos) {
  vector path = getBestPath(room, room->robot.pos, pos);
  if(vector_get(&path, 0) == NULL)
    return 1000000;

  int distance = vector_total(&path);
  vector_free(&path);

  return distance-1;
}
