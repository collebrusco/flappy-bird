#version 410 core
out vec4 outColor;
in vec2 iUV;
in vec3 iPos;

uniform float uTime;
uniform vec2 ures;
uniform float uAspect;

vec2 n_res = gl_FragCoord.xy / ures;
vec2 i_res = vec2(n_res.x * uAspect, n_res.y);

float rand(vec2 co){
    return sin(dot(co, vec2(12.9898, 78.233)) * 43758.5453);
}

float interp(float x){
    return ((x * (x * 6.0 - 15.0) + 10.0) * x * x * x);
}

float interp(float a, float b, float x){
    float ab = (b - a);
    return (ab * interp(x)) + a;
}

vec2 gradient(int x, int y){
    uint a = x;
    uint b = y;
    a *= 0xBAF3C90F;
    b ^= (a << 16) | (a >> 16);
    b *= 0x5454FAFF;
    a ^= (b << 16) | (b >> 16);
    a *= 0xDA442859;
    float theta = a * (3.14159265 / (2 * (0x10000000)));
    return vec2(cos(theta), sin(theta));
}

vec2 gradient(int x, int y, float speed){
    uint a = x;
    uint b = y;
    a *= 0xBAF3C90F;
    b ^= (a << 16) | (a >> 16);
    b *= 0x5454FAFF;
    a ^= (b << 16) | (b >> 16);
    a *= 0xDA442859;
    float theta = a * (3.14159265 / (2 * (0x10000000)));
    return vec2(cos(theta+(uTime*speed)), sin(theta+(uTime*speed)));
}

float perlin(vec2 pos, float rotation, vec2 offset, vec2 scale){
    // translate, scale
    pos += offset;
    pos *= scale;
    
    // establish grid
    int x0 = int(floor(pos.x));
    int y0 = int(floor(pos.y));
    int x1 = x0 + 1;
    int y1 = y0 + 1;
    
    // offset vectors, from grid corners to fragment
    vec2 botL = (pos - vec2(float(x0), float(y0))) / 1.41;
    vec2 topR = (pos - vec2(float(x1), float(y1))) / 1.41;
    vec2 topL = (pos - vec2(float(x0), float(y1))) / 1.41;
    vec2 botR = (pos - vec2(float(x1), float(y0))) / 1.41;
    
    // dot each offset vector with random gradient at each grid corner
    float BL = dot(botL, gradient(x0, y0, rotation));
    float TR = dot(topR, gradient(x1, y1, rotation));
    float TL = dot(topL, gradient(x0, y1, rotation));
    float BR = dot(botR, gradient(x1, y0, rotation));
    
    // interpolate, scale 0 to 1
    return (interp(interp(BL, BR, pos.x - floor(pos.x)), interp(TL, TR, pos.x - floor(pos.x)), pos.y - floor(pos.y)) * 0.5) + 0.5;
}

float perlin(vec2 pos, float rotation, vec2 offset){
    return perlin(pos, rotation, offset, vec2(1));
}

float perlin(vec2 pos, float rotation){
    return perlin(pos, rotation, vec2(0), vec2(1));
}

float perlin(vec2 pos){
    return perlin(pos, 0, vec2(0), vec2(1));
}

void main(){
    
    // base
    vec4 clr = vec4(0.6, 0.65, 1.f, 1.f);
    
    // sun location
    float distToSun = distance(i_res, vec2(0.3, 0.8));
    
    // radial gradient away from sun
    clr = vec4(mix(clr.xyz, vec3(0.55, 0.5, 0.8f), distToSun), 1.f);
    
    // 7 layers of perlin noise
    int i;
    vec3 noise = vec3(0.f);
    int div = 0;
    for (i = 1; i < 0x1000; i <<= 1) {
        noise += vec3(perlin((i_res * float(i)/5) + vec2(float(i)),div,vec2(uTime)));
        div++;
    }
    noise /= div;
    clr.xyz *= (noise / 2) + 0.5;
    
    // sun on top of noise
    if (distToSun < (0.1 - 0.01 * perlin(i_res*200, 24))){
        clr = vec4(mix(vec3(0.95, 0.95, 0.4), vec3(0.95, 0.95, 0.35) * ((perlin(i_res*5, 12.f, vec2(uTime/10, 0)) * 0.3)+0.7), distToSun * distToSun * 100), clr.w);
    }
    // bloom
    clr.xyz = mix(clr.xyz,
                  mix(vec3(0.8, 0.6, 0.15) * ((0.2 * perlin(i_res*8, 2)) + 0.8),
                      clr.xyz,
                      min((distToSun - 0.1) * 5 * (perlin((i_res/2) + uTime/4, 2)*4) * (perlin(i_res*12,2)*2), 1)),
                  min(1, distToSun * 8));

    outColor = clr;
}
