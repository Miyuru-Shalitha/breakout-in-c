#include <SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define i32ernal static
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

struct Player
{
    SDL_FRect rect;
};

struct Tile
{
    SDL_FRect rect;
    bool is_active;
};

struct Ball
{
    SDL_FRect rect;
};

constexpr i32 HORIZONTAL_TILE_COUNT = 10;
constexpr i32 VERTICAL_TILE_COUNT = 5;
global_variable Tile tiles[HORIZONTAL_TILE_COUNT * VERTICAL_TILE_COUNT];

global_variable f32 ball_x_velocity = 0.0f;
global_variable f32 ball_y_velocity = 500.0f;

i32 main(i32 argc, char* argv[])
{
    i32 image_width = 0;
    i32 image_height = 0;
    i32 image_channel_count = 0;

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

    // Initialze the player values.
    Player player = {
        .rect = {
            .x = client_width * 0.5f - 75.0f,
            .y = client_height - 50.0f,
            .w = 150.0f,
            .h = 30.0f
        }
    };

    // Initialize the ball values.
    Ball ball = {
        .rect = {
            .x = player.rect.x + player.rect.w - player.rect.w * 0.5f - 10.0f,
            .y = player.rect.y - 20.0f,
            .w = 20.0f,
            .h = 20.0f
        }
    };

    // Initialize tile positions.
    const f32 TILE_HEIGHT = 30.0f;
    const f32 TILE_WIDTH = 50.0f;
    const f32 TILE_OFFSET = 10.0f;

    for (i32 y = 0; y < VERTICAL_TILE_COUNT; y++)
    {
        for (i32 x = 0; x < HORIZONTAL_TILE_COUNT; x++)
        {
            SDL_FRect rect = {
                .x = x * TILE_WIDTH + x * TILE_OFFSET + client_width * 0.5f
                        - (TILE_WIDTH + TILE_OFFSET) * HORIZONTAL_TILE_COUNT * 0.5f, // Subtract half of whole tileset witdh with offests.
                .y = y * TILE_HEIGHT + y * TILE_OFFSET,
                .w = TILE_WIDTH,
                .h = TILE_HEIGHT
            };
            tiles[x + y * HORIZONTAL_TILE_COUNT].rect = rect;
            tiles[x + y * HORIZONTAL_TILE_COUNT].is_active = true;
        }
    }

    // Helder variable for calculating the delta time.
    clock_t last_time = clock();

    bool running = true;

    SDL_Event event;

    while (running)
    {
        // Calculate delta time.
        clock_t current_time = clock();
        f32 dt = (f32)(current_time - last_time) / CLOCKS_PER_SEC;
        last_time = current_time;

        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
        }

        i32 mouse_x = 0;
        i32 mouse_y = 0;
        SDL_GetMouseState(&mouse_x, &mouse_y);

        // Player x controls.
        player.rect.x = mouse_x - player.rect.w * 0.5f;

        // Calculate the player x velocity in fixed time steps, in order to give the ball
        // an x velocity, when it hits the player.
        local_persist f32 accumilator = 0;
        accumilator += dt;

        local_persist f32 player_previouse_x_position = 0.0f;
        local_persist f32 player_x_velocity = 0.0f;

        f32 player_x_velocity_multiplier = 8.0f; // Higher this value means, higher the ball
                                                 // x velocity when it hits the player.

        // This inner block runs on every 0.1 seconds.
        if (accumilator >= 0.1f)
        {
            accumilator = 0;
            player_x_velocity = (player.rect.x - player_previouse_x_position) * player_x_velocity_multiplier;
            player_previouse_x_position = player.rect.x;
        }

        // Ball behaviour
        ball.rect.x += ball_x_velocity * dt;
        ball.rect.y -= ball_y_velocity * dt;

        // Ball horizontal collisions
        if (ball.rect.x < 0)
        {
            ball.rect.x = 0;
            ball_x_velocity *= -1.0f;
        }
        else if (ball.rect.x + ball.rect.w > client_width)
        {
            ball.rect.x = client_width - ball.rect.w;
            ball_x_velocity *= -1.0f;
        }

        // Ball vertical collisions
        if (ball.rect.y < 0)
        {
            ball.rect.y = 0.0f;
            ball_y_velocity *= -1.0f;
        }
        else if (
            ball.rect.y + ball.rect.h > player.rect.y &&
            ball.rect.x + ball.rect.w > player.rect.x &&
            ball.rect.x < player.rect.x + player.rect.w
        )
        {
            ball.rect.y = player.rect.y - ball.rect.h;
            ball_y_velocity *= -1.0f;
            ball_x_velocity = player_x_velocity;
        }

        // Ball collision with tiles
        for (i32 y = 0; y < VERTICAL_TILE_COUNT; y++)
        {
            for (i32 x = 0; x < HORIZONTAL_TILE_COUNT; x++)
            {
                Tile* tile = tiles + x + y * HORIZONTAL_TILE_COUNT;

                if (!tile->is_active)
                {
                    continue;
                }

                f32 ball_min_x = ball.rect.x;
                f32 ball_max_x = ball.rect.x + ball.rect.w;
                f32 ball_min_y = ball.rect.y;
                f32 ball_max_y = ball.rect.y + ball.rect.h;

                f32 tile_min_x = tile->rect.x;
                f32 tile_max_x = tile->rect.x + tile->rect.w;
                f32 tile_min_y = tile->rect.y;
                f32 tile_max_y = tile->rect.y + tile->rect.h;

                // Check if intersect.
                if (
                    ball_max_x > tile_min_x && ball_min_x < tile_max_x &&
                    ball_max_y > tile_min_y && ball_min_y < tile_max_y
                )
                {
                    tile->is_active = false;

                    f32 overlap_left = ball_max_x - tile_min_x;
                    f32 overlap_right = tile_max_x - ball_min_x;
                    f32 overlap_top = ball_max_y - tile_min_y;
                    f32 overlap_bottom = tile_max_y - ball_min_y;

                    // Get the minimum overlap for collision direction.
                    f32 min_overlap = overlap_left;

                    if (overlap_right < min_overlap)
                    {
                        min_overlap = overlap_right;
                    }

                    if (overlap_top < min_overlap)
                    {
                        min_overlap = overlap_top;
                    }

                    if (overlap_bottom < min_overlap)
                    {
                        min_overlap = overlap_bottom;
                    }

                    if (min_overlap == overlap_left || min_overlap == overlap_right)
                    {
                        ball_x_velocity *= -1;
                    }
                    else if (min_overlap == overlap_top || min_overlap == overlap_bottom)
                    {
                        ball_y_velocity *= -1;
                    }

                    break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw tiles.
        for (i32 y = 0; y < VERTICAL_TILE_COUNT; y++)
        {
            for (i32 x = 0; x < HORIZONTAL_TILE_COUNT; x++)
            {
                Tile tile = tiles[x + y * HORIZONTAL_TILE_COUNT];

                if (tile.is_active)
                {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                    SDL_RenderFillRectF(renderer, &tile.rect);
                }
            }
        }

        // Draw the player.
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRectF(renderer, &player.rect);

        // Draw the ball.
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRectF(renderer, &ball.rect);

        SDL_RenderPresent(renderer);
    }

    SDL_Quit();

    return 0;
}
