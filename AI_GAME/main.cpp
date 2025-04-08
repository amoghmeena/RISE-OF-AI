#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define FIXED_TIMESTEP 0.0166666f
#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8
#define LEVEL1_LEFT_EDGE 5.0f

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL_mixer.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "cmath"
#include <ctime>
#include <vector>
#include "Entity.h"
#include "Map.h"
#include "Utility.h"
#include "Scene.h"
#include "LevelA.h"
#include "LevelB.h"      // Include LevelB header
#include "Menu.h"        // Assume Menu.h exists
#include "Lose.h"        // The new scene header

// ————— CONSTANTS ————— //
constexpr int WINDOW_WIDTH = 640,
WINDOW_HEIGHT = 480;

constexpr float BG_RED = 0.1922f,
BG_BLUE = 0.549f,
BG_GREEN = 0.9059f,
BG_OPACITY = 1.0f;

GLuint g_font_texture; // Global font texture

constexpr int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

enum AppStatus { RUNNING, TERMINATED };

int g_player_lives = 3;

// ————— GLOBAL VARIABLES ————— //
Scene* g_current_scene;
LevelA* g_level_a;
LevelB* g_level_b;       // Pointer for LevelB
Menu* g_menu;            // Pointer for the Menu scene
Lose* g_lose_scene;

SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
ShaderProgram g_shader_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

// Scene switching function
void switch_to_scene(Scene* scene)
{
    Mix_HaltMusic();
    g_current_scene = scene;
    g_current_scene->initialise();
}

void initialise();
void process_input();
void update();
void render();
void shutdown();

void initialise()
{
    // ————— VIDEO ————— //
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Hello, Scenes!",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);
    if (context == nullptr)
    {
        shutdown();
    }

#ifdef _WINDOWS
    glewInit();
#endif

    // ————— OPEN AUDIO (SDL_mixer) ————— //
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
    {
        SDL_Log("Mix_OpenAudio: %s", Mix_GetError());
    }

    // ————— GENERAL ————— //
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);
    glUseProgram(g_shader_program.get_program_id());
    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    // Load the font texture for drawing text
    g_font_texture = Utility::load_texture("assets/font1.png");

    // ————— SCENE SETUP ————— //
    g_menu = new Menu();         // Create the Menu scene
    g_level_a = new LevelA();    // Create LevelA scene
    g_level_b = new LevelB();    // Create LevelB scene
    g_lose_scene = new Lose();   // Create Lose scene
    switch_to_scene(g_menu);     // Start with the Menu scene

    // ————— BLENDING ————— //
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    if (g_current_scene != g_menu)
    {
        g_current_scene->get_state().player->set_movement(glm::vec3(0.0f));
    }

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_app_status = TERMINATED;
            break;

        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_q)
            {
                g_app_status = TERMINATED;
            }
            // If we're in the Menu, pressing Enter switches to LevelA.
            else if (event.key.keysym.sym == SDLK_RETURN)
            {
                if (g_current_scene == g_menu)
                {
                    switch_to_scene(g_level_a);
                }
            }
            // Handle jump key (space) for gameplay scenes.
            else if (event.key.keysym.sym == SDLK_SPACE)
            {
                if (g_current_scene != g_menu)
                {
                    if (g_current_scene->get_state().player->get_collided_bottom())
                    {
                        g_current_scene->get_state().player->jump();
                        Mix_PlayChannel(-1, g_current_scene->get_state().jump_sfx, 0);
                    }
                }
            }
            break;

        default:
            break;
        }
    }

    if (g_current_scene != g_menu)
    {
        const Uint8* key_state = SDL_GetKeyboardState(NULL);
        if (key_state[SDL_SCANCODE_LEFT])
        {
            g_current_scene->get_state().player->move_left();
        }
        else if (key_state[SDL_SCANCODE_RIGHT])
        {
            g_current_scene->get_state().player->move_right();
        }
        if (glm::length(g_current_scene->get_state().player->get_movement()) > 1.0f)
        {
            g_current_scene->get_state().player->normalise_movement();
        }
    }
}

void update()
{
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;
    delta_time += g_accumulator;
    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }
    while (delta_time >= FIXED_TIMESTEP)
    {
        g_current_scene->update(FIXED_TIMESTEP);
        delta_time -= FIXED_TIMESTEP;
    }
    g_accumulator = delta_time;

    // Check for scene transitions based on next_scene_id:
    int nextSceneId = g_current_scene->get_state().next_scene_id;
    if (nextSceneId == 1 && g_current_scene != g_level_b) {
        // For example, LevelA sets next_scene_id to 1 to indicate switch to LevelB.
        switch_to_scene(g_level_b);
    }

    // ————— PLAYER CAMERA ————— //
    if (g_current_scene != g_menu) {
        g_view_matrix = glm::mat4(1.0f);
        if (g_current_scene->get_state().player->get_position().x > LEVEL1_LEFT_EDGE) {
            g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-g_current_scene->get_state().player->get_position().x, 3.75, 0));
        }
        else {
            g_view_matrix = glm::translate(g_view_matrix, glm::vec3(-5, 3.75, 0));
        }
    }

    // If lives run out, overlay "You Lose" (handled in render).
}

void render()
{
    g_shader_program.set_view_matrix(g_view_matrix);
    glClear(GL_COLOR_BUFFER_BIT);
    g_current_scene->render(&g_shader_program);
    if (g_player_lives <= 0)
    {
        // Draw "You Lose" overlay using screen-space coordinates.
        glm::mat4 original_projection = g_projection_matrix;
        glm::mat4 original_view = g_view_matrix;
        glm::mat4 hud_projection = glm::ortho(0.0f, static_cast<float>(WINDOW_WIDTH),
            0.0f, static_cast<float>(WINDOW_HEIGHT),
            -1.0f, 1.0f);
        g_shader_program.set_projection_matrix(hud_projection);
        g_shader_program.set_view_matrix(glm::mat4(1.0f));
        // Position the text centered (adjust as needed)
        glm::vec3 text_position(static_cast<float>(WINDOW_WIDTH) / 2 - 150.0f,
            static_cast<float>(WINDOW_HEIGHT) / 2,
            0.0f);
        Utility::draw_text(&g_shader_program, g_font_texture, "You Lose", 0.5f, 0.0f, text_position);
        g_shader_program.set_projection_matrix(original_projection);
        g_shader_program.set_view_matrix(original_view);
    }
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();
    delete g_menu;
    delete g_level_a;
    delete g_level_b;
    delete g_lose_scene;
}

int main(int argc, char* argv[])
{
    initialise();
    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
    }
    shutdown();
    return 0;
}
