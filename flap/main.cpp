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
#include <stdlib.h>
static float dt = 1/60;
static Graphics gl;

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
    glm::vec2 anchor;
    Transform() = default;
    Transform(glm::vec2 p, float r, glm::vec2 s){
        pos = p; rotation = r; scale = s; anchor = glm::vec2(0);
    }
    Transform(glm::vec2 p, float r, glm::vec2 s, glm::vec2 a){
        pos = p; rotation = r; scale = s; anchor = a;
    }
    void syncShader(Shader& shad){
        shad.bind();
        glm::mat4 m(1.0);
        m = glm::translate(m, glm::vec3(pos, 0.0));
        m = glm::rotate(m, glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0));
        m = glm::scale(m, glm::vec3(scale, 1.0));
        if (anchor != glm::vec2(0))
            m = glm::translate(m, glm::vec3(anchor, 0.0));
        shad.uMat4("uModel", m);
    }
};

class RenderFlag {
public:
    RenderFlag(bool f){flag = f;}
    bool flag;
    operator bool& () {return flag;}
};

class Velocity {
public:
    glm::vec2 velo;
    Velocity(glm::vec2 v){velo = v;}
    operator glm::vec2& () {return velo;}
};

// loop thru all with a transform, sprite, shader, and renderflag.
void renderSystem(ECS& scene){
    for (auto e : scene.view<Shader, RenderFlag>()){
        auto& flag = scene.getComp<RenderFlag>(e);
        if (flag){
            auto& shad = scene.getComp<Shader>(e);
            auto tf = scene.tryGetComp<Transform>(e);
            if (tf)
                tf->syncShader(shad);
            auto sp = scene.tryGetComp<AtlasSprite>(e);
            if (sp)
                sp->syncShader(shad);
            shad.bind();
            gl.DrawTile();
        }
    }
}

// little test that adds angular velo to anything with a transform by key input
void rotateSystem(ECS& scene, Window const& win){
    for (auto e : scene.view<Transform>()){
        auto& t = scene.getComp<Transform>(e);
        static float av = 0;
        float dir = 0;
        if (win.keyboard[GLFW_KEY_A].down){
            dir = 1;
        }
        if (win.keyboard[GLFW_KEY_D].down){
            dir = -1;
        }
        av += 0.00005 * dir;
        t.rotation += av;
    }
}

