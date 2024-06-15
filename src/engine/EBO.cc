#include "EBO.hh"

namespace Engine {
    EBO::EBO() { glGenBuffers(1, &id); }

    EBO::EBO(GLuint* indices, GLsizeiptr size) {
        glGenBuffers(1, &id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_DYNAMIC_DRAW);
    }

    EBO::~EBO() {}

    void EBO::Bind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id); }

    void EBO::Unbind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

    void EBO::Update(GLuint* indices, GLsizeiptr size) {
        Bind();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, indices);
    }

    void EBO::Delete() { glDeleteBuffers(1, &id); }
};  // namespace Engine
