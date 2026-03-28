// main.cpp
//
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <Windows.h>
#include <cstdio>
#include <time.h>

#include "Tile.h"
#include "Snake.h"
#include "SnakeMap.h"
#include "input.h"

const int scrWidth = 1024, scrHeight = 768;
const int mapSurfaceW = 768, mapSurfaceH = 768;
const int infoSurfaceW = 256, infoSurfaceH = 768;

//int tileSz = 16;
int tileW, tileH;
int mapWidth = 0, mapHeight = 0;

#define SB_LEFTZS

#ifdef SB_LEFTZS
SDL_Rect mapRect = { 0, 0, 768, 768 };
#else
SDL_Rect mapRect = { 256, 0, 768, 768 };
#endif
SDL_Rect infoRect = { 768, 0, 256, 768 };
SDL_Rect titleRect = { 768, 0, 0, 0 };
SDL_Rect digitRect = { 0, 0, 0, 0 };

// SDL3: window + renderer own the display; surfaces are composed CPU-side
// and uploaded to a streaming texture each frame.
SDL_Window   * window = NULL;
SDL_Renderer * renderer = NULL;
SDL_Texture  * screen_texture = NULL;
SDL_Surface  * screen = NULL;
SDL_Surface  * map_surface = NULL;
SDL_Surface  * map_overlay = NULL;
SDL_Surface  * info_surface = NULL;
SDL_Surface  * title_surface = NULL;
SDL_Surface  * message = NULL;

int * foo = new int;
enum {
    background,
    wall,
    food,
    types_end
} ;

SDL_Surface * map_tex[types_end];
SDL_Surface * snake_tex[32];
SDL_Surface * digit_tex[10];

CSnakeMap * theMap;

char filename[_MAX_PATH];

int score;

void cleanup();

static void error(const char * msg)
{
    // MessageBox is Unicode (MessageBoxW) in this project; convert narrow string.
    int len = MultiByteToWideChar(CP_UTF8, 0, msg, -1, nullptr, 0);
    wchar_t * wmsg = new wchar_t[len];
    MultiByteToWideChar(CP_UTF8, 0, msg, -1, wmsg, len);
    MessageBoxW(0, wmsg, L"Error", 0);
    delete[] wmsg;
    cleanup();
    SDL_Quit();
    exit(-1);
}

bool load_image(const char* fn, SDL_Surface** s)
{
    *s = SDL_LoadBMP(fn);
    return (*s != NULL);
}

bool load_image_p(const char* fn, SDL_Surface** s)
{
    *s = SDL_LoadPNG(fn);
    return (*s != NULL);
}


void init()
{
    for (int i = 0; i < types_end; i++)
        map_tex[i] = 0;

    for (int i = 0; i < 6; i++)
        snake_tex[i] = 0;

    SDL_Init( SDL_INIT_VIDEO );
    window   = SDL_CreateWindow("Snek", scrWidth, scrHeight, 0);
    renderer = SDL_CreateRenderer(window, NULL);
    screen_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                       SDL_TEXTUREACCESS_STREAMING,
                                       scrWidth, scrHeight);
    screen = SDL_CreateSurface(scrWidth, scrHeight, SDL_PIXELFORMAT_ARGB8888);

    if (0 == (map_surface = SDL_CreateSurface(mapSurfaceW, mapSurfaceH,
        SDL_PIXELFORMAT_ARGB8888)))
        error("Couldn't create map surface.");
    SDL_FillSurfaceRect(map_surface, NULL, 0xff000000);

    if (0 == (info_surface = SDL_CreateSurface(infoSurfaceW, infoSurfaceH,
        SDL_PIXELFORMAT_ARGB8888)))
        error("Couldn't create info surface.");
    SDL_FillSurfaceRect(info_surface, NULL, 0xff000000);

    if (!load_image("media/images/title.bmp", &title_surface))
        error("Error loading images.");
    titleRect.w = title_surface->w;
    titleRect.h = title_surface->h;
    SDL_BlitSurface(title_surface, NULL, info_surface, 0);
    if(!load_image("media/images/go.bmp", &message))
        error("Error loading images.");
}

