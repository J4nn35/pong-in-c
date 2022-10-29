#ifndef __BALL_H__
#define __BALL_H__

#include <SDL2/SDL.h>
#include "defines.h"

typedef struct Ball {
    SDL_Texture *texture;
    int x, y, dx, dy, w, h;
} Ball ;

void initBall(Ball *ball) {
    SDL_QueryTexture(ball->texture, NULL, NULL, &ball->w, &ball->h);
    ball->w /= 3;
    ball->h /= 3;
    ball->x = W / 2;
    ball->y = H / 2;
    ball->dx = (rand() % 2 ? 1 : -1) * 6;
    ball->dy = (rand() % 2 ? 1 : -1) * 6;
}

#endif