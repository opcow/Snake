#pragma once

#include "Tile.h"

class CSnakeMap
{
public:
    CSnakeMap(int width, int height, SDL_Surface * surface);
    ~CSnakeMap(void);

    unsigned char GetItem(int x, int y);
    void AddItem(int x, int y, unsigned char item);
    void Draw();
    void Draw(int x, int y);
    void Clear();
    void AddTiles(SDL_Surface * textures[], int count);
    unsigned char * GetBuffer();

private:
    int mWidth;
    int mHeight;
    unsigned char * mMap;
    int mItemCount;
    SDL_Rect mRect;
    SDL_Surface * mSurface;
    SDL_Surface ** mTextures;
};

