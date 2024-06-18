#version 330 core
layout(location = 0) in vec3 aPos;

out vec3 color;

uniform mat4 camMatrix;
uniform vec3 pos;

void main() {
    color = vec3(1.0f, 0.0f, 0.0f);
    gl_Position = camMatrix * vec4(aPos + pos, 1.0);
}
