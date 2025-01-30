enum Main_Menu_Entity_Type
{
    MAIN_MENU_ENTITY_TYPE_START_BUTTON,
    MAIN_MENU_ENTITY_TYPE_QUIT_BUTTON,

    MAIN_MENU_ENTITY_TYPE_COUNT
};

global_variable bool global_main_menu_initialized;
global_variable Entity* global_start_button;

internal void update_main_menu(f32 dt, Entity** entities, i32* entity_count, f32 tile_size, bool* key_downs)
{
    if (!global_main_menu_initialized)
    {
        global_main_menu_initialized = true;
        *entities = (Entity*)malloc(sizeof(Entity) * MAIN_MENU_ENTITY_TYPE_COUNT);

        if (!(*entities))
        {
            // TODO(Miyuru): Log
        }

        *entity_count = MAIN_MENU_ENTITY_TYPE_COUNT;
        global_start_button = (*entities) + MAIN_MENU_ENTITY_TYPE_START_BUTTON;
        global_start_button->position = { 0.0f, 0.0f };
        global_start_button->color = { 0, 0, 255, 255 };
    }

    global_start_button->size = { 100.0f, 20.0f };

    if (key_downs[KEY_CODE_SPACE])
    {
        load_scene(SCENE_TYPE_LEVEL_ONE);
    }

    if (key_downs[KEY_CODE_ESC])
    {
        quit_game();
    }
}