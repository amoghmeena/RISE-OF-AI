#pragma once
#include "Scene.h"

class Lose : public Scene {
public:
    GLuint m_lose_texture_id;

    void initialise() override;
    void update(float delta_time) override;
    void render(ShaderProgram* program) override;
};
