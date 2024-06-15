#include "VBO.hh"

namespace Engine {
    VBO::VBO() { glGenBuffers(1, &id); }

    VBO::VBO(GLfloat* vertices, GLsizeiptr size) {
        glGenBuffers(1, &id);
        glBindBuffer(GL_ARRAY_BUFFER, id);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW);
    }

    VBO::~VBO() {}

    void VBO::Bind() { glBindBuffer(GL_ARRAY_BUFFER, id); }

    void VBO::Unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }

    void VBO::Update(GLfloat* vertices, GLsizeiptr size) {
        Bind();
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, vertices);
    }

    void VBO::Delete() { glDeleteBuffers(1, &id); }
}  // namespace Engine
