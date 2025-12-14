#pragma once
#include <SDL2/SDL.h>

struct Input {
    bool w,a,s,d,space,ctrl;
    float mouseDX, mouseDY;

    void beginFrame() {
        mouseDX = mouseDY = 0.0f;
    }

    void handleEvent(const SDL_Event& e) {
        if(e.type == SDL_MOUSEMOTION) {
            mouseDX += (float)e.motion.xrel;
            mouseDY += (float)e.motion.yrel;
        }
        if(e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
            bool down = e.type == SDL_KEYDOWN;
            switch(e.key.keysym.sym) {
                case SDLK_w: w = down; break;
                case SDLK_a: a = down; break;
                case SDLK_s: s = down; break;
                case SDLK_d: d = down; break;
                case SDLK_SPACE: space = down; break;
                case SDLK_LCTRL: ctrl = down; break;
            }
        }
    }
};
