// Tile.h
#pragma once

#include "sdl\include\SDL.h"

class CTile
{
public:
    CTile(SDL_Surface * texture, SDL_Surface * screen);
    ~CTile(void);

    void Draw(int x, int y);
    void Draw();

    void GetPos(int &x, int &y);
    void SetPos(int x, int y);

private:
    SDL_Rect mRect;
    SDL_Surface * mScreen;
    SDL_Surface * mTexture;
};
