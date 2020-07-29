#include "display.h"

s_SDLData initSDL() {
  s_SDLData data;

  if(SDL_Init(SDL_INIT_VIDEO) == -1) {
    printf("[CRITICAL] Error while loading SDL : %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }
  if(TTF_Init() == -1) {
    printf("[CRITICAL] Error while loading SDL_TTF : %s\n", TTF_GetError());
    exit(EXIT_FAILURE);
  }
  if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) {
   printf("[CRITICAL] Error while loading SDL_Mixer : %s\n", Mix_GetError());
  }

  data.window = NULL;
  data.window = SDL_SetVideoMode(WINDOW_X, WINDOW_Y, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
  if(!data.window) {
    printf("[CRITICAL] Error while creating the window : %s\n", SDL_GetError());
    exit(EXIT_FAILURE);
  }

  SDL_WM_SetCaption("Firefighter Robot - Projet de fin d'année CIR 1 ISEN Yncrea Rennes | Alexandre THOMAS", NULL);
  data.icon = IMG_Load(GRAPHICS_ICON);
  SDL_WM_SetIcon(data.icon, NULL);

  data.mapTileset = IMG_Load(GRAPHICS_MAP_TILESET);
  data.robotVisionTileset = IMG_Load(GRAPHICS_ROBOT_VISION_TILESET);
  data.robotSpritesheet = IMG_Load(GRAPHICS_ROBOT_SPRITESHEET);
  data.fireAnimation = IMG_Load(GRAPHICS_FIRE_ANIMATION);
  data.fireAnimationStep = -2;

  data.font = TTF_OpenFont(FONT_PATH, FONT_SIZE);

  data.music = NULL;
  data.music = Mix_LoadMUS(SOUND_BACKGROUND_MUSIC);
  if(data.music) {
    Mix_VolumeMusic(MIX_MAX_VOLUME);
    Mix_FadeInMusicPos(data.music, -1, 2000, 3.0);
  }

  return data;
}

SDL_Rect getRobotSprite(s_robot* robot) {
  SDL_Rect robotSprite;
  robotSprite.w = 32;
  robotSprite.h = 32;
  robotSprite.y = 32*robot->direction;

  if(!robot->hasExtinguisher)
    robotSprite.x = 0;
  else if(robot->status != STATUS_EXTINGUISH_FIRE)
    robotSprite.x = 32;
  else
    robotSprite.x = 64;

  return robotSprite;
}

SDL_Rect getMapTileRect(char c) {
  SDL_Rect tileRect;
  tileRect.w = 32;
  tileRect.h = 32;

  switch (c) {
    case TILE_EMPTY:
      tileRect.x = 0;
      tileRect.y = 0;
      break;

    case TILE_EXTINGUISHED_FIRE:
      tileRect.x = 32;
      tileRect.y = 0;
      break;

    case TILE_FIRE_LVL1:
    case TILE_FIRE_LVL2:
    case TILE_FIRE_LVL3:
      tileRect.x = 64;
      tileRect.y = 0;
      break;

    case TILE_WALL:
      tileRect.x = 0;
      tileRect.y = 32;
      break;

    case TILE_BLACK:
      tileRect.x = 32;
      tileRect.y = 32;
      break;

    case TILE_EXTINGUISHER:
      tileRect.x = 64;
      tileRect.y = 32;
      break;

    default:
      tileRect.x=0;
      tileRect.y=0;
  }

  return tileRect;
}

SDL_Rect getRobotVisionTileRect(char c) {
  SDL_Rect tileRect;
  tileRect.w = 32;
  tileRect.h = 32;

  switch (c) {
    case TILE_EMPTY:
    case TILE_EXTINGUISHED_FIRE:
      tileRect.x = 0;
      tileRect.y = 0;
      break;

    case TILE_FIRE_LVL1:
      tileRect.x = 32;
      tileRect.y = 0;
      break;

    case TILE_FIRE_LVL2:
      tileRect.x = 64;
      tileRect.y = 0;
      break;

    case TILE_FIRE_LVL3:
      tileRect.x = 96;
      tileRect.y = 0;
      break;

    case TILE_WALL:
    case TILE_BLACK:
      tileRect.x = 96;
      tileRect.y = 32;
      break;

    case TILE_START:
      tileRect.x = 0;
      tileRect.y = 32;
      break;

    case TILE_VISITED:
    case TILE_NOFIRE:
      tileRect.x = 64;
      tileRect.y = 32;
      break;

    case TILE_EXTINGUISHER:
    case TILE_INTERESTING:
      tileRect.x = 32;
      tileRect.y = 32;
      break;

    default:
      tileRect.x=0;
      tileRect.y=0;
  }

  return tileRect;
}

SDL_Rect getFireAnimationStep(char c, s_SDLData* data) {
  SDL_Rect fireRect;
  fireRect.w = 32;
  fireRect.h = 32;
  fireRect.x = 32*fabs(data->fireAnimationStep);

  switch(c) {
    case TILE_FIRE_LVL1:
      fireRect.y = 0;
      break;

    case TILE_FIRE_LVL2:
      fireRect.y = 32;
      break;

    case TILE_FIRE_LVL3:
      fireRect.y = 64;
      break;

    default:
      fireRect.y = 0;
  }

  data->fireAnimationStep++;
  if(data->fireAnimationStep > 2)
    data->fireAnimationStep = -2;

  return fireRect;
}

void displayScreen(s_SDLData* data, s_room* room) {
  SDL_FillRect(data->window, NULL, SDL_MapRGB(data->window->format, 0, 0, 0));

  if(room->sizeX < WINDOW_X/(32*2) && room->sizeY < WINDOW_Y) {
    for(int i=0; i < room->sizeX; i++)
      for(int j=0; j < room->sizeY; j++) {
        SDL_Rect srcMap = getMapTileRect(room->nodes[j][i].symb);
        SDL_Rect srcRobotVision = getRobotVisionTileRect(room->nodes[j][i].robotVision);
        SDL_Rect destMap;
        destMap.x = i*32;
        destMap.y = j*32;
        SDL_Rect destRobotVision;
        destRobotVision.x = WINDOW_X/2 + i*32;
        destRobotVision.y = j*32;

        SDL_BlitSurface(data->mapTileset, &srcMap, data->window, &destMap);
        SDL_BlitSurface(data->robotVisionTileset, &srcRobotVision, data->window, &destRobotVision);
      }

    SDL_Rect robotSprite = getRobotSprite(&(room->robot));
    SDL_Rect destMap;
    destMap.x = room->robot.pos.x*32;
    destMap.y = room->robot.pos.y*32;
    SDL_Rect destRobotVision;
    destRobotVision.x = WINDOW_X/2 + room->robot.pos.x*32;
    destRobotVision.y = room->robot.pos.y*32;

    SDL_BlitSurface(data->robotSpritesheet, &robotSprite, data->window, &destMap);
    SDL_BlitSurface(data->robotSpritesheet, &robotSprite, data->window, &destRobotVision);

  } else {
    for(int i=-14; i<=11; i++)
      for(int j=-20; j<=20; j++) {
        if(11.5*32 + j*32 > WINDOW_X/2)
          break;

        SDL_Rect srcMap;
        SDL_Rect destMap;
        destMap.x = 11.5*32+j*32;
        destMap.y = 13.5*32+i*32;
        if(room->robot.pos.x + j < 0 || room->robot.pos.y + i < 0 || room->robot.pos.x + j >= room->sizeX || room->robot.pos.y + i >= room->sizeY)
          srcMap = getMapTileRect(TILE_BLACK);
        else
          srcMap = getMapTileRect(room->nodes[room->robot.pos.y + i][room->robot.pos.x + j].symb);

        if(38.5*32 + j*32 < WINDOW_X/2)
          continue;

        SDL_Rect srcRobotVision;
        SDL_Rect destRobotVision;
        destRobotVision.x = 38.5*32+j*32;
        destRobotVision.y = 13.5*32+i*32;
        if(room->robot.pos.x + j < 0 || room->robot.pos.y + i < 0 || room->robot.pos.x + j >= room->sizeX || room->robot.pos.y + i >= room->sizeY)
          srcRobotVision = getRobotVisionTileRect(TILE_BLACK);
        else
          srcRobotVision = getRobotVisionTileRect(room->nodes[room->robot.pos.y + i][room->robot.pos.x + j].robotVision);

        SDL_BlitSurface(data->mapTileset, &srcMap, data->window, &destMap);
        SDL_BlitSurface(data->robotVisionTileset, &srcRobotVision, data->window, &destRobotVision);

        if(room->robot.pos.x + j >= 0 && room->robot.pos.y + i >= 0 && room->robot.pos.x + j < room->sizeX && room->robot.pos.y + i < room->sizeY)
          if(room->nodes[room->robot.pos.y + i][room->robot.pos.x + j].symb == TILE_FIRE_LVL1
            || room->nodes[room->robot.pos.y + i][room->robot.pos.x + j].symb == TILE_FIRE_LVL2
            || room->nodes[room->robot.pos.y + i][room->robot.pos.x + j].symb == TILE_FIRE_LVL3) {

              SDL_Rect srcFire = getFireAnimationStep(room->nodes[room->robot.pos.y + i][room->robot.pos.x + j].symb, data);
              SDL_BlitSurface(data->fireAnimation, &srcFire, data->window, &destMap);
            }
      }

    SDL_Rect robotSprite = getRobotSprite(&(room->robot));
    SDL_Rect destMap;
    destMap.x = 11.5*32;
    destMap.y = 13.5*32;
    SDL_Rect destRobotVision;
    destRobotVision.x = 38.5*32;
    destRobotVision.y = 13.5*32;

    SDL_BlitSurface(data->robotSpritesheet, &robotSprite, data->window, &destMap);
    SDL_BlitSurface(data->robotSpritesheet, &robotSprite, data->window, &destRobotVision);
  }

  displayBanner(data, &(room->robot));
  SDL_Flip(data->window);
}

void displayBanner(s_SDLData* data, s_robot* robot) {
  SDL_Rect verticalBar = {WINDOW_X/2-16, 0, 32, 900};
  SDL_FillRect(data->window, &verticalBar, SDL_MapRGB(data->window->format, 100, 100, 100));

  SDL_Rect banner;
  banner.x=0;
  banner.y=818;
  banner.w=1600;
  banner.h=84;
  SDL_FillRect(data->window, &banner, SDL_MapRGB(data->window->format, 200, 200, 200));

  SDL_Surface* text;
  text = TTF_RenderText_Blended(data->font, "Life", (SDL_Color) {0,0,0});
  SDL_Rect dest = {(WINDOW_X - text->w - 300)/2, 850};
  SDL_BlitSurface(text, NULL, data->window, &dest);
  SDL_FreeSurface(text);

  SDL_Rect rectPos;
  rectPos.x = (WINDOW_X + text->w - 300)/2 + 20;
  rectPos.y = 835;
  rectPos.w = 300;
  rectPos.h = 50;
  SDL_FillRect(data->window, &rectPos, SDL_MapRGB(data->window->format, 0, 0, 0));
  rectPos.x += 5;
  rectPos.y += 5;
  rectPos.w = 29*robot->healthPoints;
  rectPos.h -= 10;
  SDL_FillRect(data->window, &rectPos, SDL_MapRGB(data->window->format, 255, 0, 0));

  char str[128];
  snprintf(str, 128, "Coordinates : %3d;%3d", robot->pos.x, robot->pos.y);
  text = TTF_RenderText_Blended(data->font, str, (SDL_Color) {0,0,0});
  dest = (SDL_Rect) {50, 850};
  SDL_BlitSurface(text, NULL, data->window, &dest);
  SDL_FreeSurface(text);

  snprintf(str, 128, "Moves : %3d", robot->moves);
  text = TTF_RenderText_Blended(data->font, str, (SDL_Color) {0,0,0});
  dest = (SDL_Rect) {WINDOW_X - 50 - text->w, 850};
  SDL_BlitSurface(text, NULL, data->window, &dest);

  strncpy(str, "Plans", 128);
  text = TTF_RenderText_Blended(data->font, str, (SDL_Color) {100,100,100});
  dest = (SDL_Rect) {WINDOW_X/4 - text->w/2, 50};
  SDL_BlitSurface(text, NULL, data->window, &dest);

  strncpy(str, "Robot Vision", 128);
  text = TTF_RenderText_Blended(data->font, str, (SDL_Color) {100,100,100});
  dest = (SDL_Rect) {3*WINDOW_X/4 - text->w/2, 50};
  SDL_BlitSurface(text, NULL, data->window, &dest);

  SDL_FreeSurface(text);
}

int getEvents(SDL_Event* event) {
  switch (event->type) {
    case SDL_QUIT:
      return 1;
      break;

    default:
      return 0;
  }

  return 0;
}

void freeSDL(s_SDLData* data) {
  SDL_FreeSurface(data->window);
  SDL_FreeSurface(data->mapTileset);
  SDL_FreeSurface(data->robotVisionTileset);
  SDL_FreeSurface(data->robotSpritesheet);
  SDL_FreeSurface(data->fireAnimation);

  TTF_CloseFont(data->font);
  Mix_FreeMusic(data->music);

  Mix_CloseAudio();
  TTF_Quit();
  SDL_Quit();
}
