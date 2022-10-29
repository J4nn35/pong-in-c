#include "game.h"
#include "ball.h"
#include "paddle.h"
#include "defines.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *p1scoreText = NULL, *p2scoreText = NULL;
static TTF_Font *smallFont = NULL, *bigFont = NULL;
static Ball ball;
static Paddle p2, p1;

// loadTexture is a helper function
SDL_Texture *loadTexture(const char *path) {

    SDL_Surface *surface = IMG_Load(path);
    if(!surface) {
        fprintf(stderr, "SDL_image can't load %s\nSDL_image error: %s\n", path, IMG_GetError());
    }

    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0xaa, 0xbb, 0xcc));

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if(!texture) {
        fprintf(stderr, "SDL can't convert surface to texture\nSDL error: %s\n", SDL_GetError());
    }

    SDL_FreeSurface(surface);

    return texture;

}

SDL_Texture *renderText(TTF_Font *font, const char *text, SDL_Color color) {

    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FreeSurface(surface);

    return texture;
    
}

int init() {

    int success = 1;

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        fprintf(stderr, "SDL can't initialize\nSDL error: %s\n", SDL_GetError());
        success = 0;
    }

    int IMG_flag = IMG_INIT_PNG;

    if(!(IMG_Init(IMG_flag) & IMG_flag)) {
        fprintf(stderr, "SDL_image can't initialize\nSDL_image error: %s\n", IMG_GetError());
        success = 0;
    }

    if(TTF_Init()) {
        fprintf(stderr, "SDL_ttf can't inititalize\nSDL_ttf error: %s\n", TTF_GetError());
        success = 0;
    }

    window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, W, H, SDL_WINDOW_SHOWN);
    if(!window) {
        fprintf(stderr, "SDL can't create window\nSDL error: %s\n", SDL_GetError());
        success = 0;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!renderer) {
        fprintf(stderr, "SDL can't create renderer\nSDL error: %s\n", SDL_GetError());
        success = 0;
    }

    smallFont = TTF_OpenFont("res/OpenSans-Light.ttf", 50);
    if(!smallFont) {
        fprintf(stderr, "SDL_ttf can't load font\nSDL_ttf error: %s\n", TTF_GetError());
        success = 0;
    }

    SDL_Color gray = {100, 100, 100, 0xff};
    char p1score[3], p2score[3];
    itoa(p1.score, p1score, 10);
    itoa(p2.score, p2score, 10);

    p1scoreText = renderText(smallFont, p1score, gray);
    p2scoreText = renderText(smallFont, p2score, gray);

    bigFont = TTF_OpenFont("res/OpenSans-Light.ttf", 100);
    if(!bigFont) {
        fprintf(stderr, "SDL_ttf can't load font\nSDL_ttf error: %s\n", TTF_GetError());
        success = 0;
    }

    srand(time(NULL));

    ball.texture = loadTexture("res/ball.png");
    if(!ball.texture) {
        fprintf(stderr, "SDL_image can't load ball's texture\n");
        success = 0;
    }

    initBall(&ball);

    p2.x = W * 9 / 10;
    p2.y = W / 2;
    p2.h = H / 6;
    p2.w = 20;
    p2.speed = 10;
    p2.score = 0;

    p1.x = W / 10;
    p1.y = W / 2;
    p1.h = H / 6;
    p1.w = 20;
    p1.speed = 10;
    p1.score = 0;

    return success;

}

