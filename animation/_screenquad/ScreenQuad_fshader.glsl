#version 330 core
uniform sampler2D tex;
uniform float tex_width;
uniform float tex_height;
in vec2 uv;
out vec3 color;
uniform float offset[3] = float[] (0.0, 1.3846153846, 3.2307692308);
uniform float weight[5] = float[] (0.2270270270, 0.1945946, 0.1216216, 0.054054, 0.016216);


#define GAUSSIAN
//#define SOBEL

#ifdef SOBEL
float rgb_2_luma(vec3 c){ return .3*c[0] + .59*c[1] + .11*c[2]; }
#endif

void main() {
#ifdef GAUSSIAN
//    vec2 texel = uv;
//    vec3 bloom = vec3(0, 0, 0);
//    int count = 0;
//    for(int i = -4; i < 4; i++)
//    {
//        for(int j = -3; j < 3; j++)
//        {
//            bloom += texture(tex, texel + vec2(i, j) * 0.004).rgb * 0.25;
//        }
//    }
//    if(texture(tex, texel).r < 0.3)
//    {
//        color = bloom * bloom * 0.012 + texture(tex, texel).rgb;
//    } else
//    {
//        if(texture(tex, uv).r < 0.5)
//        {
//            color = bloom * bloom *0.009 + texture(tex, texel).rgb;
//        } else
//        {
//            color = bloom * bloom * 0.0075 + texture(tex, texel).rgb;
//        }
//    }

    ///--- Gaussian convolution
    float std = 3; ///< standard deviation (<.3 disable)
    // float std = .1; ///< standard deviation (<.3 disable)
    vec3 color_tot = vec3(0,0,0);
    float weight_tot = 0;
    int SIZE = 1 + 2 * 3 * int( ceil(std) );
    for(int i=-SIZE; i<=SIZE; i++){
        for(int j=-SIZE; j<=SIZE; j++){
            float w = exp(-(i*i+j*j)/(2.0*std*std*std*std));
            vec3 neigh_color = texture(tex, uv+vec2(i/tex_width,j/tex_height)).rgb;
            color_tot += w * neigh_color;
            weight_tot += w;
        }
    }
    color = color_tot / weight_tot; ///< ensure \int w = 1
#endif

#ifdef SOBEL
    float t_00 = rgb_2_luma( textureOffset(tex, uv, ivec2(-1, -1)).rgb );
    float t_01 = rgb_2_luma( textureOffset(tex, uv, ivec2(-1,  0)).rgb );
    float t_02 = rgb_2_luma( textureOffset(tex, uv, ivec2(-1, +1)).rgb );
    ///--- x=0
    float t_10 = rgb_2_luma( textureOffset(tex, uv, ivec2( 0, -1)).rgb );
    float t_12 = rgb_2_luma( textureOffset(tex, uv, ivec2( 0, +1)).rgb );
    ///--- x=+1
    float t_20 = rgb_2_luma( textureOffset(tex, uv, ivec2(+1, -1)).rgb );
    float t_21 = rgb_2_luma( textureOffset(tex, uv, ivec2(+1,  0)).rgb );
    float t_22 = rgb_2_luma( textureOffset(tex, uv, ivec2(+1, +1)).rgb );

    float Gx = t_20 + t_21*2 + t_22 - t_00 - t_01*2 - t_02;
    float Gy = t_02 + t_12 * 2 + t_22 - t_00 - t_10*2 - t_20;
    float G = sqrt(Gx * Gx + Gy * Gy);
    color = vec3(G, G, G);
#endif
// No processing: using this line
//color = texture(tex,uv).rgb;
}