void init_items()
{
    Uint32 colors[types_end] = { 0xffff0000, 0xff8bcffa, 0xffa54607 };
    Uint32 * pixels;

    if (!load_image("media/images/034.bmp", &map_tex[food]))
        error("Error loading images.");
    if(!load_image("media/images/033.bmp", &map_tex[wall]))
        error("Error loading images.");

    tileW = map_tex[wall]->w;
    tileH = map_tex[wall]->h;
    mapWidth = mapSurfaceW / tileW;
    mapHeight = mapSurfaceH / tileH;

    map_tex[background] = SDL_CreateSurface(tileW, tileH, SDL_PIXELFORMAT_ARGB8888);

    pixels = (Uint32 *) map_tex[background]->pixels;
    for (int i = 0; i < tileW * tileH; i++)
        pixels[i] = 0xff000000;
}

void init_snake()
{
    for (int i = 0; i < 14; i++)
    {
        sprintf_s(filename, "media/images/%03d.png", i + 10);
        if (!load_image_p(filename, &snake_tex[i]))
            error("Error loading images.");
    }
}

void init_score()
{
    for (int i = 0; i < 10; i++)
    {
        sprintf_s(filename, "media/images/%03d.bmp", i);
        if(!load_image(filename, &digit_tex[i]))
            error("Error loading images.");
    }
    digitRect.y = 80;
    digitRect.w = digit_tex[0]->w;
    digitRect.h = digit_tex[0]->h;
}

void cleanup()
{
    for (int i = 0; i < types_end; i++)
        if (map_tex[i] != 0)
            SDL_DestroySurface(map_tex[i]);

    for (int i = 0; i < 6; i++)
        if (snake_tex[i] != 0)
            SDL_DestroySurface(snake_tex[i]);

    if (map_overlay != 0)
        SDL_DestroySurface(map_overlay);
    if (map_surface != 0)
        SDL_DestroySurface(map_surface);
    if (screen != 0)
        SDL_DestroySurface(screen);

    if (screen_texture != 0)
        SDL_DestroyTexture(screen_texture);
    if (renderer != 0)
        SDL_DestroyRenderer(renderer);
    if (window != 0)
        SDL_DestroyWindow(window);

    SDL_Quit();
}

void init_map(CSnakeMap * m)
{
    unsigned char * buffer = m->GetBuffer();

    memset(buffer, 0, mapWidth * mapHeight);
    memset(buffer, wall, mapWidth);
    memset(buffer + mapWidth * (mapHeight - 1), wall, mapWidth);
    for (int i = 1; i < mapHeight - 1; i++)
    {
        buffer[i * mapWidth] = wall;
        buffer[i * mapWidth + mapWidth - 1] = wall;
    }
}

void place_food(CSnakeMap * m, int count)
{
    int x, y;
    for (int i = 0; i < count; i++)
    {
        x = 1 + (rand() % (mapWidth - 2));
        y = 1 + (rand() % (mapHeight - 2));
        m->AddItem(x, y, food);
    }
}

bool collision(CSnake * s)
{
    int x1, y1, x2, y2;

    s->GetPos(x1, y1);

    while (s = s->GetNext())
    {
        s->GetPos(x2, y2);
        if (x1 == x2 && y1 == y2)
            return true;
    }
    return false;
}

