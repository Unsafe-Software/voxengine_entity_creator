#pragma once

#include <glad/glad.h>

namespace Engine {
    class EBO {
       public:
        GLuint id;
        EBO();
        EBO(GLuint* indices, GLsizeiptr size);
        ~EBO();

        void Bind();
        void Unbind();
        void Update(GLuint* indices, GLsizeiptr size);
        void Delete();
    };
}  // namespace Engine
