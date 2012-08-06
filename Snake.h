// Snake.h
#pragma once

#include "Tile.h"

class CSnake
{
public:
    CSnake(int x, int y, int texture, CTile * tiles[]);
    ~CSnake(void);

    void Draw();
    void Draw(int x, int y);
    void MoveX(int d);
    void MoveY(int d);
    void Erase();
    void SetPos(int x, int y);
    void GetPos(int &x, int &y);
    void Grow();
    void Grow(int x, int y);
    CSnake * GetNext() { return mNext; };

private:
    int mX;
    int mY;
    int mT;
    CTile ** mTiles;
    CSnake * mNext;
};

