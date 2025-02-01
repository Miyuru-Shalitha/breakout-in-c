enum Level_One_Entity_Type
{
    LEVEL_ONE_ENTITY_TYPE_PLAYER,

    LEVEL_ONE_ENTITY_TYPE_COUNT
};

global_variable bool global_level_one_initialized;
global_variable Entity* global_player;

internal void update_level_one(
    f32 dt,
    Entity** entities, i32* entity_count,
    Tile** tiles, i32* tile_count,
    f32 tile_size, bool* key_downs
)
{
    if (!global_level_one_initialized)
    {
        global_level_one_initialized = true;
        *entities = (Entity*)malloc(sizeof(Entity) * LEVEL_ONE_ENTITY_TYPE_COUNT);

        if (!(*entities))
        {
            // TODO(Miyuru): Log
        }

        *entity_count = LEVEL_ONE_ENTITY_TYPE_COUNT;
        global_player = (*entities) + LEVEL_ONE_ENTITY_TYPE_PLAYER;
        global_player->position = { 0.0f, 0.0f };
        global_player->color = { 0, 255, 0, 255 };

        *tile_count = 100;
        *tiles = (Tile*)malloc(sizeof(Tile) * (*tile_count));

        if (!(*tiles))
        {
            // TODO(Miyuru): Log
        }

        for (i32 i = 0; i < *tile_count; i++)
        {
            (*tiles)[i] = {
                .position = { 0.0f + i * tile_size, 0.0f },
                .color = { 0, 0, 255, 255 },
                .layer = LAYER_TYPE_GROUND
            };
        }
    }

    global_player->size = { tile_size, tile_size };

    for (i32 i = 0; i < *tile_count; i++)
    {
        (*tiles)[i].size = { tile_size, tile_size };
    }

    if (key_downs[KEY_CODE_ESC])
    {
        quit_game();
    }

    // Handle player movement.
    f32 speed = 120.0f;
    Vec2 move_direction = {};

    if (key_downs[KEY_CODE_W])
    {
        move_direction.y = -1.0f;
    }

    if (key_downs[KEY_CODE_A])
    {
        move_direction.x = -1.0f;
    }

    if (key_downs[KEY_CODE_S])
    {
        move_direction.y = 1.0f;
    }

    if (key_downs[KEY_CODE_D])
    {
        move_direction.x = 1.0f;
    }

    move_direction = normalize_vec2(move_direction);
    global_player->position += move_direction * speed * dt;
    set_camera_follow_target({ global_player->position.x + tile_size * 0.5f,
                               global_player->position.y + tile_size *  0.5f });
}