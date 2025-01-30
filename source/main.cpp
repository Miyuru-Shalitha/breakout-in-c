#include <SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define internal static
#define global_variable static
#define local_persist static

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef size_t usize;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;
typedef long double f128;

struct Color
{
    i32 r;
    i32 g;
    i32 b;
    i32 a;
};

struct Vec2
{
    f32 x;
    f32 y;

    Vec2 operator+(const Vec2& other) const
    {
        return { x + other.x, y + other.y };
    }

    Vec2 operator-(const Vec2& other) const
    {
        return { x - other.x, y - other.y };
    }

    Vec2 operator*(f32 scaler) const
    {
        return { x * scaler, y * scaler };
    }

    Vec2 operator/(f32 scaler) const
    {
        if (!scaler)
        {
            return {};
        }

        return { x / scaler, y / scaler };
    }

    Vec2& operator+=(const Vec2& other)
    {
        x += other.x;
        y += other.y;

        return *this;
    }

    Vec2& operator-=(const Vec2& other)
    {
        x -= other.x;
        y -= other.y;

        return *this;
    }

    Vec2& operator*=(const Vec2& other)
    {
        x *= other.x;
        y *= other.y;

        return *this;
    }

    Vec2& operator/=(const Vec2& other)
    {
        x /= other.x;
        y /= other.y;

        return *this;
    }
};

internal Vec2 normalize_vec2(const Vec2& vector)
{
    f32 length = (f32)sqrt(vector.x * vector.x + vector.y * vector.y);

    if (!length)
    {
        return { 0.0f, 0.0f };
    }

    return (vector / length);
}

struct Entity
{
    Vec2 position;
    Vec2 size;
    Color color;
};

enum Scene_Type
{
    SCENE_TYPE_MAIN_MENU,
    SCENE_TYPE_LEVEL_ONE,

    SCENE_TYPE_COUNT
};

enum Key_Code
{
    KEY_CODE_W,
    KEY_CODE_A,
    KEY_CODE_S,
    KEY_CODE_D,
    KEY_CODE_SPACE,
    KEY_CODE_ESC,

    KEY_CODE_COUNT
};

void load_scene(Scene_Type scene);
void quit_game();

#include "main_menu.cpp"
#include "level_one.cpp"

global_variable Scene_Type global_active_scene;
global_variable Entity* global_entities;
global_variable i32 global_entity_count;
global_variable bool key_downs[KEY_CODE_COUNT];
global_variable bool global_running = true;

internal void load_scene(Scene_Type scene)
{
    global_active_scene = scene;
}

internal void quit_game()
{
    global_running = false;
}

i32 main(i32 argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("%s\n", SDL_GetError());
        return 1;
    }

    const i32 WINDOW_WIDTH = 1280;
    const i32 WINDOW_HEIGHT = 720;
    i32 client_width = 0;
    i32 client_height = 0;

    SDL_Window* window = SDL_CreateWindow("Paradox Breakout",
             SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
             WINDOW_WIDTH, WINDOW_HEIGHT,
             SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (!window)
    {
        printf("%s\n", SDL_GetError());
        return 1;
    }

    SDL_GetWindowSize(window, &client_width, &client_height);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!renderer)
    {
        printf("%s\n", SDL_GetError());
        return 1;
    }

    clock_t last_time = clock();
    global_active_scene = SCENE_TYPE_MAIN_MENU;
    SDL_Event event;

    while (global_running)
    {
        // Calculate delta time.
        clock_t current_time = clock();
        f32 dt = (f32)(current_time - last_time) / CLOCKS_PER_SEC;
        last_time = current_time;

        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                global_running = false;
            }

            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_SPACE:
                    {
                        key_downs[KEY_CODE_SPACE] = true;
                    } break;

                    case SDLK_ESCAPE:
                    {
                        key_downs[KEY_CODE_ESC] = true;
                    } break;

                    case SDLK_w:
                    {
                        key_downs[KEY_CODE_W] = true;
                    } break;

                    case SDLK_a:
                    {
                        key_downs[KEY_CODE_A] = true;
                    } break;

                    case SDLK_s:
                    {
                        key_downs[KEY_CODE_S] = true;
                    } break;

                    case SDLK_d:
                    {
                        key_downs[KEY_CODE_D] = true;
                    } break;
                }
            }

            if (event.type == SDL_KEYUP)
            {
                switch (event.key.keysym.sym)
                {
                    case SDLK_SPACE:
                    {
                        key_downs[KEY_CODE_SPACE] = false;
                    } break;

                    case SDLK_ESCAPE:
                    {
                        key_downs[KEY_CODE_ESC] = false;
                    } break;

                    case SDLK_w:
                    {
                        key_downs[KEY_CODE_W] = false;
                    } break;

                    case SDLK_a:
                    {
                        key_downs[KEY_CODE_A] = false;
                    } break;

                    case SDLK_s:
                    {
                        key_downs[KEY_CODE_S] = false;
                    } break;

                    case SDLK_d:
                    {
                        key_downs[KEY_CODE_D] = false;
                    } break;
                }
            }

            // TODO(Miyuru): Set client dimentions only checking the window resized event.
            if (event.type == SDL_WINDOWEVENT)
            {
                SDL_GetWindowSize(window, &client_width, &client_height);
            }
        }

        f32 tile_size = client_width / 40.0f;

        switch (global_active_scene)
        {
            case SCENE_TYPE_MAIN_MENU:
            {
                update_main_menu(dt, &global_entities, &global_entity_count, tile_size, key_downs);
            } break;

            case SCENE_TYPE_LEVEL_ONE:
            {
                update_level_one(dt, &global_entities, &global_entity_count, tile_size, key_downs);
            } break;
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw entities.
        for (i32 i = 0; i < global_entity_count; i++)
        {
            Entity entity = global_entities[i];
            Color color = entity.color;
            SDL_FRect rect = {
                .x = entity.position.x,
                .y = entity.position.y,
                .w = entity.size.x,
                .h = entity.size.y
            };
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderFillRectF(renderer, &rect);
        }

        SDL_RenderPresent(renderer);
    }

    SDL_Quit();

    return 0;
}