int update() {

    int on = 1;

    SDL_Color white = {0xff, 0xff, 0xff, 0xff};
    SDL_Texture *text = NULL;

    if(p1.score > 10) {
        on = 0;
        text = renderText(bigFont, "P1 won!", white);
    }
    if(p2.score > 10) {
        on = 0;
        text = renderText(bigFont, "P2 won!", white);
    }

    if(text) {
        int w, h;
        SDL_QueryTexture(text, NULL, NULL, &w, &h);
        SDL_Rect textRenderRect = {(W - w) / 2, (H - h) / 2, w, h};
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        SDL_RenderCopy(renderer, text, NULL, &textRenderRect);
        SDL_RenderPresent(renderer);
        SDL_Delay(1000);
        SDL_DestroyTexture(text);
        text = NULL;
    }

    // get input

    SDL_Event e;
    const Uint8 *keyStates = SDL_GetKeyboardState(NULL);

    while(SDL_PollEvent(&e)) {
        switch(e.type) {
        case SDL_QUIT:
            on = 0;
            break;
        }
    }

    if(keyStates[SDL_SCANCODE_UP]) {
        p2.y -= p2.speed;
    } else if(keyStates[SDL_SCANCODE_DOWN]) {
        p2.y += p2.speed;
    }
    if(keyStates[SDL_SCANCODE_W]) {
        p1.y -= p1.speed;
    } else if(keyStates[SDL_SCANCODE_S]) {
        p1.y += p1.speed;
    }

    //update the ball

    ball.y += ball.dy;
    ball.x += ball.dx;

    if( (ball.y > p2.y && ball.y < p2.y + p2.h && ball.x + ball.w > p2.x && ball.x < p2.x + p2.w) ||
        (ball.y > p1.y && ball.y < p1.y + p1.h && ball.x > p1.x && ball.x < p1.x + p1.w) ) {
            ball.dx *= -1;
    }

    SDL_Color gray = {100, 100, 100, 0xff};

    if(ball.y < 0 || ball.y + ball.h > H) {
        ball.dy *= -1;
    }
    if(ball.x < 0) {
        p2.score++;
    }
    if(ball.x + ball.w > W) {
        p1.score++;
    }

    char p1score[3], p2score[3];
    itoa(p1.score, p1score, 10);
    itoa(p2.score, p2score, 10);

    if(ball.x < 0 || ball.x + ball.w > W) {
        p1scoreText = renderText(smallFont, p1score, gray);
        p2scoreText = renderText(smallFont, p2score, gray);
        initBall(&ball);
        render();
        SDL_Delay(300);
    }

    //update paddles
    
    if(p2.y < 0) {
        p2.y = 0;
    } else if(p2.y + p2.h > H) {
        p2.y = H - p2.h;
    }

    if(p1.y < 0) {
        p1.y = 0;
    } else if(p1.y + p1.h > H) {
        p1.y = H - p1.h;
    }

    return on;

}

void render() {

    //clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
    SDL_RenderClear(renderer);

    //draw ball
    SDL_Rect ballRenderRect = {ball.x, ball.y, ball.w , ball.h};

    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderCopy(renderer, ball.texture, NULL, &ballRenderRect);

    //draw paddles
    SDL_Rect p2Rect = {p2.x, p2.y, p2.w, p2.h};
    SDL_Rect p1Rect = {p1.x, p1.y, p1.w, p1.h};

    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderFillRect(renderer, &p2Rect);
    SDL_RenderFillRect(renderer, &p1Rect);

    // show score

    int w, h;

    SDL_QueryTexture(p1scoreText, NULL, NULL, &w, &h);
    SDL_Rect p1scoreRect = {W / 4 - w / 2, H / 6, w, h};
    SDL_RenderCopy(renderer, p1scoreText, NULL, &p1scoreRect);

    SDL_QueryTexture(p2scoreText, NULL, NULL, &w, &h);
    SDL_Rect p2scoreRect = {W*3 / 4 - w / 2, H / 6, w, h};
    SDL_RenderCopy(renderer, p2scoreText, NULL, &p2scoreRect);

    SDL_RenderPresent(renderer);

}

void quit() {

    TTF_CloseFont(smallFont);
    smallFont = NULL;

    TTF_CloseFont(bigFont);
    bigFont = NULL;

    SDL_DestroyTexture(ball.texture);
    ball.texture = NULL;

    SDL_DestroyTexture(p1scoreText);
    p1scoreText = NULL;

    SDL_DestroyTexture(p2scoreText);
    p2scoreText = NULL;

    SDL_DestroyRenderer(renderer);
    renderer = NULL;

    SDL_DestroyWindow(window);
    window = NULL;

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

}