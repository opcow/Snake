// Tile.h
//
#pragma once

#include <SDL.h>

class CTile
{
public:
    CTile(SDL_Surface * texture, SDL_Surface * surface);
    ~CTile(void);

    void Draw(int x, int y);

private:
    SDL_Rect mRect;
    SDL_Surface * mSurface;
    SDL_Surface * mTexture;
};
