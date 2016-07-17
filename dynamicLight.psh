#ifdef GL_ES
    precision mediump float;
#endif

varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
uniform vec2 u_resolution;
//========================================
// Light source define
//========================================
const int MAX_LIGHTS = 15;
struct LightSource
{
    vec2 lightPos;
    vec4 lightCol;
    float range;
    float luminance;
};
uniform LightSource ccl_u_lights[MAX_LIGHTS];
uniform int u_lightCount;

const int MAX_POLYDATA = 300;
const int MAX_SHADOW_CASTER = 50;
const int TYPE_POLYGON = 1;
const int TYPE_CIRCLE = 2;
const int TYPE_BOX = 3;

struct PolyData
{
    vec2 p;
};
uniform PolyData ccl_u_polyData[MAX_POLYDATA];
struct ShadowData
{
    int type;
    int polyCount;
    int pos;
};
uniform ShadowData ccl_u_shadowData[MAX_SHADOW_CASTER];
uniform int u_shadowDataCount;
//========================================
// override shadertoy variable
//========================================
vec2   iResolution = u_resolution;           // viewport resolution (in pixels)
float  iGlobalTime = CC_Time.x;              // shader playback time (in seconds)
vec4   iMouse = vec4(0,0,0,0);               // mouse pixel coords. xy: current (if MLB down), zw: click




//////////////////////////////////////
// Combine distance field functions //
//////////////////////////////////////


float smoothMerge(float d1, float d2, float k)
{
    float h = clamp(0.5 + 0.5*(d2 - d1)/k, 0.0, 1.0);
    return mix(d2, d1, h) - k * h * (1.0-h);
}


float merge(float d1, float d2)
{
    return min(d1, d2);
}


float mergeExclude(float d1, float d2)
{
    float a = min(d1 * sign(d2), d2 * sign(d1));
    float b = max(d1 * sign(d2), d2 * sign(d1));
    
    if (sign(d1) == sign(d2))
        return a;
    else
        return b;
}


float substract(float d1, float d2)
{
    return max(-d1, d2);
}


float intersect(float d1, float d2)
{
    return max(d1, d2);
}


//////////////////////////////
// Rotation and translation //
//////////////////////////////


vec2 rotateCCW(vec2 p, float a)
{
    mat2 m = mat2(cos(a), sin(a), -sin(a), cos(a));
    return p * m;   
}


vec2 rotateCW(vec2 p, float a)
{
    mat2 m = mat2(cos(a), -sin(a), sin(a), cos(a));
    return p * m;
}


vec2 translate(vec2 p, vec2 t)
{
    return p - t;
}


//////////////////////////////
// Distance field functions //
//////////////////////////////


float pie(vec2 p, float angle)
{
    angle = radians(angle) / 2.0;
    vec2 n = vec2(cos(angle), sin(angle));
    return abs(p).x * n.x + p.y*n.y;
}


float circleDist(vec2 p, float radius)
{
    return length(p) - radius;
}


float triangleDist(vec2 p, float radius)
{
    return max( abs(p).x * 0.866025 + 
                p.y * 0.5, -p.y) 
                -radius * 0.5;
}


float triangleDist(vec2 p, float width, float height)
{
    vec2 n = normalize(vec2(height, width / 2.0));
    return max( abs(p).x*n.x + p.y*n.y - (height*n.y), -p.y);
}


float semiCircleDist(vec2 p, float radius, float angle, float width)
{
    width /= 2.0;
    radius -= width;
    return substract(pie(p, angle), 
                     abs(circleDist(p, radius)) - width);
}


float boxDist(vec2 p, vec2 size, float radius)
{
    size -= vec2(radius);
    vec2 d = abs(p) - size;
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - radius;
}


float lineDist(vec2 p, vec2 start, vec2 end, float width)
{
    vec2 dir = start - end;
    float lngth = length(dir);
    dir /= lngth;
    vec2 proj = max(0.0, min(lngth, dot((start - p), dir))) * dir;
    return length( (start - p) - proj ) - (width / 2.0);
}


///////////////////////
// Masks for drawing //
///////////////////////


float fillMask(float dist)
{
    return clamp(-dist, 0.0, 1.0);
}


float innerBorderMask(float dist, float width)
{
    //dist += 1.0;
    float alpha1 = clamp(dist + width, 0.0, 1.0);
    float alpha2 = clamp(dist, 0.0, 1.0);
    return alpha1 - alpha2;
}


float outerBorderMask(float dist, float width)
{
    //dist += 1.0;
    float alpha1 = clamp(dist, 0.0, 1.0);
    float alpha2 = clamp(dist - width, 0.0, 1.0);
    return alpha1 - alpha2;
}


///////////////
// The scene //
///////////////


