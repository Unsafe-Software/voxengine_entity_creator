#pragma once

#include <glad/glad.h>

#include "VBO.hh"

namespace Engine {
    class VAO {
       public:
        GLuint id;
        VAO();
        ~VAO();

        void LinkAttrib(Engine::VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
        void Bind();
        void Unbind();
        void Delete();
    };
}  // namespace Engine
