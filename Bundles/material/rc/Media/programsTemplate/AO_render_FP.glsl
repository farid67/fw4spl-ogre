#version 330

uniform float u_vpWidth;
uniform float u_vpHeight;

uniform int scale; // here we take scale = 2
uniform sampler2D AO_map;
in vec4 pos;
const float gaussian[5] = float[] (0.153170, 0.144893, 0.122649, 0.092902, 0.062970);
const float epsilon = 0.0001;

out vec4 fragColour;

void main()
{
    // normalisation
    vec2 texCoord = gl_FragCoord.xy/vec2(u_vpWidth,u_vpHeight);

    fragColour.rgb = vec3(texture(AO_map,texCoord).r);

    /*

    vec4 tmp = texture(AO_map,texCoord);


    fragColour.gb = tmp.yz;

    vec2 key_values = tmp.yz; // the last two values in AO_map(r,g,b)
    // return a number on {0,1}
    float key = key_values.x * (256.0 / 257.0) + key_values.y * (1.0 / 257.0);

    float sum = tmp.r;

    float base_weight = gaussian[0];
    float total_weight = base_weight;

    sum *= total_weight;

    int r;
    for (r=-4; r<=4; ++r)
    {
        if (r!=0)
        {
            // horizontal = vec2(1,0)
            vec2 uv =  vec2(gl_FragCoord.xy) * (vec2(1,0) * scale);
            uv = uv/vec2(u_vpWidth,u_vpHeight);
            tmp = texture(AO_map,uv);
            float tapKey = tmp.y * (256.0 / 257.0) + tmp.z * (1.0 / 257.0);
            float value = tmp.x;

            // spatial domain -> gaussian tap
            float weight = 0.3 + gaussian[abs(r)];

            // bilateral weight
            weight *= max (0.0, 1.0 - 2000.0 * abs(tapKey - key));

            sum += value * weight;
            total_weight += weight;
        }
    }

    fragColour.rgb = vec3(sum/(total_weight+epsilon));

    // need the vertical part, here or in an other material

    //experimental
    /*
    sum = fragColour.r;

    total_weight = base_weight;
    sum*= total_weight;

    for (r=-5; r<=5; ++r)
    {
        if (r!=0)
        {
            // vertical = vec2(0,1)
            vec2 uv =  vec2(gl_FragCoord.xy) * (vec2(0,1) * scale);
            uv = uv/vec2(u_vpWidth,u_vpHeight);
            tmp = texture(AO_map,uv);
            float tapKey = tmp.y * (256.0 / 257.0) + tmp.z * (1.0 / 257.0);
            float value = tmp.x;

            // spatial domain -> gaussian tap
            float weight = 0.3 + gaussian[abs(r)];

            // bilateral weight
            weight *= max (0.0, 1.0 - 2000.0 * abs(tapKey - key));

            sum += value * weight;
            total_weight += weight;
        }
    }

    fragColour.rgb = vec3(sum/(total_weight+epsilon));

    */

}
