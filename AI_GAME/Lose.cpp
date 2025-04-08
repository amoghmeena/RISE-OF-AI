#include "Lose.h"
#include "Utility.h"


void Lose::initialise() {
    m_game_state.next_scene_id = -2;

    // Load the "Lose.png" image
    m_lose_texture_id = Utility::load_texture("assets/Lose.png");
}

void Lose::update(float delta_time) {
    // Optionally, wait for user input here:
    // e.g., pressing Enter => exit the game or go back to menu
    // 
    // const Uint8* keys = SDL_GetKeyboardState(nullptr);
    // if (keys[SDL_SCANCODE_RETURN]) {
    //     // Switch to some scene or exit
    // }
}

void Lose::render(ShaderProgram* program) {
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT);

    // A full-screen quad spanning -5..+5 horizontally, -3.75..+3.75 vertically
    float vertices[] = {
        -5.0f, -3.75f,
         5.0f, -3.75f,
         5.0f,  3.75f,
        -5.0f, -3.75f,
         5.0f,  3.75f,
        -5.0f,  3.75f
    };

    float tex_coords[] = {
        0.0f, 1.0f,  // lower-left corner of Lose.png
        1.0f, 1.0f,  // lower-right corner
        1.0f, 0.0f,  // upper-right corner
        0.0f, 1.0f,  // lower-left corner
        1.0f, 0.0f,  // upper-right corner
        0.0f, 0.0f   // upper-left corner
    };

    glm::mat4 model_matrix = glm::mat4(1.0f);
    program->set_model_matrix(model_matrix);

    glUseProgram(program->get_program_id());
    glBindTexture(GL_TEXTURE_2D, m_lose_texture_id);

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->get_position_attribute());

    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, tex_coords);
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    // Draw the quad
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Disable the arrays
    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}
