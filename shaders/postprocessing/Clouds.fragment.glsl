#version 430 core

#include PostProcessEffectBase.glsl

#define CLOUD_SAMPLES 2
#define CLOUDCOVERAGE_DENSITY 90
#include Atmosphere.glsl

uniform int RenderPass;

vec4 shade(){    
    return vec3(0,0,0,0);
   // if(shouldBreak()) return vec4(0);
    vec3 dir = normalize(reconstructCameraSpaceDistance(UV, 1.0));
    vec4 retedg = vec4(0);
    vec4 retavg = vec4(0);
    
    if(RenderPass == 0){
        vec2 lastData = texture(cloudsCloudsTex, dir).rg;
        vec2 val = raymarchCloudsRay();
        retedg.rg = vec2(max(val.r, lastData.r), min(val.g, lastData.g));
        retavg.rg = vec2(mix(val.r, lastData.r, CloudsIntegrate), val.g);
    } else if(RenderPass == 1){
        float lastData = texture(cloudsCloudsTex, dir).r;
        float val = shadows();
        retedg.r = min(val, lastData);
        retavg.r = mix(val, lastData, CloudsIntegrate);
    } else {
        float lastData = texture(cloudsCloudsTex, dir).r;
        float val = skyfog();
        retedg.r = min(val, lastData);
        retavg.r = mix(val, lastData, CloudsIntegrate);
    }

    return mix(retavg, retedg, CloudsIntegrate * 0.8);
}