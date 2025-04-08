#include "LevelC.h"
#include "Utility.h"
#include <vector>
#include <SDL.h>  

#define LEVEL_WIDTH 14
#define LEVEL_HEIGHT 8


unsigned int LEVEL_DATA_C[] = {
   
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,1,1,0,0,0,1,1,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,0,0,1,0,0,0,0,0,1,0,0,0,1,
    1,0,0,0,0,1,1,1,0,0,0,0,0,1,
    1,0,0,0,0,0,0,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

extern int g_player_lives;
extern bool is_end;

LevelC::~LevelC() {
    delete m_game_state.map;
    delete m_game_state.player;
    delete[] m_game_state.enemies;
    Mix_FreeChunk(m_game_state.jump_sfx);
    Mix_FreeMusic(m_game_state.bgm);
}

void LevelC::initialise() {
    // Create the map for LevelC using LEVEL_DATA_C.
    GLuint map_texture_id = Utility::load_texture("assets/tile1.png");
    m_game_state.map = new Map(LEVEL_WIDTH, LEVEL_HEIGHT, LEVEL_DATA_C, map_texture_id, 1.0f, 2, 1);

    // Setup player using the same texture and walking animation as prior levels.
    GLuint player_texture_id = Utility::load_texture("assets/walking.png");
    int player_walking_animation[2][8] = {
        {8, 9, 10, 11, 12, 13, 14, 15},
        {0, 1, 2, 3, 4, 5, 6, 7}
    };
    glm::vec3 acceleration = glm::vec3(0.0f, -4.81f, 0.0f);
    m_game_state.player = new Entity(
        player_texture_id,         // texture id
        5.0f,                      // speed
        acceleration,              // acceleration
        5.0f,                      // jumping power
        player_walking_animation,  // walking animation frames
        0.0f,                      // animation time
        4,                         // animation frame count
        0,                         // current animation index
        8,                         // animation columns
        2,                         // animation rows
        1.0f,                      // width
        1.0f,                      // height
        PLAYER
    );
    // Set a new starting position for the player in LevelC.
    m_game_state.player->set_position(glm::vec3(2.0f, -2.0f, 0.0f));
    m_game_state.player->set_jumping_power(5.0f);

    // -------------------------
    // Enemy initialization for LevelC
    // -------------------------
    GLuint enemy_texture_id = Utility::load_texture("assets/ghost.png");
    SDL_Log("LevelC - Enemy texture id: %u", enemy_texture_id);
    const int ENEMY_COUNT = 1;
    m_game_state.enemies = new Entity[ENEMY_COUNT];
    m_number_of_enemies = ENEMY_COUNT;  // Set the enemy count for LevelC

    // Construct the enemy using the AI constructor.
    
    m_game_state.enemies[0] = Entity(enemy_texture_id, 1.0f, 1.0f, 1.0f, ENEMY, FYLER, WALKING);
    // Position the enemy in LevelC (adjust as needed).
    SDL_Log("LevelC - Enemy initial position set to (6, -3.5, 0)");
    m_game_state.enemies[0].set_position(glm::vec3(6.0f, -3.5f, 0.0f));
    m_game_state.enemies[0].set_movement(glm::vec3(0.0f));
    m_game_state.enemies[0].set_acceleration(glm::vec3(0.0f, -9.81f, 0.0f));

    // -------------------------
    // Audio setup for LevelC: same BGM and jump SFX as before.
    // -------------------------
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    m_game_state.bgm = Mix_LoadMUS("assets/time_for_adventure.mp3");
    Mix_PlayMusic(m_game_state.bgm, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
    m_game_state.jump_sfx = Mix_LoadWAV("assets/jump.wav");
    m_game_state.hurt_sfx = Mix_LoadWAV("assets/hurt.wav");
    m_game_state.win_sfx = Mix_LoadWAV("assets/power_up.wav");
}

void LevelC::update(float delta_time) {
    // Update the player.
    m_game_state.player->update(delta_time, m_game_state.player, m_game_state.enemies, 1, m_game_state.map);

    // Update all enemies (here, only one enemy).
    for (int i = 0; i < 1; i++) {
        m_game_state.enemies[i].update(delta_time, m_game_state.player, NULL, NULL, m_game_state.map);
    }

    // Check collisions between the player and enemy.
    for (int i = 0; i < 1; i++) {
        if (m_game_state.player->check_collision(&m_game_state.enemies[i])) {
            g_player_lives--;
            Mix_PlayChannel(-1, m_game_state.hurt_sfx, 0);
            m_game_state.player->set_position(glm::vec3(2.0f, -2.0f, 0.0f));
        }
    }

    // (Optional) Add level transition logic if desired.
    if (m_game_state.player->get_position().x > (LEVEL_WIDTH - 3)) {
         
        is_end = true;
        Mix_PlayChannel(-1, m_game_state.win_sfx, 0);
    }
}

void LevelC::render(ShaderProgram* program) {
    m_game_state.map->render(program);
    m_game_state.player->render(program);
    for (int i = 0; i < 1; i++) {
        m_game_state.enemies[i].render(program);
    }
}
