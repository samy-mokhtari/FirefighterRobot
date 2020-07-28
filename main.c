#include "includes.h"
#include "room.h"

int main(int argc, char** argv) {
  int stop = 0;

  srand(time(NULL));

  char file[MAX_PATH];
  printf("File > ");
  scanf("%s", file);

  s_SDLData SDLData = initSDL();

  s_room room = loadRoom(file);
  vector path = getBestPath(&room, room.startPos, room.extinguisherPos);
  float timeBeforeExit = 0;

  displayRoom(&room, 0);
  displayScreen(&SDLData, &room);

  int idx = vector_total(&path)-2;

  SDL_Event event;

  while(!stop) {
      displayRoom(&room, 1);
      displayScreen(&SDLData, &room);

      if(room.robot.healthPoints <= 0) {
        printf("The robot died\n");
        stop = 1;
      }

      if(room.robot.status == STATUS_GO_TO_EXTINGUISHER) {
        if(idx >= 0)
          idx = moveTo(&room, &path, idx);
        else
          printf("[WARNING] Unpredicted behavior\n");
      }
      else if(room.robot.status == STATUS_DETERMINE_INTERESTING_POINT) {
        vector_free(&path);
        path = nextNodePath(&room);

        idx = vector_total(&path)-2;
        printf("%d\n", idx);
      }
      else if(room.robot.status == STATUS_SEARCH_FIRE) {
        if(idx >= 0)
          idx = moveTo(&room, &path, idx);
        else {
          if(room.robot.hasExtinguisher && room.robot.pos.x == room.robot.lastFire.x && room.robot.pos.y == room.robot.lastFire.y) {
            room.robot.status = STATUS_EXTINGUISH_FIRE;
            if(room.nodes[room.robot.pos.y][room.robot.pos.x].symb == TILE_FIRE_LVL1)
              room.nodes[room.robot.pos.y][room.robot.pos.x].robotVision = TILE_FIRE_LVL1;
          } else {
            printf("[WARNING] Unpredicted behavior\n");
            exit(1);
          }
        }
      } else if(room.robot.status == STATUS_GO_TO_FIRE) {
        vector_free(&path);
        path = getBestPath(&room, room.robot.pos, room.robot.lastFire);

        idx = vector_total(&path) - 2;
        room.robot.status = STATUS_SEARCH_FIRE;
      } else if (room.robot.status == STATUS_EXTINGUISH_FIRE) {
        room.robot.status = STATUS_WAIT_TO_EXIT;

        for(int i=0; i<room.sizeX; i++)
          for(int j=0; j<room.sizeY; j++)
            if(room.nodes[j][i].symb == TILE_FIRE_LVL1 || room.nodes[j][i].symb == TILE_FIRE_LVL2 || room.nodes[j][i].symb == TILE_FIRE_LVL3) {
              room.nodes[j][i].robotVision = room.nodes[j][i].symb;
              room.nodes[j][i].symb = TILE_EXTINGUISHED_FIRE;
            }

      } else if (room.robot.status == STATUS_WAIT_TO_EXIT) {
        for(int i=0; i<room.sizeX; i++)
          for(int j=0; j<room.sizeY; j++)
            if(room.nodes[j][i].symb == TILE_FIRE_LVL1 || room.nodes[j][i].symb == TILE_FIRE_LVL2 || room.nodes[j][i].symb == TILE_FIRE_LVL3) {
              room.nodes[j][i].robotVision = room.nodes[j][i].symb;
              room.nodes[j][i].symb = TILE_EXTINGUISHED_FIRE;
            }

        timeBeforeExit += 0.1;
        if(timeBeforeExit > 10)
          stop=1;
      }
      else {
        stop = 1;
      }

      if(SDL_PollEvent(&event)) {
        switch (event.type) {
          case SDL_QUIT:
            stop = 1;
            break;
        }
      }

      SDL_Delay(100);
  }

  vector_free(&path);

  return EXIT_SUCCESS;
}
