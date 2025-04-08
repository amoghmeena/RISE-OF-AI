#pragma once
#include "Scene.h"

class LevelB : public Scene {
public:
    int ENEMY_COUNT = 1;
    virtual void initialise() override;
    virtual void update(float delta_time) override;
    virtual void render(ShaderProgram* program) override;
    virtual ~LevelB();
};