void draw_screen()
{
    SDL_BlitSurface(map_surface, NULL, screen, &mapRect);
    SDL_BlitSurface(info_surface, NULL, screen, &infoRect);

    SDL_UpdateTexture(screen_texture, NULL, screen->pixels, screen->pitch);
    SDL_RenderTexture(renderer, screen_texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void print_score()
{
    int d, i;
    int num = score;

    digitRect.x = 252;
    i = 5;
    do
    {
        d = num % 10;
        digitRect.x -= digitRect.w;
        SDL_BlitSurface( digit_tex[d], NULL, info_surface, &digitRect );
        num /= 10;
        i--;
    } while (num > 0);

    while (i >= 0)
    {
        digitRect.x -= digitRect.w;
        SDL_BlitSurface( digit_tex[0], NULL, info_surface, &digitRect );
        i--;
    }
}

void spawn_random(CSnakeMap * m, CSnake * s, unsigned char i)
{
    int x, y;

    do {
        x = 1 + (rand() % (mapWidth - 2));
        y = 1 + (rand() % (mapHeight - 2));
    } while (s->CheckCollision(x, y));

    m->AddItem(x, y, i);
    m->Draw(x, y);
}

//void xdo_move(CSnake * s, int dir)
//{
//    static Uint64 old_ticks = 0;
//    Uint64 new_ticks;
//    Uint64 delta = 100;
//    static Uint64 delay = 600;
//    static int oldDir = NODIR;
//
//    new_ticks = SDL_GetTicks();
//    delta = new_ticks - old_ticks;
//
//    if (delta >= delay)
//    {
//        if (delay > 150)
//            delay -= 25;
//        old_ticks = new_ticks;
//        switch (dir)
//        {
//        case LEFT:
//            if (oldDir != RIGHT)
//            {
//                s->MoveX(-1);
//                oldDir = dir;
//            }
//            else
//                s->MoveX(1);
//            break;
//        case RIGHT:
//            if (oldDir != LEFT)
//            {
//                s->MoveX(1);
//                oldDir = dir;
//            }
//            else
//                s->MoveX(-1);
//            break;
//        case UP:
//            if (oldDir != DOWN)
//            {
//                s->MoveY(-1);
//                oldDir = dir;
//            }
//            else
//                s->MoveY(1);
//            break;
//        case DOWN:
//            if (oldDir != UP)
//            {
//                s->MoveY(1);
//                oldDir = dir;
//            }
//            else
//                s->MoveY(-1);
//            break;
//        }
//    }
//}

void do_move(CSnake * s, int dir)
{
    static Uint64 old_ticks = 0;
    Uint64 new_ticks;
    Uint64 delta = 100;
    static Uint64 delay = 320;
    static int oldDir = NODIR;
    int directions[] = { 0, -1, 1, -1, 1 };

    void (CSnake::*mx)(int) = &CSnake::MoveX;
    void (CSnake::*my)(int) = &CSnake::MoveY;
    void (CSnake::*md)(int);
    
    new_ticks = SDL_GetTicks();
    delta = new_ticks - old_ticks;


    if (dir == LEFT || dir == RIGHT)
        md = mx;
    else
        md = my;

    if (delta < delay)
        return;

    old_ticks = new_ticks;
    //if (delay > 150)
    //    delay -= 25;


    switch (dir)
    {
    case LEFT:
        if (oldDir != RIGHT)
            oldDir = LEFT;
        break;
    case RIGHT:
        if (oldDir != LEFT)
            oldDir = RIGHT;
        break;
    case UP:
        if (oldDir != DOWN)
            oldDir = UP;
        break;
    case DOWN:
        if (oldDir != UP)
            oldDir = DOWN;
        break;
    }
    (s->*md)(directions[oldDir]);
}

bool checkMap(CSnake * s)
{
    int x, y;
    unsigned char item;

    s->GetPos(x, y);
    item = theMap->GetItem(x, y);

    switch (item)
    {
    case wall:
        return false;
        break;
    case food:
        theMap->AddItem(x, y, 0);
        s->Grow();
        spawn_random(theMap, s, food);
        score += 10;
        print_score();
        return true;
        break;
    default:
        return true;
        break;
    }
}

int main( int argc, char* args[] )
{
    CSnake * snake;
    int snake_x, snake_y;
    unsigned char snake_hit;
    int k = 1, sb_toggle = 0;
    Uint32 delay = 10;
    bool bQuit = false;

    score = 0;

    srand((unsigned)time( NULL ));
    init();
    init_items();
    init_snake();
    init_score();

    theMap = new CSnakeMap(mapWidth, mapHeight, map_surface);
    theMap->AddTiles(map_tex, types_end);

    init_map(theMap);

    snake = new CSnake(mapWidth / 2, mapHeight / 2, nullptr, map_surface, snake_tex);
    snake->Grow(-1, 0);
    snake->Grow(-1, 0);
    snake->Grow(-1, 0);
    snake->Grow(-1, 0);
    snake->Grow(-1, 0);
    place_food(theMap, 1);
    theMap->Draw();
    print_score();
    snake->Draw();
    draw_screen();
    SDL_Delay(2000);

    do_move(snake, RIGHT);

    while (!quitcheck())
    {
        if (bQuit)
            break;
        k = getInput();
        if (k == QUIT)
            break;
        if (k >= LEFT && k <= DOWN)
            do_move(snake, k);
        if (collision(snake))
            break;
        if (!checkMap(snake))
            break;
        draw_screen();
    }
    SDL_Rect r; r.x = 128; r.y = 256, r.w = 512, r.h = 256;
    SDL_BlitSurface( message, NULL, map_surface, &r );
    draw_screen();
    SDL_Delay(2000);
    SDL_DestroySurface(message);

    delete snake;
    cleanup();
 
    return 0;
}
