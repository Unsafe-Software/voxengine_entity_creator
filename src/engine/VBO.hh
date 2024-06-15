#pragma once

#include <glad/glad.h>

namespace Engine {
    class VBO {
       public:
        GLuint id;
        VBO();
        VBO(GLfloat* vertices, GLsizeiptr size);
        ~VBO();

        void Bind();
        void Unbind();
        void Update(GLfloat* vertices, GLsizeiptr size);
        void Delete();
    };
}  // namespace Engine
