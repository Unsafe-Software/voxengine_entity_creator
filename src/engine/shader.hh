#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <cerrno>
#include <fstream>
#include <iostream>
#include <sstream>

#include "../utils/logger.hh"

namespace Engine {
    std::string readFileContent(const char* filename);

    class Shader {
       private:
        Utils::Logger* logger;

       public:
        GLuint id;

        Shader(Utils::Logger& logger, const char* vertexFile, const char* fragmentFile);
        ~Shader();

        void Activate();
    };
}  // namespace Engine
