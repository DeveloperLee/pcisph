#version 430
uniform vec4 color;
in vec2 gPosition;
uniform float particleRadius;
layout (location = 0) out vec4 position;
flat in int vert_num;
void main() {
    vec3 n = vec3(2.0 * gPosition, 0.0);
    float r2 = dot(n.xy, n.xy);
    if (r2 > 1.0) discard;
    n.z = sqrt(1.0 - r2);
    vec3 L = normalize(vec3(1.0));
    float d = max(0.0, dot(L, n));
    //out_color = vec4(d * color.xyz, color.w);
    gl_FragDepth=gl_FragCoord.z - normalize(n).z*particleRadius;
    position=vec4(vec2(float(vert_num) / 10000.),0,1);
}
