#pragma once

#include <GLFW/glfw3.h>
#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

#include "utils/logger.hh"

namespace Entity {
    class Reader {
       private:
        std::ifstream file;
        Utils::Logger* logger;

       public:
        int cursor;

        Reader(Utils::Logger* logger, std::string path) {
            this->logger = logger;
            this->file.open(path, std::ios::binary);
            if (!this->file) {
                this->logger->Fatal(std::format("Failed to open file: `{}` for reading", path));
            }
            this->cursor = 0;
        }

        std::string String(int length) {
            std::string str;
            str.resize(length);
            this->file.read(&str[0], length);
            this->cursor += length;
            return str;
        }

        int Int(int length) {
            int value;
            this->file.read(reinterpret_cast<char*>(&value), length);
            this->cursor += length;
            return value;
        }

        int UByte() {
            unsigned char value;
            this->file.read(reinterpret_cast<char*>(&value), 1);
            this->cursor += 1;
            return value;
        }

        ~Reader() { this->file.close(); }
    };

    class EntityBase {
       private:
        std::string data_path;
        YAML::Node data;
        Utils::Logger* logger;

        std::string model_path;
        glm::vec3 velocity;
        glm::vec3 acceleration;

        Engine::VAO VAO;
        Engine::VBO VBO;
        Engine::EBO EBO;

        int pallet[256][4];
        int voxel_amount;
        std::vector<glm::vec4> voxels;
        std::vector<std::vector<std::vector<int>>> blocks;
        std::vector<GLfloat> vertices;
        std::vector<GLuint> indices;
        int indices_size;

        int readIntOrZero(YAML::Node node) { return node.IsDefined() ? node.as<int>() : 0; }

       public:
        std::string name;
        glm::vec3 model_size;
        glm::vec3 position;
        glm::vec3 position_offset;
        glm::vec3 rotation;  // In 90 degree increments

        EntityBase(Utils::Logger& logger) : VAO(), VBO(), EBO() {
            this->logger = &logger;
            for (int i = 0; i < 256; i++) {
                this->pallet[i][0] = 0;
                this->pallet[i][1] = 0;
                this->pallet[i][2] = 0;
                this->pallet[i][3] = 0;
            }
            vertices = std::vector<GLfloat>();
            indices = std::vector<GLuint>();
            this->VAO.Bind();
            this->VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, 7 * sizeof(GLfloat), (void*)0);
            this->VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, 7 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
            this->VAO.LinkAttrib(VBO, 2, 1, GL_FLOAT, 7 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
            this->VAO.Unbind();
        };

        ~EntityBase(){};

        void Save() {
            // Save path is data_path but with .yml instead of .vox
            std::string save_path = this->model_path;
            std::string suffix = ".vox";
            if (save_path.size() >= suffix.size() && save_path.compare(save_path.size() - suffix.size(), suffix.size(), suffix) == 0) {
                // Replace ".vox" with ".yml"
                save_path.replace(save_path.size() - suffix.size(), suffix.size(), ".yml");
            }

            YAML::Node data;
            data["name"] = this->name;
            data["model"] = this->model_path;
            data["position_offset"]["x"] = std::ceil(this->position_offset.x * 100.0) / 100.0;
            data["position_offset"]["y"] = std::ceil(this->position_offset.y * 100.0) / 100.0;
            data["position_offset"]["z"] = std::ceil(this->position_offset.z * 100.0) / 100.0;
            data["rotation"]["x"] = this->rotation.x;
            data["rotation"]["y"] = this->rotation.y;
            data["rotation"]["z"] = this->rotation.z;
            std::ofstream file(save_path);
            file << data;
            file.close();
            logger->Info(std::format("Saved entity `{}` to `{}`", this->name, save_path));
        }

