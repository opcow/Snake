#include "SnakeMap.h"


CSnakeMap::CSnakeMap(int width, int height, SDL_Surface * surface)
{
    mWidth = width;
    mHeight = height;
    mMap = new unsigned char[width * height];
    mSurface = surface;
    mTextures = 0;
}

CSnakeMap::~CSnakeMap(void)
{
    if (mMap != 0)
        delete mMap;
}

void CSnakeMap::AddTiles(SDL_Surface * textures[], int count)
{
    mRect.h = textures[0]->h;
    mRect.w = textures[0]->w;
    mTextures = textures;
    mItemCount = count;
}

void CSnakeMap::Draw()
{
    for (int i = 0; i < mHeight; i++)
        for (int j = 0; j < mWidth; j++)
            Draw(j, i);
}

void CSnakeMap::Draw(int x, int y)
{
    mRect.x = x * mRect.w;
    mRect.y = y * mRect.h;

    SDL_BlitSurface(mTextures[mMap[x + y * mWidth]], NULL, mSurface, &mRect);
}

unsigned char * CSnakeMap::GetBuffer()
{
    return mMap;
}

unsigned char CSnakeMap::GetItem(int x, int y)
{
    return mMap[x + y * mWidth];
}

void CSnakeMap::AddItem(int x, int y, unsigned char item)
{
    mMap[x + y * mWidth] = item;
}