#include "room.h"

s_room loadRoom(char* mapFilename) {
  s_room room;

  printf("Loading room : %s\n", mapFilename);
  FILE* file = fopen(mapFilename, "r+");

  if(!file) {
    printf("[CRITICAL] Can't load room %s\n", mapFilename);
    exit(EXIT_FAILURE);
  }

  fscanf(file, "%d:%d\n", &(room.sizeX), &(room.sizeY));
  printf("Room size : %dx%d\n", room.sizeX, room.sizeY);

  room.nodes = malloc(room.sizeY*sizeof(s_node*));
  for(int i=0; i<room.sizeY; i++)
    room.nodes[i] = malloc(room.sizeX*sizeof(s_node));

  int startPosFound=0, extinguisherPosFound=0, fireCenterFound=0;

  char c;
  for(int i=0; i<room.sizeY; i++) {
    for(int j=0; j<room.sizeX+1; j++) {
      c = fgetc(file);
      if(c == '\n')
        continue;

      if(c == TILE_START) {
        if(!startPosFound)
          room.startPos = (s_pos){j, i};
        else
          c = ' ';

        startPosFound++;
      }

      if(c == TILE_EXTINGUISHER) {
        if(!extinguisherPosFound)
          room.extinguisherPos = (s_pos){j, i};
        else
          c = ' ';

        extinguisherPosFound++;
      }

      if(c == TILE_FIRE_LVL3) {
        if(fireCenterFound)
          c = ' ';

        fireCenterFound++;
      }

      room.nodes[i][j].symb = c;
      if(c != TILE_FIRE_LVL1 && c != TILE_FIRE_LVL2 && c !=TILE_FIRE_LVL3)
        room.nodes[i][j].robotVision = c;
      else
        room.nodes[i][j].robotVision = ' ';
      room.nodes[i][j].G = 0;
      room.nodes[i][j].H =0;
      room.nodes[i][j].parent = NULL;
      room.nodes[i][j].pos = (s_pos){j,i};
    }
  }

  if(startPosFound == 0) {
    printf("[CRITICAL] No starting position found.\n");
    exit(EXIT_FAILURE);
  } else if (startPosFound > 1) {
    printf("[WARNING] More than one starting position found. Copies have been deleted.\n");
  }

  if(extinguisherPosFound == 0) {
    printf("[CRITICAL] No extinguisher found\n");
    exit(EXIT_FAILURE);
  } else if (extinguisherPosFound > 1) {
    printf("[WARNING] More than one extinguisher found. Copies have been deleted\n");
  }

  if(fireCenterFound== 0) {
    printf("[CRITICAL] No fire center found\n");
    exit(EXIT_FAILURE);
  } else if (fireCenterFound > 1) {
    printf("[WARNING] More than one fire center found. Copies have been deleted\n");
  }

  printf("Starting position : (%d;%d)\n", room.startPos.x, room.startPos.y);
  printf("Extinguisher : (%d;%d)\n", room.extinguisherPos.x, room.extinguisherPos.y);
  printf("\n");

  room.robot = initRobot(room.startPos.x, room.startPos.y);
  addToRobotVision(&room, (s_pos){room.startPos.x, room.startPos.y});
  for(int i=-1; i<=1; i++)
    for(int j=-1; j<=1; j++)
      if(fabs(i) != fabs(j))
        addToRobotVision(&room, (s_pos){room.startPos.x + i, room.startPos.y + j});

  return room;
}

void displayRoom(s_room* room, int drawRobotVision) {
  for(int i=0; i<10; i++)
    printf("\n");

  for(int i=0; i<room->sizeY; i++) {
    for(int j=0; j<room->sizeX; j++) {
      if(isAtPos(&(room->robot), j, i))
        printf("%c", displayRobot(&(room->robot)));
      else
        printf("%c", room->nodes[i][j].symb);
    }

    printf("           ");
    for(int j=0; j<room->sizeX && drawRobotVision; j++) {
      if(isAtPos(&(room->robot), j, i))
        printf("%c", displayRobot(&(room->robot)));
      else
        printf("%c", room->nodes[i][j].robotVision);
    }

    printf("\n");
  }
  printf("Coords: (%2d;%2d)    Has extinguisher: %d      HP: %d\n", room->robot.pos.x, room->robot.pos.y, room->robot.hasExtinguisher, room->robot.healthPoints);
  printf("Moves : %d\n", room->robot.moves);
  printf("Fire : %d;%d\n", room->robot.lastFire.x, room->robot.lastFire.y);
}

int moveRobot(s_room* room, e_direction dir) {
  room->robot.direction = dir;
  int newX=room->robot.pos.x, newY=room->robot.pos.y;

  switch (dir) {
    case UP:
      newY--;
      break;

    case RIGHT:
      newX++;
      break;

    case DOWN:
      newY++;
      break;

    case LEFT:
      newX--;
      break;

    default:
      printf("[WARNING] Unknown direction\n");
  }


  if(newX >= 0 && newX < room->sizeX && newY>=0 && newY < room->sizeY) {
    if(room->nodes[newY][newX].symb != TILE_WALL) {
      room->robot.pos.x = newX;
      room->robot.pos.y = newY;
      return 1;
    }
  }

  printf("[WARNING] Can't go outside the map\n");
  return 0;
}
