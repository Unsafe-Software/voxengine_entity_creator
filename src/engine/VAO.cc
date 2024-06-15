#include "VAO.hh"

namespace Engine {
    VAO::VAO() { glGenVertexArrays(1, &id); }

    VAO::~VAO() {}

    void VAO::Bind() { glBindVertexArray(id); }

    void VAO::Unbind() { glBindVertexArray(0); }

    void VAO::LinkAttrib(Engine::VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset) {
        VBO.Bind();
        glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
        glEnableVertexAttribArray(layout);
        VBO.Unbind();
    }

    void VAO::Delete() { glDeleteVertexArrays(1, &id); }
}  // namespace Engine
