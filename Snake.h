// Snake.h
//
#pragma once

#include <SDL.h>

class CSnake
{
public:
    CSnake(int x, int y, int texture, SDL_Surface * surface, SDL_Surface * textures[]);
    ~CSnake(void);

    void Draw();
    void Draw(int x, int y);
    void MoveX(int d);
    void MoveY(int d);
    void SetPos(int x, int y);
    void GetPos(int &x, int &y);
    void Grow();
    void Grow(int x, int y);
    void Erase();
    bool CheckCollision(int x, int y);
    CSnake * GetNext() { return mNext; };

private:
    int mX;
    int mY;
    int mT;
    SDL_Rect mRect;
    SDL_Surface * mSurface;
    SDL_Surface ** mTextures;
    CSnake * mNext;
};

