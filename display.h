#ifndef DISPLAY_H
#define DISPLAY_H

#include "includes.h"
#include "room.h"

#define WINDOW_X 1600
#define WINDOW_Y 900
#define MAX_PATH 128
#define GRAPHICS_ICON "./data/graphics/icon.png"
#define GRAPHICS_MAP_TILESET "./data/graphics/map_tileset.png"
#define GRAPHICS_ROBOT_VISION_TILESET "./data/graphics/robotVision_tileset.png"
#define GRAPHICS_ROBOT_SPRITESHEET "./data/graphics/robot_spritesheet.png"
#define GRAPHICS_FIRE_ANIMATION "./data/graphics/fire_animation.png"
#define FONT_SIZE 32
#define FONT_PATH "./data/calibri.ttf"
#define SOUND_BACKGROUND_MUSIC "./data/sounds/background_theme.mp3"

typedef struct s_SDLData {
    SDL_Surface* window;
    SDL_Surface* icon;
    SDL_Surface* mapTileset;
    SDL_Surface* robotVisionTileset;
    SDL_Surface* robotSpritesheet;
    SDL_Surface* fireAnimation;

    TTF_Font* font;
    Mix_Music* music;
    int fireAnimationStep;
} s_SDLData;

s_SDLData initSDL();
SDL_Rect getRobotSprite(s_robot* robot);
SDL_Rect getMapTileRect(char c);
SDL_Rect getRobotVisionTileRect(char c);
SDL_Rect getFireAnimationStep(char c, s_SDLData* data);
void displayScreen(s_SDLData* data, s_room* room);
void displayBanner(s_SDLData* data, s_robot* robot);
int getEvents(SDL_Event* event);
void freeSDL(s_SDLData* data);

#endif
