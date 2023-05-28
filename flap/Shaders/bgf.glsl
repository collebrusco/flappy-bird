#version 410 core
out vec4 outColor;
in vec2 iUV;
in vec3 iPos;

uniform float uTime;
uniform float uresy;

float rand(vec2 co){
    return sin(dot(co, vec2(12.9898, 78.233)) * 43758.5453);
}

float rand(float a, float b, float seed){
    float rand = rand(vec2(seed, gl_FragCoord.x/1000));
    return (rand * (b-a)) + a;
}

void main(){
//    // option 2: pixelated noise
//    int noise_scale = 64;
//    vec4 clr = vec4(rand(vec2(float(int(iUV.x*1024)/noise_scale), float(int(iUV.y*1024)/noise_scale))),
//                    rand(vec2(float(int(iUV.y*1024)/noise_scale), float(int(iUV.x*1024)/noise_scale))),
//                    rand(vec2(rand(vec2(float(int(iUV.x*1024)/noise_scale), float(int(iUV.x*1024)/noise_scale))),
//                              rand(vec2(float(int(iUV.y*1024)/noise_scale), float(int(iUV.y*1024)/noise_scale))))), 1.f); // scaled noise
//    clr.xyz = mix(clr.xyz, vec3(0, 0, 0), 0.25);
//    outColor = clr;
    
//    option 3: fine noise
//    outColor = vec4(rand(iUV.xy) - 0.2f, rand(iUV.yx) - 0.2f, rand(vec2(rand(iUV.yy), rand(iUV.xx))) - 0.2f, 1.f);
    
//    option 4: single color
//    outColor = vec4(0, 0, 0, 1);
    
    vec4 clr = vec4(0.6, 0.65, 1.f, 1.f);
    clr = vec4(mix(clr.xyz, vec3(0.55, 0.5, 0.8f), 1 - (gl_FragCoord.y / uresy)), 1.f);
    
//    if (gl_FragCoord.y < rand(0, uresy/2, uTime)){
//        clr = vec4(0.3, 0.1, 0.4, 1.f);
//    }
    
    outColor = clr;
}
