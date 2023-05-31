//
//  debug.h
//  flap
//
//  Created by Frank Collebrusco on 5/30/23.
//

#ifndef debug_h
#define debug_h
#include <iostream>
#include "sw/Stopwatch.h"

#define SIZE 0x800

//DEBUG
static ftime::Stopwatch debug(ftime::MILLISECONDS);
static float dump[SIZE];
static uint32_t dumpCount = SIZE - 1;

void debug_init(){
    dumpCount = SIZE - 1;
}

void debug_start_sample(){
    debug.reset_start();
}

void debug_end_sample(){
    dump[dumpCount--] = debug.stop();
}

bool debug_buffer_full(){
    return dumpCount == 0xFFFFFFFF;
}

void debug_output_result(){
    if (debug_buffer_full()){
        float avg = 0;
        for (int i = 0; i < SIZE; i++){
            avg += dump[i];
        }
        avg /= SIZE;
        std::cout << "avg time: " << avg << "\n";
    }
}

//int render_sys_stress_main() {
//    ftime::Stopwatch sw(ftime::SECONDS);
//    sw.start();
//    gl.init();
//    gl.loader.setShaderPath("Shaders/");
//    gl.loader.setAssetPath("Textures/");
//    ECS scene;
//
//    Window& window = gl.createWindow("flappy bird", 1080, 1920);
//    window.update();
//
//    Shader shader = gl.loader.UploadShader("vert", "frag");
//
//    TEXTURE_SLOT tex = gl.loader.UploadTexture("terrain", true);
//    auto seed = ((unsigned int)(sw.read(ftime::NANOSECONDS)*100000)) ^ 0x00000000A30B4F29;
//    std::cout << seed << "\n";
//    srand(seed);
//    for (int i = 0; i < 1000; i++) {
//        entID e = scene.newEntity();
//        scene.addComp<AtlasSprite>(e, tex, glm::ivec2(32, 64), glm::vec2(0.5, 15.f));
//
//        scene.addComp<Transform>(e, glm::vec2(frand(-1200, 1200), frand(-1000, 1000)), frand(0, 359), glm::vec2(100, 200) * frand(0.2,3));
//
//        scene.addComp<RenderFlag>(e, true);
//
//        scene.addComp<Shader>(e);
//        scene.getComp<Shader>(e) = shader;
//    }
//
//    entID cam = scene.newEntity();
//    scene.addComp<OrthoCamera>(cam, glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 1.f, 0.f), 0.001, 1000, -1);
//
//    sw.reset_start();
//
//    uint32_t dumpCount = 0xFFF;
//    float rendertimes[0x1000];
//    while (!window.should_close()){
//        static uint32_t fpsdelay = 0;
//        bool fps = window.keyboard[GLFW_KEY_F].down && !(fpsdelay++ % 100);
//        gl.clear();
//
//        ftime::Stopwatch debug(ftime::MILLISECONDS);
//        cameraSystem(scene, window);
//        rotateSystem(scene, window);
//        debug.reset_start();
//        renderSystem(scene);
//        float t = debug.stop_reset_start();
//        if (fps)
//            std::cout << "\tren sys: " << t << "\n";
//        if (dumpCount){
//            rendertimes[dumpCount--] = t;
//        } else {
//            float avg = 0;
//            for (int i = 0; i < 0x1000; i++){
//                avg += rendertimes[i];
//            }
//            avg /= 0x1000;
//            std::cout << "average render time: " << avg << "ms\n";
//            gl.destroy();
//            return 0;
//        }
//        window.update();
//        dt = sw.stop_reset_start();
//    }
//
//    gl.destroy();
//    return 0;
//}

#endif /* debug_h */