        void RenderMenu() {
            ImGui::Text("Entity: %s", this->name.c_str());
            ImGui::Text("Model: %s", this->model_path.c_str());
            ImGui::Text("Velocity: %f, %f, %f", this->velocity.x, this->velocity.y, this->velocity.z);
            ImGui::Text("Acceleration: %f, %f, %f", this->acceleration.x, this->acceleration.y, this->acceleration.z);
            ImGui::Text("Model Size: %f, %f, %f", this->model_size.x, this->model_size.y, this->model_size.z);
            ImGui::Text("Voxel Amount: %d", this->voxel_amount);
            ImGui::Separator();
            ImGui::Text("Position:");
            int x = this->position.x;
            int y = this->position.y;
            int z = this->position.z;
            ImGui::SliderInt("X", &x, -128.0f, 256.0f);
            ImGui::SliderInt("Y", &y, -128.0f, 256.0f);
            ImGui::SliderInt("Z", &z, -128.0f, 256.0f);
            this->position = glm::vec3(x, y, z);
            ImGui::Text("Rotation:");
            int rx = this->rotation.x;
            int ry = this->rotation.y;
            int rz = this->rotation.z;
            ImGui::SliderInt("RX", &rx, 0.0f, 3.0f);
            ImGui::SliderInt("RY", &ry, 0.0f, 3.0f);
            ImGui::SliderInt("RZ", &rz, 0.0f, 3.0f);
            this->rotation = glm::vec3(rx, ry, rz);
        };

        void SetPosition(glm::vec3 position) { this->position = position; };

        void SetPosition(int x, int y, int z) { this->position = glm::vec3(x, y, z); };

