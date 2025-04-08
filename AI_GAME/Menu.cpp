#include "Menu.h"
#include "Utility.h"

void Menu::initialise() {
    m_game_state.next_scene_id = -1;
    m_menu_texture_id = Utility::load_texture("assets/Menu.png");
}

void Menu::update(float delta_time) {
    const Uint8 *keys = SDL_GetKeyboardState(nullptr);
    if (keys[SDL_SCANCODE_RETURN]) {
        m_game_state.next_scene_id = 1; // Set to your actual level scene ID
    }
}

void Menu::render(ShaderProgram *program) {
    glClear(GL_COLOR_BUFFER_BIT);

    // Fullscreen quad coordinates (camera space)
    float vertices[] = {
        -5.0f, -3.75f,
         5.0f, -3.75f,
         5.0f,  3.75f,
        -5.0f, -3.75f,
         5.0f,  3.75f,
        -5.0f,  3.75f
    };

    float tex_coords[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f
    };

    glm::mat4 model_matrix = glm::mat4(1.0f);
    program->set_model_matrix(model_matrix);

    glUseProgram(program->get_program_id());
    glBindTexture(GL_TEXTURE_2D, m_menu_texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}