// loops thru ortho cameras in scene, if any, update view width if needed and upload to all shaders
void cameraSystem(ECS& scene, Window const& win){
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

// rand 0 to 1
float frand(){
    return ((float)rand()) / ((float)RAND_MAX);
}

// rand a to b
float frand(float a, float b){
    return (frand() * (b-a)) + a;
}

void flapSystem(ECS& scene, entID borb, Window& win, float dt){
    static const float g = 4000;
    auto& trans = scene.getComp<Transform>(borb);
    auto& velo = scene.getComp<Velocity>(borb);
    if (win.keyboard[GLFW_KEY_SPACE].pressed){
        velo.velo.y = glm::min(glm::max(velo.velo.y + 1000, 1000.f), 2500.f);
    } else if (trans.pos.y > (-win.frame.y/2)) {
        velo.velo.y -= g * dt;
    } else {
        velo.velo.y = 0;
    }
}

void kineticSystem(ECS& scene, float dt){
    for (auto e : scene.view<Transform, Velocity>()){
        auto& trans = scene.getComp<Transform>(e);
        auto& velo = scene.getComp<Velocity>(e).velo;
        trans.pos += (velo * dt);
    }
}

class isPipe {
    bool ispipe;
};

void newPipe(ECS& scene, Window& win, TEXTURE_SLOT tex, Shader& shader){
    float center = frand((-win.frame.y/2)+300, (win.frame.y/2)-300);
    auto top = scene.newEntity();
    auto bot = scene.newEntity();
    scene.addComp<AtlasSprite>(top, tex, glm::ivec2(32, 64), glm::vec2(1.f,0.f));
    scene.addComp<Transform>(top, glm::vec2(300+(win.frame.x/2), center), 180, glm::vec2(300, 600), glm::vec2(0, -1));
    scene.addComp<Shader>(top, shader);
    scene.addComp<RenderFlag>(top, true);
    scene.addComp<Velocity>(top, glm::vec2(-300, 0));
    scene.addComp<isPipe>(top);
    
    scene.addComp<AtlasSprite>(bot, tex, glm::ivec2(32, 64), glm::vec2(1.f,0.f));
    scene.addComp<Transform>(bot, glm::vec2(300+(win.frame.x/2), center), 0, glm::vec2(300, 600), glm::vec2(0, -1));
    scene.addComp<Shader>(bot, shader);
    scene.addComp<RenderFlag>(bot, true);
    scene.addComp<Velocity>(bot, glm::vec2(-300, 0));
    scene.addComp<isPipe>(bot);
}

void pipeSpawnSystem(ECS& scene, Window& win, TEXTURE_SLOT tex, Shader& shader){
    static ftime::Stopwatch newPipeTimer(ftime::SECONDS);
    static bool a = [&]()->bool{
        newPipeTimer.start();
        return true;
    } ();
    newPipeTimer.start();
    float npt = newPipeTimer.read();
    if (npt > 2.f){
        newPipeTimer.stop_reset_start();
        newPipe(scene, win, tex, shader);
    }
}

void collisionSystem(ECS& scene, entID borb, bool& col){
    static const float bw = 75.f* (9.f/16.f);
//    static uint32_t prevToggle = 0;
//    static entID prevScore[2] = {0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF};
//    static unsigned int score = 0;
    auto bt = scene.getComp<Transform>(borb);
    for (auto e : scene.view<isPipe>()){
        auto& trans = scene.getComp<Transform>(e);
        if (abs(trans.pos.x - bt.pos.x) > (112+bw)){continue;}
//        if (!(e == prevScore[0] || e == prevScore[1])){    // we're scoring a point
//            prevScore = e;
//            score++;
//            std::cout << (score>>1) << "pts!\n";
//        }
        bool hit;
        if (trans.rotation == 0.f){
            hit = ((trans.pos.y-300) - bt.pos.y) > -(bw+40);
        } else {
            hit = ((trans.pos.y+300) - bt.pos.y) < (bw+40);
        }
        if (hit){
            col = true;
            return;
        }
    }
}

void pipeCleanerSystem(ECS& scene, Window& win){
    for (auto e : scene.view<isPipe>()){
        auto trans = scene.tryGetComp<Transform>(e);
        if (trans && (trans->pos.x < (-win.frame.x/2)-300)){
            scene.removeEntity(e);
        }
    }
}

int flappy(){
    // init gfx elements
    gl.init();
    gl.loader.setAssetPath("Textures/");
    gl.loader.setShaderPath("Shaders/");
    auto& window = gl.createWindow("flappy", 1280, 720);
    auto shader = gl.loader.UploadShader("vert", "frag");
    auto tex = gl.loader.UploadTexture("borb", true);
    
    // init scene and entities
    ECS scene;
    // camera
    auto cam = scene.newEntity();
    scene.addComp<OrthoCamera>(cam).setViewWidth(window.frame.x);
    scene.getComp<OrthoCamera>(cam).update();
    // bg
    auto bg = scene.newEntity();
    auto bgshad = gl.loader.UploadShader("bgv", "bgf");
    scene.addComp<Shader>(bg, bgshad);
    scene.addComp<RenderFlag>(bg, true);
    // bird
    auto borb = scene.newEntity();
    scene.addComp<AtlasSprite>(borb, tex, glm::ivec2(32), glm::vec2(0));
    scene.addComp<Transform>(borb, glm::vec2(0), 0, glm::vec2(300));
    scene.addComp<Shader>(borb, shader);
    scene.addComp<RenderFlag>(borb, true);
    scene.addComp<Velocity>(borb, glm::vec2(0));
    
    ftime::Stopwatch dtimer(ftime::SECONDS);
    ftime::Stopwatch total(ftime::SECONDS);
    total.start();
    dtimer.reset_start();
    bool col = false;
    //DEBUG
                                                    ftime::Stopwatch debug(ftime::MILLISECONDS);
                                                    float dump[0x800];
                                                    uint32_t dumpCount = 0x7FF;
    while (!window.should_close()){
        gl.clear();
        
        if (!col){
            bgshad.uFloat("uTime", total.read());
            bgshad.uVec2("ures", glm::vec2(window.frame.x, window.frame.y));
            bgshad.uFloat("uAspect", window.aspect);
            pipeSpawnSystem(scene, window, tex, shader);
            pipeCleanerSystem(scene, window);
            flapSystem(scene, borb, window, dt);
            kineticSystem(scene, dt);
            collisionSystem(scene, borb, col);
        }
        cameraSystem(scene, window);
                                                    debug.reset_start();
        renderSystem(scene);
                                                    dump[dumpCount--] = debug.stop();
                                                    if (dumpCount == 0xFFFFFFFF){
                                                        float avg = 0;
                                                        for (int i = 0; i < 0x800; i++){
                                                            avg += dump[i];
                                                        }
                                                        avg /= 0x800;
                                                        std::cout << "avg render time: " << avg << "\n";
                                                        break;
                                                    }
        window.update();
        dt = dtimer.stop_reset_start();
        if (window.keyboard[GLFW_KEY_F].down)
            std::cout << 1/dt << " fps\n";
    }
    gl.destroy();
    return 0;
}

int render_sys_stress_main() {
    ftime::Stopwatch sw(ftime::SECONDS);
    sw.start();
    gl.init();
    gl.loader.setShaderPath("Shaders/");
    gl.loader.setAssetPath("Textures/");
    ECS scene;

    Window& window = gl.createWindow("flappy bird", 1080, 1920);
    window.update();

    Shader shader = gl.loader.UploadShader("vert", "frag");
    
    TEXTURE_SLOT tex = gl.loader.UploadTexture("terrain", true);
    auto seed = ((unsigned int)(sw.read(ftime::NANOSECONDS)*100000)) ^ 0x00000000A30B4F29;
    std::cout << seed << "\n";
    srand(seed);
    for (int i = 0; i < 1000; i++) {
        entID e = scene.newEntity();
        scene.addComp<AtlasSprite>(e, tex, glm::ivec2(32, 64), glm::vec2(0.5, 15.f));
        
        scene.addComp<Transform>(e, glm::vec2(frand(-1200, 1200), frand(-1000, 1000)), frand(0, 359), glm::vec2(100, 200) * frand(0.2,3));
        
        scene.addComp<RenderFlag>(e, true);
        
        scene.addComp<Shader>(e);
        scene.getComp<Shader>(e) = shader;
    }
    
    entID cam = scene.newEntity();
    scene.addComp<OrthoCamera>(cam, glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 1.f, 0.f), 0.001, 1000, -1);
    
    sw.reset_start();
    
    uint32_t dumpCount = 0xFFF;
    float rendertimes[0x1000];
    while (!window.should_close()){
        static uint32_t fpsdelay = 0;
        bool fps = window.keyboard[GLFW_KEY_F].down && !(fpsdelay++ % 100);
        gl.clear();
        
        ftime::Stopwatch debug(ftime::MILLISECONDS);
        cameraSystem(scene, window);
        rotateSystem(scene, window);
        debug.reset_start();
        renderSystem(scene);
        float t = debug.stop_reset_start();
        if (fps)
            std::cout << "\tren sys: " << t << "\n";
        if (dumpCount){
            rendertimes[dumpCount--] = t;
        } else {
            float avg = 0;
            for (int i = 0; i < 0x1000; i++){
                avg += rendertimes[i];
            }
            avg /= 0x1000;
            std::cout << "average render time: " << avg << "ms\n";
            gl.destroy();
            return 0;
        }
        window.update();
        dt = sw.stop_reset_start();
    }
    
    gl.destroy();
    return 0;
}

int main(){
//    render_sys_stress_main();
    flappy();
    return 0;
}
