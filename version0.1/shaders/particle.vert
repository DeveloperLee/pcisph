#version 430
uniform mat4 mv;
in vec3 position;
out vec4 vPosition;
void main() {
    vPosition = mv * vec4(position, 1.0);
}
