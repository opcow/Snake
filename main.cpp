// main.cpp
#include "sdl\include\SDL.h"
#include <time.h>

#include "Tile.h"
#include "Snake.h"
#include "textures.h"

const int scrWidth = 768, scrHeight = 768;
const int mapWidth = scrWidth / 16, mapHeight = scrWidth / 16;

SDL_Surface * screen = NULL;

enum {
    background,
    sky,
    dirt,
    sand,
    wall,
    food,
    types_end
};

CTile * tile[types_end + 1];
CTile * snake_tiles[6];

char map[(scrHeight / 16) * (scrWidth / 16)];

SDL_Surface * textures[types_end + 1];
SDL_Surface * snake_tex[7];


void init()
{
    int i, j;
    Uint32 colors[types_end + 1] = { 0xff000000, 0xff8bcffa, 0xffa54607, 0xfff2e37c, 0xff808080, 0xffa00000 };
    Uint32 * pixels;

    //Start SDL
    SDL_Init( SDL_INIT_EVERYTHING );

    //Set up screen and make the textures for pixel tiles
    screen = SDL_SetVideoMode( scrWidth, scrHeight, 32, SDL_SWSURFACE );

    for (int i = 0; i < types_end + 1; i++)
        textures[i] = SDL_CreateRGBSurface(SDL_SWSURFACE, 16, 16, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);


    // initialize the colors for the tiles
    for (i = 0; i < types_end + 1; i++)
    {
        pixels = (Uint32 *) textures[i]->pixels;
        for (j = 0; j < 16 * 16; j++)
            pixels[j] = colors[i];
    }
    snake_tiles[0] = new CTile(SDL_LoadBMP("headr.bmp"), screen);
    snake_tiles[1] = new CTile(SDL_LoadBMP("headl.bmp"), screen);
    snake_tiles[2] = new CTile(SDL_LoadBMP("headu.bmp"), screen);
    snake_tiles[3] = new CTile(SDL_LoadBMP("headd.bmp"), screen);
    snake_tiles[4] = new CTile(SDL_LoadBMP("body.bmp"), screen);
    snake_tiles[6] = new CTile(textures[background], screen);

    textures[food] = SDL_LoadBMP("food.bmp");
}

void cleanup()
{
    //Free the textures
    for (int i = 0; i < types_end + 1; i++)
        SDL_FreeSurface(textures[i]);

    //Quit SDL
    SDL_Quit();
}

int keycheck(CSnake * s)
{
    SDL_Event event;
    SDL_PollEvent( &event );
    static int dirx = 0, diry = 0;

    switch( event.type ){
        /* Keyboard event */
    case SDL_KEYDOWN:
        switch(event.key.keysym.sym){
        case SDLK_LEFT:
        case SDLK_a:
            if (dirx != 1)
            {
                dirx = -1;
                diry = 0;
            }
            break;
        case SDLK_RIGHT:
        case SDLK_d:
            if (dirx != -1)
            {
                dirx = 1;
                diry = 0;
            }
            break;
        case SDLK_UP:
        case SDLK_w:
            if (diry != 1)
            {
                diry = -1;
                dirx = 0;
            }
            break;
        case SDLK_DOWN:
        case SDLK_s:
            if (diry != -1)
            {
                diry = 1;
                dirx = 0;
            }
            break;
        case SDLK_q:
            return 2;
            break;
        default:
            return 0;
            break;
        }
        break;
        /* SDL_QUIT event (window close) */
    case SDL_QUIT:
        return 2;
        break;
    }
    if (dirx != 0)
        s->MoveX(dirx);
    if (diry != 0)
        s->MoveY(diry);
    return 1;

}

void init_map()
{
    memset(map, 0, mapWidth * mapHeight);
    memset(map, wall, mapWidth);
    memset(map + mapWidth * (mapHeight - 1), wall, mapWidth);
    for (int i = 1; i < mapHeight - 1; i++)
    {
        map[i * mapWidth] = wall;
        map[i * mapWidth + mapWidth - 1] = wall;
    }

    int x, y;
    for (int i = 0; i < 200; i++)
    {
        x = 1 + (rand() % (mapWidth - 2));
        y = 1 + (rand() % (mapHeight - 2));
        map[x + y * mapWidth] = food;
    }
}

void draw_map()
{
    for (int i = 0; i < scrHeight / 16; i++)
    {
        for (int j = 0; j < mapWidth; j++)
        {
            tile[map[j + i * mapWidth]]->Draw(j * 16, i * 16);
        }
    }
}

int checkmap(CSnake * s)
{
    int x, y, i;

    s->GetPos(x, y);
    i = (int) map[x + y * mapWidth];
    map[x + y * mapWidth] = background;
    return i;
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

int main( int argc, char* args[] )
{
    CSnake * snake;
    int k, block;

    srand((unsigned)time( NULL ));
    init();
    init_map();
    for (int i = 0; i < types_end + 1; i++)
        tile[i] = new CTile(textures[i], screen);

    draw_map();
    snake = new CSnake(10, 10, 0, snake_tiles);
    snake->Grow(9, 10);
    snake->Grow(8, 10);
    snake->Draw();

    SDL_Flip(screen);

    while (2 != (k = keycheck(snake)))
    {
        if (k == 1)
        {
            if (collision(snake))
                break;
            block = checkmap(snake);
            if (block == wall)
                break;
            else if (block == food)
            {
                snake->Grow();
            }

            SDL_Flip(screen);
            SDL_Delay(100);
        }
    }

    delete snake;
    cleanup();

    for (int i = 0; i < types_end + 1; i++)
        delete tile[i];

    for (int i = 0; i < 6; i++)
        delete snake_tiles[i];

    return 0;
}
