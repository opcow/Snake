#pragma once

#include <SDL.h>
#include "input.h"

#define LKEY 0X00000002
#define RKEY 0X00000004
#define UKEY 0X00000008
#define DKEY 0X00000010

int getInput()
{
    static int direction = RIGHT;
    static bool leftPressed = false, rightPressed = false,
                upPressed = false, downPressed = false;

    static Uint32 keyMap = 0;

    Uint8 *keystate = SDL_GetKeyState(NULL);
    if (keystate[SDLK_q])
        return QUIT;

    // check to see if a key has been released since it was last read as
    // pressed the point is to keep track of the last key pressed. that key
    // rules should make for less rage inducement from pressing on key before
    // releasing another when tring to make a quick turn

    if (keystate[SDLK_LEFT] || keystate[SDLK_a])
    {
        if((keyMap & LKEY) == 0)
        {
            direction = LEFT;
            keyMap |= LKEY;
        }
     }
    else if((keyMap & LKEY) != 0)
            keyMap = 0;

    if (keystate[SDLK_RIGHT] || keystate[SDLK_d])
    {
        if((keyMap & RKEY) == 0)
        {
            direction = RIGHT;
            keyMap |= RKEY;
        }
    }
    else if((keyMap & RKEY) != 0)
            keyMap = 0;

    if (keystate[SDLK_UP] || keystate[SDLK_w])
    {
        if((keyMap & UKEY) == 0)
        {
            direction = UP;
            keyMap |= UKEY;
        }
    }
    else if((keyMap & UKEY) != 0)
            keyMap = 0;

    if (keystate[SDLK_DOWN] || keystate[SDLK_s])
    {
        if((keyMap & DKEY) == 0)
        {
            direction = DOWN;
            keyMap |= DKEY;
        }
    }
    else if((keyMap & DKEY) != 0)
            keyMap = 0;

    return direction;
}

bool quitcheck()
{
    SDL_Event event;
    SDL_PollEvent( &event );
    static int dirx = 0, diry = 0;

    switch( event.type ){
         /* SDL_QUIT event (window close) */
    case SDL_QUIT:
        return true;
        break;
    }
     return false;
}