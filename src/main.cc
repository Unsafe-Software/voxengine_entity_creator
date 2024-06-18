// clang-format off
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// clang-format on
#include <yaml-cpp/yaml.h>

#include <chrono>
#include <fstream>

#include "cmake_defines.hh"
#include "engine/EBO.hh"
#include "engine/VAO.hh"
#include "engine/VBO.hh"
#include "engine/camera.hh"
#include "engine/line.hh"
#include "engine/shader.hh"
#include "entity.hh"
#include "imfilebrowser.h"
#include "utils/logger.hh"

int screen_width = 800;
int screen_height = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); }

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
}

GLfloat lightVertices[] = {
    -0.1f, -0.1f, 0.1f, -0.1f, -0.1f, -0.1f, 0.1f, -0.1f, -0.1f, 0.1f, -0.1f, 0.1f, -0.1f, 0.1f, 0.1f, -0.1f, 0.1f, -0.1f, 0.1f, 0.1f, -0.1f, 0.1f, 0.1f, 0.1f};

GLuint lightIndices[] = {0, 1, 2, 0, 2, 3, 0, 4, 7, 0, 7, 3, 3, 7, 6, 3, 6, 2, 2, 6, 5, 2, 5, 1, 1, 5, 4, 1, 4, 0, 4, 5, 6, 4, 6, 7};

std::string getFileNameWithoutExtension(const std::string& filePath) {
    size_t lastSlashPos = filePath.find_last_of("/\\");
    size_t lastDotPos = filePath.find_last_of('.');

    if (lastDotPos == std::string::npos || (lastSlashPos != std::string::npos && lastDotPos < lastSlashPos)) {
        lastDotPos = std::string::npos;
    }

    if (lastSlashPos == std::string::npos) {
        lastSlashPos = 0;
    } else {
        lastSlashPos += 1;
    }

    std::string fileNameWithoutExtension = filePath.substr(lastSlashPos, lastDotPos - lastSlashPos);

    return fileNameWithoutExtension;
}

