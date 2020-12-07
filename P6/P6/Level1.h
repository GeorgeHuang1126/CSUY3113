

#ifndef Level1_h
#define Level1_h

#include "Scene.h"

class Level1 : public Scene
{
public:
    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(ShaderProgram* program) override;
    void Reset() override;
};

#endif /* Level1_hpp */
