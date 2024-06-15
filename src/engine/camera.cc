#include "camera.hh"

namespace Engine {
    Camera::Camera(int width, int height, glm::vec3 position) {
        Camera::width = width;
        Camera::height = height;
        Position = position;
    }

    void Camera::UpdateMatrix(float FOVdeg, float nearPlane, float farPlane) {
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        view = glm::lookAt(Position, Position + Orientation, Up);
        projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);
        cameraMatrix = projection * view;
        this->FOVdeg = FOVdeg;
        this->nearPlane = nearPlane;
        this->farPlane = farPlane;
    }

    void Camera::UpdateAspect(int width, int height) {
        Camera::width = width;
        Camera::height = height;
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);
        cameraMatrix = projection * glm::lookAt(Position, Position + Orientation, Up);
    }

    void Camera::Matrix(Engine::Shader& shader, const char* uniform) {
        glUniformMatrix4fv(glGetUniformLocation(shader.id, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
    }

    void Camera::SetOrientation(glm::vec3 orientation) { Orientation = glm::normalize(orientation); }

    void Camera::Inputs(GLFWwindow* window, bool recordMouse, bool recordKeys) {
        float speed = 2.0f;
        float sensitivity = 350.0f;
        if (recordKeys) {
            // Horizontal movement
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                Position += speed * Orientation;
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                Position += speed * -glm::normalize(glm::cross(Orientation, Up));
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                Position += speed * -Orientation;
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                Position += speed * glm::normalize(glm::cross(Orientation, Up));
            }
            // Vertical movement
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                Position += speed * Up;
            }
            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                Position += speed * -Up;
            }
            // Rotation
            if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
                Orientation = glm::rotate(Orientation, glm::radians(sensitivity / width * 2), Up);
            } else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
                Orientation = glm::rotate(Orientation, glm::radians(-sensitivity / width * 2), Up);
            }
            if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
                glm::vec3 right = glm::normalize(glm::cross(Orientation, Up));
                Orientation = glm::rotate(Orientation, glm::radians(sensitivity / height * 2), right);
            } else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
                glm::vec3 right = glm::normalize(glm::cross(Orientation, Up));
                Orientation = glm::rotate(Orientation, glm::radians(-sensitivity / height * 2), right);
            }
        }

        if (recordMouse) {
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

                if (firstClick) {
                    glfwSetCursorPos(window, (width / 2), (height / 2));
                    firstClick = false;
                }

                double mouseX;
                double mouseY;
                glfwGetCursorPos(window, &mouseX, &mouseY);
                float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
                float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;
                glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

                if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f)) {
                    Orientation = newOrientation;
                }

                Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);
                glfwSetCursorPos(window, (width / 2), (height / 2));
            } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                firstClick = true;
            }
        }
    }
}  // namespace Engine
