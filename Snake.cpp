// Snake.cpp
#include "Snake.h"

CSnake::CSnake(int x, int y, int texture, CTile * tiles[])
{
    mTiles = tiles;
    mX = x;
    mY = y;
    mT = texture;
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

    mTiles[mT]->Draw(mX * 16, mY * 16);
}

void CSnake::Erase()
{
    mTiles[6]->Draw(mX * 16, mY * 16);
}

void CSnake::MoveX(int d)
{

    if (mNext != 0)
    {
        mNext->Erase();
        mNext->SetPos(mX, mY);
    }
    mX += d;
    mT = d == 1 ? 0 : 1;
    mTiles[mT]->Draw(mX * 16, mY * 16);
}

void CSnake::MoveY(int d)
{
    if (mNext != 0)
    {
        mNext->Erase();
        mNext->SetPos(mX, mY);
    }
    mY += d;
    mT = d == 1 ? 3 : 2;
    mTiles[mT]->Draw(mX * 16, mY * 16);
}

void CSnake::SetPos(int x, int y)
{
    if (mNext != 0)
    {
        mNext->Erase();
        mNext->SetPos(mX, mY);
    }

    mX = x;
    mY = y;
    mTiles[mT]->Draw(mX * 16, mY * 16);
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
        mNext = new CSnake(mX, mY, 4, mTiles);
}

void CSnake::Grow(int x, int y)
{
    if (mNext != 0)
        mNext->Grow(x, y);
    else
        mNext = new CSnake(x, y, 4, mTiles);
}