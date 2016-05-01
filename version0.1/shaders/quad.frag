#version 330
uniform sampler2D tex;
in vec2 uv;
out vec4 fragColor;
uniform float near;
uniform float far;
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
    if(0<1){
        //just color
        fragColor = texture(tex,uv);
        //scaled to see depth
        float n = near;
        float f = far;
        float z = texture(tex,uv).x;
        z= (2.0 * n) / (f + n - z * (f - n));
        //fragColor = vec4(z,z,z,1);

    }else{
        vec2 texelSize = 1.0 / textureSize(tex,0).xy;
        float n = near;
        float f = far;
        float z = texture(tex,uv).x;
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

        fragColor = vec4((water_color*.2)+(water_color*diffuse_amount*.6)+vec3(specular_amount),1);
    }
}
