#version 140
in vec3 vNormal;
in vec2 vUV;
in vec3 vEyePos;

uniform vec4 ambientLight;
uniform vec4 color;
uniform sampler2D tex;

uniform vec4 lightPosType[4];
uniform vec4 lightColorRange[4];
uniform float specularity;

out vec4 fragColor;

vec3 computeLight(){
    vec3 lightColor = ambientLight.xyz;
    vec3 normal = normalize(vNormal);

    float diffuseFrac = 1.0 - ambientLight.w;

    for (int i=0;i<4;i++){
        bool isDirectional = lightPosType[i].w == 0.0;
        bool isPoint       = lightPosType[i].w == 1.0;
        vec3 lightDirection;
        float att = 1.0;
        if (isDirectional){
            lightDirection = normalize(lightPosType[i].xyz);
        } else if (isPoint) {
            vec3 lightVector = lightPosType[i].xyz - vEyePos;
            float lightVectorLength = length(lightVector);
            float lightRange = lightColorRange[i].w;
            lightDirection = lightVector / lightVectorLength;
            if (lightRange <= 0.0){
                att = 1.0;
            } else if (lightVectorLength >= lightRange){
                att = 0.0;
            } else {
                att = pow(1.0 - lightVectorLength / lightRange,1.5); // non physical range based attenuation
            }
        } else {
            continue;
        }

        // diffuse light
        float thisDiffuse = max(0.0,dot(lightDirection, normal));
        if (thisDiffuse > 0.0){
           lightColor += (att * diffuseFrac * thisDiffuse) * lightColorRange[i].xyz;
        }

        // specular light
        if (specularity > 0.0){
            vec3 H = normalize(lightDirection - normalize(vEyePos));
            float nDotHV = dot(normal, H);
            if (nDotHV > 0.0){
                float pf = pow(nDotHV, specularity);
                lightColor += vec3(att * diffuseFrac * pf); // white specular highlights
            }
        }
    }

    return lightColor;
}

void main(void)
{
    vec4 c = color;


    fragColor = vec4(1,1,1,1);
}