float sceneDist(vec2 p)
{
    float finalDist = 0.0;
    bool isSetted = false;
    for(int i = 0; i < u_shadowDataCount; ++i)
    {

        // DRAW POLYGON
        if(ccl_u_shadowData[i].type == TYPE_POLYGON)
        {
            for(int j = ccl_u_shadowData[i].pos; j < ccl_u_shadowData[i].pos + ccl_u_shadowData[i].polyCount; ++j)
            {
                int next = j + 1;
                if(next == ccl_u_shadowData[i].pos + ccl_u_shadowData[i].polyCount) next = 0;

                float d = lineDist(p,ccl_u_polyData[j].p,  ccl_u_polyData[next].p, 4.0);
                if(!isSetted)
                {
                    finalDist = d;
                    isSetted = true;
                }else{
                    finalDist = merge(finalDist, d);
                }
            }
        }else if (ccl_u_shadowData[i].type == TYPE_CIRCLE)
        {
            float d = circleDist(translate(p, ccl_u_polyData[ccl_u_shadowData[i].pos].p), ccl_u_polyData[ccl_u_shadowData[i].pos + 1].p.x);
            if(!isSetted)
            {
                finalDist = d;
                isSetted = true;
            }else{
                finalDist = merge(finalDist, d);
            }
        }else if (ccl_u_shadowData[i].type == TYPE_BOX)
        {
            float d = boxDist(translate(p, ccl_u_polyData[ccl_u_shadowData[i].pos].p), ccl_u_polyData[ccl_u_shadowData[i].pos + 1].p, ccl_u_polyData[ccl_u_shadowData[i].pos + 2].p.x);
            if(!isSetted)
            {
                finalDist = d;
                isSetted = true;
            }else{
                finalDist = merge(finalDist, d);
            }
        }
        
    }

    return finalDist;
}


float sceneSmooth(vec2 p, float r)
{
    float accum = sceneDist(p);
    accum += sceneDist(p + vec2(0.0, r));
    accum += sceneDist(p + vec2(0.0, -r));
    accum += sceneDist(p + vec2(r, 0.0));
    accum += sceneDist(p + vec2(-r, 0.0));
    return accum / 5.0;
}


//////////////////////
// Shadow and light //
//////////////////////


float shadow(vec2 p, vec2 pos, float radius)
{
    vec2 dir = normalize(pos - p);
    float dl = length(p - pos);
    
    // fraction of light visible, starts at one radius (second half added in the end);
    float lf = radius * dl;
    
    // distance traveled
    float dt = 0.01;

    for (int i = 0; i < 64; ++i)
    {               
        // distance to scene at current position
        float sd = sceneDist(p + dir * dt);

        // early out when this ray is guaranteed to be full shadow
        if (sd < -radius) 
            return 0.0;
        
        // width of cone-overlap at light
        // 0 in center, so 50% overlap: add one radius outside of loop to get total coverage
        // should be '(sd / dt) * dl', but '*dl' outside of loop
        lf = min(lf, sd / dt);
        
        // move ahead
        dt += max(1.0, abs(sd));
        if (dt > dl) break;
    }

    // multiply by dl to get the real projected overlap (moved out of loop)
    // add one radius, before between -radius and + radius
    // normalize to 1 ( / 2*radius)
    lf = clamp((lf*dl + radius) / (2.0 * radius), 0.0, 1.0);
    lf = smoothstep(0.0, 1.0, lf);
    return lf;
}



vec4 drawLight(vec2 p, vec2 pos, vec4 color, float dist, float range, float radius)
{
    // distance to light
    float ld = length(p - pos);
    
    // out of range
    if (ld > range) return vec4(0.0);
    
    // shadow and falloff
    float shad = shadow(p, pos, radius);
    float fall = (range - ld)/range;
    fall *= fall;

    // do not draw source
    //float source = fillMask(circleDist(p - pos, radius));
    return vec4((shad * fall) * color.rgb, color.a / 2);
}


float luminance(vec4 col)
{
    return 0.2126 * col.r + 0.7152 * col.g + 0.0722 * col.b;
}


void setLuminance(inout vec4 col, float lum)
{
    lum /= luminance(col);
    col *= lum;
}


float AO(vec2 p, float dist, float radius, float intensity)
{
    float a = clamp(dist / radius, 0.0, 1.0) - 1.0;
    return 1.0 - (pow(abs(a), 5.0) + 1.0) * intensity + (1.0 - intensity);
    return smoothstep(0.0, 1.0, dist / radius);
}


//=============================================================================
// Main
//=============================================================================
void main(void)
{
    
    vec2 p = gl_FragCoord.xy + vec2(0.5);
    vec2 c = u_resolution.xy / 2.0;

    float dist = sceneDist(p);

    // gradient
    vec4 col = vec4(0, 0, 0, 1.0);
    // grid
    //col *= clamp(min(mod(p.y, 10.0), mod(p.x, 10.0)), 0.9, 1.0);
    // ambient occlusion
    col *= AO(p, sceneSmooth(p, 10.0), 40.0, 0.4);

    // create input lights source
    for(int i = 0; i < u_lightCount; i++)
    {
        setLuminance(ccl_u_lights[i].lightCol, ccl_u_lights[i].luminance);
        vec4 light = drawLight(p, ccl_u_lights[i].lightPos, ccl_u_lights[i].lightCol, dist, ccl_u_lights[i].range, 2.0);
        col.rgb += light.rgb;
        // need modify alpha here
    }
    // shape fill
    //col = mix(col, vec4(1.0, 0.4, 0.0, 1.0), fillMask(dist));
    // shape outline
    col = mix(col, vec4(0.8, 0.8, 0.8, 1.0), innerBorderMask(dist, 1.5));

    //========================
    gl_FragColor = v_fragmentColor * clamp(col, 0.0, 0.8);
}