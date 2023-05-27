//
//  main.cpp
//  flap
//
//  Created by Frank Collebrusco on 5/26/23.
//

#include <iostream>
#include "flgl/flgl.h"
#include "ecs/ECS.h"
#include "sw/Stopwatch.h"
#include "Camera.h"
static float dt = 1;

class AtlasSprite {
    TEXTURE_SLOT slot;
    glm::ivec2 dims;
    glm::vec2 pos;
public:
    AtlasSprite() = default;
    AtlasSprite(TEXTURE_SLOT s, glm::ivec2 d, glm::vec2 p){
        slot = s; dims = d; pos = p;
    }
    void syncShader(Shader& shad){
        shad.bind();
        shad.uInt("uTexture", slot);
        shad.uIVec2("uSpriteWH", dims);
        shad.uVec2("uSpriteSheetCoords", pos);
    }
};
#include <glm/ext/matrix_transform.hpp>
class Transform {
public:
    glm::vec2 pos;
    float rotation;
    glm::vec2 scale;
    Transform() = default;
    Transform(glm::vec2 p, float r, glm::vec2 s){
        pos = p; rotation = r; scale = s;
    }
    void syncShader(Shader& shad){
        glm::mat4 m(1.0);
        m = glm::translate(m, glm::vec3(pos, 0.0));
        m = glm::rotate(m, glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0));
        m = glm::scale(m, glm::vec3(scale, 1.0));
        shad.uMat4("uModel", m);
    }
};

Graphics gl;
ECS scene;

void preRenderSystem(ECS scene){
    for (auto e : scene.view<AtlasSprite, Shader, Transform>()){
        auto& shad = scene.getComp<Shader>(e);
        (scene.getComp<Transform>(e)).syncShader(shad);
        (scene.getComp<AtlasSprite>(e)).syncShader(shad);
    }
}

void rotateSystem(ECS scene, Window const& win){
    for (auto e : scene.view<Transform>()){
        auto& t = scene.getComp<Transform>(e);
        static float av = 0;
        float dir = 0;
        if (win.keyboard.keys[GLFW_KEY_A].down){
            dir = 1;
        }
        if (win.keyboard.keys[GLFW_KEY_D].down){
            dir = -1;
        }
        av += 0.0001 * dir;
        t.rotation += av;
    }
}

void cameraSystem(ECS scene, Window const& win){
    for (auto e : scene.view<OrthoCamera>()){
        auto& cam = scene.getComp<OrthoCamera>(e);
        if (cam.getViewWidth() != win.frame.x){
            cam.setViewWidth(win.frame.x);
            cam.update();
        }
        Graphics::forEachShader([&](Shader s)->void{
            s.uMat4("uView", cam.View());
            s.uMat4("uProj", cam.Proj());
        });
    }
}

int main(int argc, const char * argv[]) {
    gl.init();
    gl.loader.setShaderPath("Shaders/");
    gl.loader.setAssetPath("Textures/");

    Window& window = gl.createWindow("flappy bird", 480, 480);
    window.update();

    Shader shader = gl.loader.UploadShader("vert", "frag");
    
    TEXTURE_SLOT tex = gl.loader.UploadTexture("terrain", true);
    
    entID e = scene.newEntity();
    scene.addComp<AtlasSprite>(e, tex, glm::ivec2(32, 64), glm::vec2(0.5, 15.f));
    
    scene.addComp<Transform>(e, glm::vec2(0), 45.f, glm::vec2(500, 1000));
    
    scene.addComp<Shader>(e);
    scene.getComp<Shader>(e) = shader;
    
    entID cam = scene.newEntity();
    scene.addComp<OrthoCamera>(cam, glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 1.f, 0.f), 0.001, 1000, -1);
    
    ftime::Stopwatch sw(ftime::SECONDS);
    
    sw.reset_start();
    while (!window.should_close()){
        gl.clear();
        
        cameraSystem(scene, window);
        rotateSystem(scene, window);
        preRenderSystem(scene);
        
        gl.DrawTile();
        
        window.update();
        dt = sw.stop_reset_start();
        static uint32_t fpsdelay = 0;
        if (window.keyboard.keys[GLFW_KEY_F].down && !(fpsdelay++ % 100))
            std::cout << 1 / dt << "fps\n";
    }
    
    gl.destroy();
    return 0;
}
