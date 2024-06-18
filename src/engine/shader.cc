#include "shader.hh"

namespace Engine {
    std::string readFileContent(Utils::Logger* logger, const char* filename) {
        std::ifstream in(filename, std::ios::binary);
        if (in) {
            std::string contents;
            in.seekg(0, std::ios::end);
            contents.resize(in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(&contents[0], contents.size());
            in.close();
            return contents;
        }
        logger->Info(std::format("Failed to read shader file ~{}~", filename));
        return "";
    }

    Shader::Shader(Utils::Logger& logger, const char* vertexFile, const char* fragmentFile) {
        this->logger = &logger;
        // Create the vertex shader
        std::string vertexSource = readFileContent(this->logger, vertexFile);
        const char* vertexSourceC = vertexSource.c_str();
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexSourceC, NULL);
        glCompileShader(vertexShader);
        GLint success;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            this->logger->Error(std::format("Failed to compile vertex shader:\n{}", infoLog));
        }

        // Create the fragment shader
        std::string fragmentSource = readFileContent(this->logger, fragmentFile);
        const char* fragmentSourceC = fragmentSource.c_str();
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentSourceC, NULL);
        glCompileShader(fragmentShader);
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            this->logger->Error(std::format("Failed to compile fragment shader:\n{}", infoLog));
        }

        // Create the shader program
        GLuint sharedProgram = glCreateProgram();
        glAttachShader(sharedProgram, vertexShader);
        glAttachShader(sharedProgram, fragmentShader);
        glLinkProgram(sharedProgram);

        // Delete the vertex and fragmet shader
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        id = sharedProgram;
    }

    void Shader::Activate() { glUseProgram(id); }

    Shader::~Shader() { glDeleteProgram(id); }
}  // namespace Engine
