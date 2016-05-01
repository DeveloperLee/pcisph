#version 430
uniform vec4 color;
in vec2 gPosition;
uniform float particleRadius;
layout (location = 0) out vec4 thickness;
void main() {
    vec3 n = vec3(2.0 * gPosition, 0.0);
    float r2 = dot(n.xy, n.xy);
    if (r2 > 1.0) discard;
    n.z = sqrt(1.0 - r2);
    vec3 L = normalize(vec3(1.0));
    float d = max(0.0, dot(L, n));
    n = normalize(n);
    thickness=vec4(vec3(n.z)*.1,1);
}
