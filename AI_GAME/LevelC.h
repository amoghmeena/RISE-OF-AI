#pragma once
#include "Scene.h"

class LevelC : public Scene {
public:
    virtual void initialise() override;
    virtual void update(float delta_time) override;
    virtual void render(ShaderProgram* program) override;
    virtual ~LevelC();
};
