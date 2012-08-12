// main.cpp
//
#include <SDL.h>
#include <SDL_ttf.h>

#include <Windows.h>
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
SDL_Rect infoRect = { 768, 0, 256, 768 };
SDL_Rect titleRect = { 768, 0, 0, 0 };
SDL_Rect digitRect = { 0, 0, 0, 0 };
#else
SDL_Rect mapRect = { 256, 0, 768, 768 };
SDL_Rect sbRect = { 0, 0, 256, 768 };
#endif

SDL_Surface * screen = NULL;
SDL_Surface * map_surface = NULL;
SDL_Surface * map_overlay = NULL;
SDL_Surface * info_surface = NULL;
SDL_Surface * title_surface = NULL;
SDL_Surface * message = NULL;

int * foo = new int;
enum {
    background,
    wall,
    food,
    types_end
} ;

SDL_Surface * map_tex[types_end];
SDL_Surface * snake_tex[6];
SDL_Surface * digit_tex[10];

CSnakeMap * theMap;

//// testing text ////
TTF_Font *font;
SDL_Surface *text_surface;
char info_text[40] = "Info";
SDL_Rect textr = { 32, 200, 256, 100 };
SDL_Color textcolor = {255,0,0};
//// testing text ////

char filename[_MAX_PATH];

int score;

void cleanup();

void error(char * msg)
{
    MessageBox(0, msg, "Error", 0);
    cleanup();
    SDL_Quit();
    exit(-1);
}

bool load_image(char * fn, SDL_Surface ** s)
{
    SDL_Surface * loadedImage;
    if ( 0 == (loadedImage = SDL_LoadBMP(fn)))
        return false;;
    *s = SDL_DisplayFormat(loadedImage);
    SDL_FreeSurface(loadedImage);
    return true;
}

void init()
{
    for (int i = 0; i < types_end; i++)
        map_tex[i] = 0;

    for (int i = 0; i < 6; i++)
        snake_tex[i] = 0;

    //Start SDL
    SDL_Init( SDL_INIT_EVERYTHING );

    int foo = TTF_Init();
    font=TTF_OpenFont("media/fonts/Vera.ttf", 16);

    //Set up screen and make the textures for pixel tiles
    screen = SDL_SetVideoMode( scrWidth, scrHeight, 32, SDL_SWSURFACE );

    //draw the map items here
    if (0 == (map_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, mapSurfaceW,
        mapSurfaceH, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000)))
        error("Couldn't create map surface.");
    SDL_FillRect(map_surface, 0, 0xff000000);

    if (0 == (info_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, infoSurfaceW,
        infoSurfaceH, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000)))
        error("Couldn't create info surface.");
    SDL_FillRect(info_surface, 0, 0xff000000);

//    if (0 == (map_overlay = SDL_CreateRGBSurface(SDL_SWSURFACE, mapSurfaceW,
//        mapSurfaceH, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000)))
//        error();


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
 
    map_tex[background] = SDL_CreateRGBSurface(SDL_SWSURFACE, tileW, tileH, 32,
        0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);

    pixels = (Uint32 *) map_tex[background]->pixels;
    for (int i = 0; i < tileW * tileH; i++)
        pixels[i] = 0xff000000;
}

void init_snake()
{
    for (int i = 0; i < 5; i++)
    {
        sprintf(filename, "media/images/%03d.bmp", i + 10);
        if (!load_image(filename, &snake_tex[i]))
            error("Error loading images.");
    }
}

void init_score()
{
    for (int i = 0; i < 10; i++)
    {
        sprintf(filename, "media/images/%03d.bmp", i);
        if(!load_image(filename, &digit_tex[i]))
            error("Error loading images.");
    }
    digitRect.y = 80;
    digitRect.w = digit_tex[0]->w;
    digitRect.h = digit_tex[0]->h;
}

