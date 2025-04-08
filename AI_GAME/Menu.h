#pragma once
#include "Scene.h"

class Menu : public Scene {
public:
    GLuint m_menu_texture_id;  

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};
