// Snake.cpp

#include "Snake.h"

CSnake::CSnake(int x, int y, CSnake * prev, SDL_Surface * surface, SDL_Surface * textures[])
{
    mGridX = x;
    mGridY = y;
    mT = 0;
    mSurface = surface;
    mTextures = textures;

    mRect.x = 0;
    mRect.y = 0;
    mRect.w = mTextures[0]->w;
    mRect.h = mTextures[0]->h;

	mPrev = prev;
    mNext = 0;
}

CSnake::~CSnake(void)
{
    if (mNext != 0)
        delete mNext;
}

//void CSnake::Draw(int x, int y)
//{
//
//    if (mNext != 0)
//        mNext->Draw(mX, mY);
//
//    mX = x;
//    mY = y;
//    Draw();
//}

void CSnake::Draw()
{
	if (mNext != 0)
        mNext->Draw();

	if (mNext == 0) // this is the tail, so determine texture based on previous segment
		if (mPrev->mGridY == mGridY) { // horizontal
            if (mPrev->mGridX > mGridX)
                mT = 10;
            else
                mT = 12;
        } else { // vertical
            if (mPrev->mGridY > mGridY)
                mT = 13;
            else
                mT = 11;
		}
    else {
        if (mPrev == 0) // this is the head, so determine texture based on next segment
            if (mNext->mGridY == mGridY) // horizontal
                if (mNext->mGridX > mGridX)
                    mT = 2;
                else
                    mT = 0;
            else { // vertical
                if (mNext->mGridY > mGridY)
                    mT = 3;
                else
                    mT = 1;
            }
    else
        {
            if (mPrev->mGridY == mGridY && mNext->mGridY == mGridY)
                mT = 4; // horizontal
            else if (mPrev->mGridX == mGridX && mNext->mGridX == mGridX)
                    mT = 5; // vertical
            else if ((mPrev->mGridX > mGridX && mNext->mGridY < mGridY) || (mNext->mGridX > mGridX && mPrev->mGridY < mGridY))
				mT = 6; // lower left corner
            else if ((mPrev->mGridX > mGridX && mNext->mGridY > mGridY) || (mNext->mGridX > mGridX && mPrev->mGridY > mGridY))
                mT = 7; // upper left corner
            else if ((mPrev->mGridX < mGridX && mNext->mGridY < mGridY) || (mNext->mGridX < mGridX && mPrev->mGridY < mGridY))
				mT = 8; // lower right corner
            else if ((mPrev->mGridX < mGridX && mNext->mGridY > mGridY) || (mNext->mGridX < mGridX && mPrev->mGridY > mGridY))
				mT = 9; // upper right corner
        }
    }

    mRect.x = mGridX * mRect.w;
    mRect.y = mGridY * mRect.h;
	Erase();
    SDL_BlitSurface(mTextures[mT], NULL, mSurface, &mRect);
}

void CSnake::MoveX(int d)
{
	if (mNext != 0)
		mNext->SetPos(mGridX, mGridY);
	else
		Erase();
	mGridX += d;
	Draw();
}

void CSnake::MoveY(int d)
{
	if (mNext != 0)
		mNext->SetPos(mGridX, mGridY);
	else
		Erase();
	mGridY += d;
	Draw();
}

void CSnake::SetPos(int x, int y)
{
    if (mNext == 0)
        Erase();
    else
        mNext->SetPos(mGridX, mGridY);

    mGridX = x;
    mGridY = y;
    Draw();
}

void CSnake::GetPos(int &x, int &y)
{
    x = mGridX;
    y = mGridY;
}

void CSnake::Grow()
{
    if (mNext != 0)
        mNext->Grow();
    else
        mNext = new CSnake(mGridX, mGridY, this, mSurface, mTextures);
}

void CSnake::Grow(int x, int y)
{
    if (mNext != 0)
        mNext->Grow(x, y);
    else
        mNext = new CSnake(mGridX + x, mGridY + y, this, mSurface, mTextures);
}

void CSnake::Erase()
{
    mRect.x = mGridX * mRect.w;
    mRect.y = mGridY * mRect.h;
    SDL_FillSurfaceRect(mSurface, &mRect, 0xff000000);
}

bool CSnake::CheckCollision(int x, int y)
{
    if (mGridX == x && mGridY == y)
        return true;
    if (mNext == 0)
        return false;
    return mNext->CheckCollision(x, y);
}