void cleanup()
{
    //Free the textures
    for (int i = 0; i < types_end; i++)
        if (map_tex[i] != 0)
            SDL_FreeSurface(map_tex[i]);

    for (int i = 0; i < 6; i++)
        if (snake_tex[i] != 0)
            SDL_FreeSurface(snake_tex[i]);

    if (map_overlay != 0)
        SDL_FreeSurface(map_overlay);
    if (map_surface != 0)
        SDL_FreeSurface(map_surface);
    if (screen != 0)
    SDL_FreeSurface(screen);

    //Quit SDL
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

void draw_debug()
{
    text_surface = TTF_RenderText_Solid(font, info_text, textcolor);
    //SDL_FillRect(map_overlay, 0, 0x00000000);
    SDL_BlitSurface(text_surface, NULL, screen, &textr);
    SDL_FreeSurface(text_surface);
}

void draw_screen()
{

    SDL_BlitSurface(map_surface, NULL, screen, &mapRect);
    //SDL_FillRect(info_surface, 0, 0xffff0000);
    //SDL_FillRect(score_surface, 0, 0xff00ff00);
    //SDL_BlitSurface(score_surface, NULL, info_surface, &scoreRect);
    SDL_BlitSurface(info_surface, NULL, screen, &infoRect);

    //SDL_BlitSurface(map_overlay, NULL, screen, 0);
    draw_debug();
    SDL_Flip(screen);
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

    // test
    textr.x = x * tileW;
    textr.y = y * tileH;
    sprintf(info_text, " <- X: %02d, Y: %02d", x, y);
    // test

    m->AddItem(x, y, i);
    m->Draw(x, y);
}

void xdo_move(CSnake * s, int dir)
{
    static Uint32 old_ticks = 0;
    Uint32 new_ticks;
    Uint32 delta = 100;
    static Uint32 delay = 300;
    static int oldDir = NODIR;

    new_ticks = SDL_GetTicks();
    delta = new_ticks - old_ticks;

    if (delta >= delay)
    {
        if (delay > 150)
            delay -= 25;
        old_ticks = new_ticks;
        switch (dir)
        {
        case LEFT:
            if (oldDir != RIGHT)
            {
                s->MoveX(-1);
                oldDir = dir;
            }
            else
                s->MoveX(1);
            break;
        case RIGHT:
            if (oldDir != LEFT)
            {
                s->MoveX(1);
                oldDir = dir;
            }
            else
                s->MoveX(-1);
            break;
        case UP:
            if (oldDir != DOWN)
            {
                s->MoveY(-1);
                oldDir = dir;
            }
            else
                s->MoveY(1);
            break;
        case DOWN:
            if (oldDir != UP)
            {
                s->MoveY(1);
                oldDir = dir;
            }
            else
                s->MoveY(-1);
            break;
        }
    }
}

void do_move(CSnake * s, int dir)
{
    static Uint32 old_ticks = 0;
    Uint32 new_ticks;
    Uint32 delta = 100;
    static Uint32 delay = 300;
    static int oldDir = NODIR;
    int directions[] = { 0, -1, 1, -1, 1 };

    void (CSnake::*mx)(int) = &CSnake::MoveX;
    void (CSnake::*my)(int) = &CSnake::MoveY;
    void (CSnake::*md)(int);
    
    new_ticks = SDL_GetTicks();
    delta = new_ticks - old_ticks;

    if (delta >= delay)
    {
        old_ticks = new_ticks;
        if (delay > 150)
            delay -= 25;

        if (dir == LEFT || dir == RIGHT)
            md = mx;
        else
            md = my;

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

    snake = new CSnake(mapWidth / 2, mapHeight / 2, 0, map_surface, snake_tex);
    snake->Grow(-1, 0);
    snake->Grow(-1, 0);
    snake->Draw();
    do_move(snake, RIGHT);

    place_food(theMap, 1);
    theMap->Draw();

    print_score();
    draw_screen();

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
        //theMap->Draw();
        draw_screen();
    }
    SDL_Rect r; r.x = 128; r.y = 256, r.w = 512, r.h = 256;
    SDL_BlitSurface( message, NULL, map_surface, &r );
    draw_screen();
    SDL_Delay(2000);
    SDL_FreeSurface(message);

    delete snake;
    cleanup();
 
    return 0;
}
