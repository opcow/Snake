// Snake.cpp
//
#include "Snake.h"

CSnake::CSnake(int x, int y, int texture, SDL_Surface * surface, SDL_Surface * textures[])
{
    mX = x;
    mY = y;
    mT = texture;
    mSurface = surface;
    mTextures = textures;

    mRect.x = 0;
    mRect.y = 0;
    mRect.w = mTextures[0]->w;
    mRect.h = mTextures[0]->h;

    mNext = 0;
}

CSnake::~CSnake(void)
{
    if (mNext != 0)
        delete mNext;
}

void CSnake::Draw(int x, int y)
{

    if (mNext != 0)
        mNext->Draw(mX, mY);

    mX = x;
    mY = y;
    Draw();
}

void CSnake::Draw()
{
    if (mNext != 0)
        mNext->Draw();

    mRect.x = mX * mRect.w;
    mRect.y = mY * mRect.h;
    SDL_BlitSurface( mTextures[mT], NULL, mSurface, &mRect );
}

void CSnake::MoveX(int d)
{
    if (mNext != 0)
        mNext->SetPos(mX, mY);
    else
        Erase();
    mX += d;
    mT = d == 1 ? 0 : 1;
    Draw();
}

void CSnake::MoveY(int d)
{
    if (mNext != 0)
        mNext->SetPos(mX, mY);
    else
        Erase();
    mY += d;
    mT = d == 1 ? 3 : 2;
   Draw();
}

void CSnake::SetPos(int x, int y)
{
    if (mNext == 0)
        Erase();
    else
        mNext->SetPos(mX, mY);

    mX = x;
    mY = y;
    Draw();
}

void CSnake::GetPos(int &x, int &y)
{
    x = mX;
    y = mY;
}

void CSnake::Grow()
{
    if (mNext != 0)
        mNext->Grow();
    else
        mNext = new CSnake(mX, mY, 4, mSurface, mTextures);
}

void CSnake::Grow(int x, int y)
{
    if (mNext != 0)
        mNext->Grow(x, y);
    else
        mNext = new CSnake(mX + x, mY + y, 4, mSurface, mTextures);
}

void CSnake::Erase()
{
    mRect.x = mX * mRect.w;
    mRect.y = mY * mRect.h;
    SDL_FillRect(mSurface, &mRect, 0xff000000);
}

bool CSnake::CheckCollision(int x, int y)
{
    if (mX == x && mY == y)
        return true;
    if (mNext == 0)
        return false;
    return mNext->CheckCollision(x, y);
}