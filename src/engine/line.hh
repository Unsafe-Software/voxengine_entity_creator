#pragma once

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

namespace Engine {
    class Line {
        GLfloat line[6];
        uint id;

       public:
        Line(glm::vec3 start, glm::vec3 end) {
            glGenBuffers(1, &this->id);
            this->line[0] = start.x;
            this->line[1] = start.y;
            this->line[2] = start.z;
            this->line[3] = end.x;
            this->line[4] = end.y;
            this->line[5] = end.z;
            this->Bind();
            glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), this->line, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
            this->Unbind();
        };

        void Bind() { glBindBuffer(GL_ARRAY_BUFFER, this->id); }

        void Unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }

        void Draw() {
            this->Bind();
            glDrawArrays(GL_LINES, 0, 3);
            this->Unbind();
        }
    };
}  // namespace Engine
