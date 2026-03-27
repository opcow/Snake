#pragma once

#include <SDL3/SDL.h>
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

    const bool *keystate = SDL_GetKeyboardState(NULL);
    if (keystate[SDL_SCANCODE_Q])
        return QUIT;

    // check to see if a key has been released since it was last read as pressed.
    // the point is to keep track of the last key pressed. that key
    // rule should make for less rage inducement from pressing one key before
    // releasing another when tring to make a quick turn

    if (keystate[SDL_SCANCODE_LEFT] || keystate[SDL_SCANCODE_A])
    {
        if((keyMap & LKEY) == 0)
        {
            direction = LEFT;
            keyMap |= LKEY;
        }
     }
    else if((keyMap & LKEY) != 0)
            keyMap = 0;

    if (keystate[SDL_SCANCODE_RIGHT] || keystate[SDL_SCANCODE_D])
    {
        if((keyMap & RKEY) == 0)
        {
            direction = RIGHT;
            keyMap |= RKEY;
        }
    }
    else if((keyMap & RKEY) != 0)
            keyMap = 0;

    if (keystate[SDL_SCANCODE_UP] || keystate[SDL_SCANCODE_W])
    {
        if((keyMap & UKEY) == 0)
        {
            direction = UP;
            keyMap |= UKEY;
        }
    }
    else if((keyMap & UKEY) != 0)
            keyMap = 0;

    if (keystate[SDL_SCANCODE_DOWN] || keystate[SDL_SCANCODE_S])
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
    case SDL_EVENT_QUIT:
        return true;
        break;
    }
     return false;
}