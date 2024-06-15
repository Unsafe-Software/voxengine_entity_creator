#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in float aNormal;

out vec3 color;
out vec3 Normal;
out vec3 crntPos;

uniform mat4 camMatrix;
uniform mat4 model;

void main() {
    vec3 normal = vec3(0.0, 0.0, 0.0);
    vec3 normalColor = vec3(0.0, 0.0, 0.0);
    if (aNormal == 1.0f) {
        normal = vec3(0.0, 1.0, 0.0);
        normalColor = vec3(1.0, 0.0, 0.0);
    } else if (aNormal == 2.0) {
        normal = vec3(0.0, -1.0, 0.0);
        normalColor = vec3(0.0, 1.0, 0.0);
    } else if (aNormal == 3.0) {
        normal = vec3(1.0, 0.0, 1.0);
        normalColor = vec3(0.0, 0.0, 1.0);
    } else if (aNormal == 4.0) {
        normal = vec3(-1.0, 0.0, 0.0);
        normalColor = vec3(1.0, 1.0, 0.0);
    } else if (aNormal == 5.0) {
        normal = vec3(0.0, 0.0, 1.0);
        normalColor = vec3(1.0, 0.0, 1.0);
    } else if (aNormal == 6.0) {
        normal = vec3(0.0, 0.0, -1.0);
        normalColor = vec3(0.0, 1.0, 1.0);
    }

    color = aColor;
    crntPos = vec3(model * vec4(aPos, 1.0f));
    gl_Position = camMatrix * vec4(crntPos, 1.0);
    Normal = normal;
}
