#version 410 core
/*
 This shader is a basic example of applying sprite sheet textures to 2d objects.
 upload the texture slot, the pix w & h of the sprite, and position of the sprite's bottom left corner in 64-pixel units
 This is configured for a 1024x1024 sprite sheet with 64 pix position units, this can be changed
 */
uniform sampler2D uTexslot;
uniform ivec2 uSpriteWH;
uniform vec2 uSpriteSheetCoords;

uniform float uTime;
uniform vec2 ures;
uniform float uAspect;

vec2 n_res = gl_FragCoord.xy / ures;
vec2 i_res = vec2(n_res.x * uAspect, n_res.y);

out vec4 outColor;
in vec2 iUV;


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
    float theta = a * (3.14159265 / ((0x10000000)));
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
    float theta = a * (3.14159265 / ((0x10000000)));
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
    vec2 normTexCoords = vec2(0.0);
    normTexCoords.x = ((uSpriteSheetCoords.x * 64) + (iUV.x * uSpriteWH.x)) / 1024.0;
    normTexCoords.y = 1 - ((uSpriteSheetCoords.y * 64) + (iUV.y * uSpriteWH.y)) / 1024.0;
    ivec2 pixelPos = ivec2(normTexCoords * 1024);
    vec4 pixelColor = texelFetch(uTexslot, pixelPos, 0);
    if (pixelColor.a != 1.0f){
        discard;
    }
    vec4 c = texture(uTexslot, normTexCoords);
    //apply additional shading here...
    for (int i = 1; i < 2; i++){
        float sn = perlin(i_res * 50 * i, 3, vec2(uTime * 20 * (5-i), uTime * 20));
        if (sn > 0.7){
            c.xyz = vec3(1.f);//mix(clr.xyz, vec3(1.f), (sn*sn)/0.81);
        }
    }
    outColor = c;
}