        glm::mat4 createRotationMatrix(float angleX, float angleY, float angleZ) {
            glm::mat4 rotationX = glm::mat4(1.0f);
            glm::mat4 rotationY = glm::mat4(1.0f);
            glm::mat4 rotationZ = glm::mat4(1.0f);

            if (angleX == 90.0f)
                rotationX = glm::rotate(rotationX, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            else if (angleX == 180.0f)
                rotationX = glm::rotate(rotationX, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            else if (angleX == 270.0f)
                rotationX = glm::rotate(rotationX, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));

            if (angleY == 90.0f)
                rotationY = glm::rotate(rotationY, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            else if (angleY == 180.0f)
                rotationY = glm::rotate(rotationY, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            else if (angleY == 270.0f)
                rotationY = glm::rotate(rotationY, glm::radians(270.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            if (angleZ == 90.0f)
                rotationZ = glm::rotate(rotationZ, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            else if (angleZ == 180.0f)
                rotationZ = glm::rotate(rotationZ, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            else if (angleZ == 270.0f)
                rotationZ = glm::rotate(rotationZ, glm::radians(270.0f), glm::vec3(0.0f, 0.0f, 1.0f));

            return rotationZ * rotationY * rotationX;  // Order matters
        }

        glm::vec3 rotatePosition(const glm::vec3& position, float angleX, float angleY, float angleZ) {
            glm::mat4 rotationMatrix = createRotationMatrix(angleX, angleY, angleZ);
            glm::vec4 rotatedPosition = rotationMatrix * glm::vec4(position, 1.0f);
            return glm::vec3(rotatedPosition);
        }

        glm::mat4 GetModel() {
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 rotationMatrix = createRotationMatrix(this->rotation.x * 90.0f, this->rotation.y * 90.0f, this->rotation.z * 90.0f);
            glm::vec3 pos_offset_rotated = rotatePosition(this->position_offset, this->rotation.x * 90.0f, this->rotation.y * 90.0f, this->rotation.z * 90.0f);
            model = glm::translate(model, this->position + pos_offset_rotated);
            model = model * rotationMatrix;
            return model;
        }

        // 1 = up, 2 = down, 3 = left, 4 = right, 5 = front, 6 = back
        int PushVertex(int x, int y, int z, float r, float g, float b, int n) {
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(r);
            vertices.push_back(g);
            vertices.push_back(b);
            vertices.push_back(n);

            return vertices.size() / 7 - 1;
        };

        // 1 = up, 2 = down, 3 = left, 4 = right, 5 = front, 6 = back
        void PushBlock(int rx, int ry, int rz, glm::vec3 color) {
            float r = color.x / 0xFF;
            float g = color.y / 0xFF;
            float b = color.z / 0xFF;
            int x = rx;
            int y = ry;
            int z = rz;
            int id_1, id_2, id_3, id_4;
            // Up side
            // Check if there is a block above
            if (!(ry < model_size.y - 1 && blocks[rx][ry + 1][rz] == 1)) {
                id_1 = PushVertex(x, y + 1, z, r, g, b, 1);
                id_2 = PushVertex(x + 1, y + 1, z, r, g, b, 1);
                id_3 = PushVertex(x + 1, y + 1, z - 1, r, g, b, 1);
                id_4 = PushVertex(x, y + 1, z - 1, r, g, b, 1);

                indices.push_back(id_1);
                indices.push_back(id_2);
                indices.push_back(id_3);
                indices.push_back(id_1);
                indices.push_back(id_3);
                indices.push_back(id_4);
            }

            // Down side
            if (!(ry > 0 && blocks[rx][ry - 1][rz] == 1)) {
                id_1 = PushVertex(x, y, z, r, g, b, 2);
                id_2 = PushVertex(x + 1, y, z, r, g, b, 2);
                id_3 = PushVertex(x + 1, y, z - 1, r, g, b, 2);
                id_4 = PushVertex(x, y, z - 1, r, g, b, 2);

                indices.push_back(id_1);
                indices.push_back(id_2);
                indices.push_back(id_3);
                indices.push_back(id_1);
                indices.push_back(id_3);
                indices.push_back(id_4);
            }

            // Right side
            if (!(rx < model_size.x - 1 && blocks[rx + 1][ry][rz] == 1)) {
                id_1 = PushVertex(x + 1, y, z, r, g, b, 4);
                id_2 = PushVertex(x + 1, y, z - 1, r, g, b, 4);
                id_3 = PushVertex(x + 1, y + 1, z - 1, r, g, b, 4);
                id_4 = PushVertex(x + 1, y + 1, z, r, g, b, 4);

                indices.push_back(id_1);
                indices.push_back(id_2);
                indices.push_back(id_3);
                indices.push_back(id_1);
                indices.push_back(id_3);
                indices.push_back(id_4);
            }

            // Left side
            if (!(rx > 0 && blocks[rx - 1][ry][rz] == 1)) {
                id_1 = PushVertex(x, y, z, r, g, b, 3);
                id_2 = PushVertex(x, y, z - 1, r, g, b, 3);
                id_3 = PushVertex(x, y + 1, z - 1, r, g, b, 3);
                id_4 = PushVertex(x, y + 1, z, r, g, b, 3);

                indices.push_back(id_1);
                indices.push_back(id_2);
                indices.push_back(id_3);
                indices.push_back(id_1);
                indices.push_back(id_3);
                indices.push_back(id_4);
            }

            // Back side
            if (!(rz < model_size.z - 1 && blocks[rx][ry][rz + 1] == 1)) {
                id_1 = PushVertex(x, y, z, r, g, b, 6);
                id_2 = PushVertex(x + 1, y, z, r, g, b, 6);
                id_3 = PushVertex(x + 1, y + 1, z, r, g, b, 6);
                id_4 = PushVertex(x, y + 1, z, r, g, b, 6);

                indices.push_back(id_1);
                indices.push_back(id_2);
                indices.push_back(id_3);
                indices.push_back(id_1);
                indices.push_back(id_3);
                indices.push_back(id_4);
            }

            // Front side
            if (!(rz > 0 && blocks[rx][ry][rz - 1] == 1)) {
                id_1 = PushVertex(x, y, z - 1, r, g, b, 5);
                id_2 = PushVertex(x + 1, y, z - 1, r, g, b, 5);
                id_3 = PushVertex(x + 1, y + 1, z - 1, r, g, b, 5);
                id_4 = PushVertex(x, y + 1, z - 1, r, g, b, 5);

                indices.push_back(id_1);
                indices.push_back(id_2);
                indices.push_back(id_3);
                indices.push_back(id_1);
                indices.push_back(id_3);
                indices.push_back(id_4);
            }
        };

        void LoadModelForSetup(std::string model_path) {
            this->model_path = model_path;
            this->name = "Undefined";
            this->position_offset = glm::vec3(0);
            this->rotation = glm::vec3(0);
            LoadModel();
            Triangulate();
        };

        glm::vec3 GetPosition() { return this->position; };

        void LoadModel() {
            Reader r(this->logger, this->model_path);
            std::string magic = r.String(4);
            if (magic != "VOX ") {
                this->logger->Fatal(std::format("`{}`: Invalid magic number: `{}` for VOX file", this->model_path, magic));
            }

            int version = r.Int(4);
            if (version != 200) {
                this->logger->Fatal(std::format("`{}`: Unsupported version: `{}`", this->model_path, version));
            }

            std::string main_chunk_name = r.String(4);
            if (main_chunk_name != "MAIN") {
                this->logger->Fatal(std::format("`{}`: Invalid main chunk name: `{}`", this->model_path, main_chunk_name));
            }
            int main_chunk_size = r.Int(4);
            int remaining_file = r.Int(4);
            if (main_chunk_size != 0) {
                this->logger->Fatal(std::format("`{}`: Incorrect main chunk size: `{}`", this->model_path, main_chunk_size));
            }

            while (r.cursor < remaining_file + 5 * 4) {
                std::string chunk_name = r.String(4);
                int chunk_size = r.Int(4);
                int child_size = r.Int(4);
                if (chunk_name == "SIZE") {
                    this->model_size = glm::vec3(r.Int(4), r.Int(4), r.Int(4));
                } else if (chunk_name == "PACK") {
                    int model_amount = r.Int(4);
                    if (model_amount != 1) {
                        logger->Warn(std::format("`{}`: Model amount is not 1: `{}`. Model loading may malfunction.", this->model_path, model_amount));
                    }
                } else if (chunk_name == "RGBA") {
                    if (chunk_size != 1024) {
                        logger->Fatal(std::format("`{}`: Invalid RGBA chunk size: `{}`", this->model_path, chunk_size));
                    }
                    for (int i = 0; i < 256; i++) {
                        this->pallet[i][0] = r.UByte();
                        this->pallet[i][1] = r.UByte();
                        this->pallet[i][2] = r.UByte();
                        this->pallet[i][3] = r.UByte();
                    }
                } else if (chunk_name == "XYZI") {
                    this->voxel_amount = r.Int(4);
                    for (int i = 0; i < voxel_amount; i++) {
                        glm::vec4 voxel = glm::vec4(r.UByte(), r.UByte(), r.UByte(), r.UByte());
                        this->voxels.push_back(voxel);
                    }
                } else {
                    this->logger->Warn(std::format("`{}`: Skipping unknown chunk: `{}` (`{}` + `{}`)", this->model_path, chunk_name, chunk_size, child_size));
                    r.cursor += chunk_size;
                }
            }

            blocks.resize(model_size.x);
            for (int x = 0; x < model_size.x; x++) {
                blocks[x].resize(model_size.y);
                for (int y = 0; y < model_size.y; y++) {
                    blocks[x][y].resize(model_size.z);
                }
            }

            for (int x = 0; x < model_size.x; x++) {
                for (int y = 0; y < model_size.y; y++) {
                    for (int z = 0; z < model_size.z; z++) {
                        blocks[x][y][z] = 0;
                    }
                }
            }

            for (int i = 0; i < voxels.size(); i++) {
                if (voxels[i].x >= model_size.x || voxels[i].y >= model_size.y || voxels[i].z >= model_size.z) {
                    logger->Warn(std::format("Voxel out of bounds: `{}`, `{}`, `{}`", voxels[i].x, voxels[i].y, voxels[i].z));
                    continue;
                }
                blocks[voxels[i].x][voxels[i].y][voxels[i].z] = voxels[i].w;
            }

            logger->Info(std::format("Loaded entity: `{}`", this->name));
        };

        void Triangulate() {
            vertices.clear();
            indices.clear();
            for (int x = 0; x < model_size.x; x++) {
                for (int y = 0; y < model_size.y; y++) {
                    for (int z = 0; z < model_size.z; z++) {
                        if (blocks[x][y][z] != 0) {
                            PushBlock(x, y, z, glm::vec3(pallet[blocks[x][y][z]][0], pallet[blocks[x][y][z]][1], pallet[blocks[x][y][z]][2]));
                        }
                    }
                }
            }

            GLfloat vertices_array[this->vertices.size()];
            for (size_t i = 0; i < this->vertices.size(); i++) {
                vertices_array[i] = this->vertices[i];
            }
            GLuint indices_array[this->indices.size()];
            for (size_t i = 0; i < this->indices.size(); i++) {
                indices_array[i] = this->indices[i];
            }
            this->indices_size = sizeof(indices_array);
            VAO.Bind();
            VBO.Update(vertices_array, sizeof(vertices_array));
            EBO.Update(indices_array, sizeof(indices_array));
            VAO.Unbind();
        };

        void Render() {
            VAO.Bind();
            glDrawElements(GL_TRIANGLES, this->indices_size, GL_UNSIGNED_INT, 0);
            VAO.Unbind();
        };
    };
}  // namespace Entity