int main() {
    Utils::Logger logger;
    int logger_window_size = 150;
    logger.SetLoggerWindowSize(logger_window_size);
    logger.Info(PROJECT_NAME ": " PROJECT_VERSION);

    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    logger.Info("GLFW initialized successfully");

    // Create a window
    GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "VoxEngine Entity Creator", NULL, NULL);
    if (window == NULL) {
        logger.Fatal("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSwapInterval(1);
    logger.Info("Window created successfully");

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        logger.Fatal("Failed to initialize GLAD");
        return -1;
    }
    logger.Info("GLAD initialized successfully");

    Engine::Shader lightShader(logger, "data/shaders/light.vert", "data/shaders/light.frag");
    Engine::VAO lightVAO;
    lightVAO.Bind();
    Engine::VBO lightVBO(lightVertices, sizeof(lightVertices));
    Engine::EBO lightEBO(lightIndices, sizeof(lightIndices));
    lightVAO.LinkAttrib(lightVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
    lightVAO.Unbind();
    lightVBO.Unbind();
    lightEBO.Unbind();
    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos = glm::vec3(16.0f, 8.0f, 32.0f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPos);
    lightShader.Activate();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.id, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
    glUniform4f(glGetUniformLocation(lightShader.id, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    logger.Info("Light shader and buffers initialized successfully");

    // Create camera object
    Engine::Camera camera(screen_width, screen_height, glm::vec3(32.0f, 16.0f, 32.0f));
    camera.SetOrientation(glm::vec3(-0.7f, -0.3f, -0.7f));
    logger.Info("Camera initialized successfully");

    // IMGUI setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::StyleColorsDark();
    logger.Info("ImGui initialized successfully");

    // Entity shader
    Engine::Shader entityShader(logger, "data/shaders/entity.vert", "data/shaders/entity.frag");
    entityShader.Activate();
    logger.Info("Entity shader initialized successfully");

    // Create entities
    Entity::EntityBase entity(logger);
    entity.SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    bool entity_initialized = false;

    // Position offset cube
    Engine::VAO cube_VAO;
    cube_VAO.Bind();
    GLfloat cube_vertices[] = {
        -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0, 1.0, -1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, -1.0};
    for (int i = 0; i < 24; i++) {
        cube_vertices[i] *= 0.1f;
    }
    GLuint cube_indices[] = {0, 1, 2, 2, 3, 0, 1, 5, 6, 6, 2, 1, 7, 6, 5, 5, 4, 7, 4, 0, 3, 3, 7, 4, 4, 5, 1, 1, 0, 4, 3, 2, 6, 6, 7, 3};
    Engine::VBO cube_VBO(cube_vertices, sizeof(cube_vertices));
    Engine::EBO cube_EBO(cube_indices, sizeof(cube_indices));
    cube_VAO.LinkAttrib(cube_VBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
    cube_VAO.Unbind();
    Engine::Shader cube_shader(logger, "data/shaders/zero.vert", "data/shaders/zero.frag");
    cube_shader.Activate();
    logger.Info("Zero cube initialized successfully");

    ImGui::FileBrowser openFileDialog;
    openFileDialog.SetTitle("Select a voxel model file");
    openFileDialog.SetTypeFilters({".vox"});

    glEnable(GL_DEPTH_TEST);

    static char entity_name[128] = "";

    // Main loop
    logger.Info("Entering main loop");
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // Clear the screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGuiIO& io = ImGui::GetIO();
        camera.Inputs(window, !io.WantCaptureMouse, !io.WantCaptureKeyboard);
        camera.UpdateMatrix(45, 0.01f, 250);
        screen_width = io.DisplaySize.x;
        screen_height = io.DisplaySize.y;
        glViewport(0, 0, screen_width * 2, screen_height * 2);
        camera.UpdateAspect(screen_width, screen_height);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render the light cube
        lightShader.Activate();
        camera.Matrix(lightShader, "camMatrix");
        lightVAO.Bind();
        glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(uint), GL_UNSIGNED_INT, 0);

        // Render Entities
        if (entity_initialized) {
            entityShader.Activate();
            camera.Matrix(entityShader, "camMatrix");
            glUniform3f(glGetUniformLocation(entityShader.id, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
            glUniform4f(glGetUniformLocation(entityShader.id, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
            glUniform3f(glGetUniformLocation(entityShader.id, "lightPos"), -lightPos.x, lightPos.y, -lightPos.z);
            glm::mat4 entityModel = entity.GetModel();
            glUniformMatrix4fv(glGetUniformLocation(entityShader.id, "model"), 1, GL_FALSE, glm::value_ptr(entityModel));
            entity.Render();
        }

        // Render zero cube
        cube_shader.Activate();
        cube_VAO.Bind();
        camera.Matrix(cube_shader, "camMatrix");
        glUniform3f(glGetUniformLocation(cube_shader.id, "pos"), 0.0f, 0.0f, 0.0f);
        glDrawElements(GL_TRIANGLES, sizeof(cube_indices) / sizeof(int), GL_UNSIGNED_INT, 0);

        // ImGUI
        logger.Render();
        ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w = 0.25f;
        ImGui::GetStyle().Colors[ImGuiCol_TitleBg].w = 0.25f;
        ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive].w = 0.25f;
        ImGui::SetNextWindowPos(ImVec2(1, 30));
        ImGui::Begin("Entity");
        if (entity_initialized) {
            ImGui::Text("Camera");
            ImGui::Text("Position: %.2f %.2f %.2f", camera.Position.x, camera.Position.y, camera.Position.z);
            ImGui::Text("Orientation: %.2f %.2f %.2f", camera.Orientation.x, camera.Orientation.y, camera.Orientation.z);
            ImGui::Separator();

            ImGui::Text("Entity settings");

            if (ImGui::InputTextWithHint(" ", "Entity internal name", entity_name, IM_ARRAYSIZE(entity_name))) {
                entity.name = entity_name;
                entity_name[0] = '\0';
            }

            float pos_offset[3] = {entity.position_offset.x, entity.position_offset.y, entity.position_offset.z};
            int biggest_model_size = fmax(entity.model_size.x, fmax(entity.model_size.y, entity.model_size.z));
            ImGui::SliderFloat3("Position offset", pos_offset, -biggest_model_size, biggest_model_size, "%.2f", 1.0f);
            entity.position_offset = glm::vec3(pos_offset[0], pos_offset[1], pos_offset[2]);

            int rot[3] = {static_cast<int>(entity.rotation.x), static_cast<int>(entity.rotation.y), static_cast<int>(entity.rotation.z)};
            ImGui::SliderInt3("Rotation", rot, 0, 3);
            entity.rotation = glm::vec3(rot[0], rot[1], rot[2]);
            ImGui::Separator();

            if (ImGui::Button("Save entity properties")) {
                entity.Save();
            }
        }

        if (ImGui::Button("Open file")) openFileDialog.Open();
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(1, 1));
        ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w = 0.0f;
        ImGui::GetStyle().Colors[ImGuiCol_Border].w = 0.0f;
        ImGui::Begin("Info", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoInputs);
        ImGui::Text(PROJECT_NAME ": " PROJECT_VERSION);
        ImGui::End();
        ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w = 1.0f;
        ImGui::GetStyle().Colors[ImGuiCol_Border].w = 1.0f;
        ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w = 1.0f;
        ImGui::GetStyle().Colors[ImGuiCol_TitleBg].w = 1.0f;
        ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive].w = 1.0f;

        openFileDialog.Display();
        if (openFileDialog.HasSelected()) {
            logger.Info(std::format("Loading file: {}", openFileDialog.GetSelected().string()));
            entity.LoadModelForSetup(openFileDialog.GetSelected().string());
            entity_initialized = true;
            std::string entity_name_str = getFileNameWithoutExtension(openFileDialog.GetSelected().string());
            std::copy(entity_name_str.begin(), entity_name_str.end(), entity_name);
            openFileDialog.ClearSelected();
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    lightVAO.Delete();
    lightVBO.Delete();
    lightEBO.Delete();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}
