#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
// clang-format on

#include "shader.hh"

namespace Engine {
    class Camera {
       public:
        glm::vec3 Position;
        glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::mat4 cameraMatrix = glm::mat4(1.0f);
        float nearPlane;
        float farPlane;
        float FOVdeg;
        bool firstClick = true;
        int width;
        int height;

        Camera(int width, int height, glm::vec3 position);

        void UpdateMatrix(float FOVdeg, float nearPlane, float farPlane);
        void UpdateAspect(int width, int height);
        void Matrix(Engine::Shader& shader, const char* uniform);
        void SetOrientation(glm::vec3 orientation);
        void Inputs(GLFWwindow* window, bool recordMouse = true, bool recordKeys = true);
    };
}  // namespace Engine
