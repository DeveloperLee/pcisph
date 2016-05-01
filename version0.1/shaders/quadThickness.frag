#version 330
uniform sampler2D tex;
uniform sampler2D thickness;
in vec2 uv;
out vec4 fragColor;
uniform mat4 proj;
uniform mat4 view;

vec3 uvToEye(vec2 texCoord) {
    float depth = texture(tex,texCoord).x;
    vec2 transUV = (texCoord - vec2(.5))*2;
    vec4 sscoord = vec4(transUV,depth,1);
    mat4 invproj = inverse(proj);
    vec4 eyepos = invproj * sscoord;
    return eyepos.xyz/eyepos.w;
}
void main() {
    vec2 texelSize = 1.0 / textureSize(tex,0).xy;
    float z = texture(tex,uv).x;
    float thick = texture(thickness,uv).x;
    if(z==1){
        discard;
    }
    vec3 eyepos = uvToEye(uv);
    vec3 ddx = uvToEye(uv+vec2(texelSize.x,0)) - eyepos;
    vec3 ddx2 = eyepos - uvToEye(uv-vec2(texelSize.x,0));
    if(abs(ddx.z)>abs(ddx2.z)){
        ddx = ddx2;
    }

    vec3 ddy = uvToEye(uv+vec2(0,texelSize.y))-eyepos;
    vec3 ddy2 = eyepos - uvToEye(uv-vec2(0,texelSize.y));
    if(abs(ddy.z)>abs(ddy2.z)){
        ddy = ddy2;
    }


    vec3 norm = cross(ddx,ddy);
    fragColor = vec4((normalize(norm)+1.)/2.,1);

    vec4 light_pos_view_space = view * vec4(-2,2,-2,1);

    float diffuse_amount = dot(normalize(norm),normalize(light_pos_view_space.xyz-eyepos));

    vec3 normal = normalize(norm);
    vec3 lightdir = normalize(light_pos_view_space.xyz-eyepos);

    vec3 R = 2.0*(dot(normal,lightdir))*normal - lightdir;
    vec3 V = normalize(-light_pos_view_space.xyz);

    float specular_amount=pow( max(dot(R,V),0.0) ,20);

    vec3 water_color = vec3(.2,.4,.8);

//    vec4 particleColor = exp(-vec4(0.6f, 0.2f, 0.05f, 3.0f) * thickness*.5);
//    particleColor.w = clamp(1.0f - particleColor.w, 0.0f, 1.0f);

    vec3 ambient_color = water_color * .2;
    vec3 diffuse_color = (water_color*diffuse_amount*.6);
    vec3 specular_color = vec3(specular_amount);

    fragColor = vec4(ambient_color+specular_color,clamp(thick,0,1));


    //fresnel
    vec3 n = normal;
    vec3 eyeToVertex = normalize(eyepos);
    vec3 E = vec3(0,0,1);
    vec3 vertexToEye = -eyeToVertex;
    float r0 = .02;
    float F = r0 + (1.f-r0)*pow((1-dot(vertexToEye,n)),5);

    vec3 alpha = mix(water_color,vec3(0,0,0),exp(-thick));
    vec3 refracted = alpha*(1-F);

    vec3 beta = vec3(1,1,1);
    beta = mix(beta,vec3(0,0,0),exp(-thick));
    vec3 reflected = beta*F;

    vec3 l = normalize(light_pos_view_space.xyz - eyepos);
    vec3 H = normalize(l+vertexToEye);
    specular_color = vec3(1)* pow(clamp(dot(n,H),0,1),30);

    fragColor = vec4(refracted + reflected + specular_color ,1);


   //fragColor = particleColor;
    //fragColor = vec4(1,1,0,1);
    //fragColor = vec4(vec3(thick),1 );
}
