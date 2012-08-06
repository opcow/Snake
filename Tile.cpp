// Tile.cpp
#include "Tile.h"

CTile::CTile(SDL_Surface * texture, SDL_Surface * screen)
{
    mRect.x = 0;
    mRect.y = 0;
    mRect.w = 16;
    mRect.h = 16;
    mTexture = texture;
    mScreen = screen;
};

CTile::~CTile(void)
{
}

void CTile::Draw(int x, int y)
{
    mRect.x = x;
    mRect.y = y;
    SDL_BlitSurface( mTexture, NULL, mScreen, &mRect );
}

void CTile::Draw()
{
    SDL_BlitSurface( mTexture, NULL, mScreen, &mRect );
}
