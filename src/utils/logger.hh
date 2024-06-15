#pragma once

#include <imgui.h>

#include <algorithm>
#include <ctime>
#include <format>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#define ASCII_COLOR_RESET "\033[0m"
#define ASCII_COLOR_BOLD "\033[1m"
#define ASCII_COLOR_GRAY "\033[90m"
#define ASCII_COLOR_BOLD_RED "\033[1;31m"
#define ASCII_COLOR_DARK_RED "\033[31m"
#define ASCII_COLOR_BOLD_YELLOW "\033[1;33m"
#define ASCII_COLOR_BOLD_GREEN "\033[1;32m"

namespace Utils {
    enum class LogLevel { INFO, WARNING, ERROR, FATAL, BLANK };

    class Logger {
       private:
        std::vector<std::pair<std::string, Utils::LogLevel>> messages;
        int* logger_window_size;
        bool scrolled = false;

        std::string getTimestapm() {
            auto t = std::time(nullptr);
            auto tm = *std::localtime(&t);

            std::ostringstream oss;
            oss << std::put_time(&tm, "%d-%m-%Y %H:%M:%S");
            return oss.str();
        };

        std::vector<std::string> splitStringByNewline(const std::string str) {
            std::vector<std::string> result;
            std::istringstream stream(str);
            std::string line;

            while (std::getline(stream, line)) {
                result.push_back(line);
            }

            return result;
        }

        void blank(const std::string& message) { Log(message, LogLevel::BLANK); };

        const char* getLogLevel(LogLevel level) {
            switch (level) {
                case LogLevel::INFO:
                    return "INFO";
                case LogLevel::WARNING:
                    return "WARN";
                case LogLevel::ERROR:
                    return "ERROR";
                case LogLevel::FATAL:
                    return "FATAL";
                case LogLevel::BLANK:
                    return "";
            }
        }

        void setLogLevelColor(LogLevel level) {
            ImGuiStyle* style = &ImGui::GetStyle();
            switch (level) {
                case LogLevel::INFO:
                    style->Colors[ImGuiCol_Text] = ImVec4(0.75f, 0.75f, 0.75f, 1.0f);
                    break;
                case LogLevel::WARNING:
                    style->Colors[ImGuiCol_Text] = ImVec4(1.0f, 0.85f, 0.0f, 1.0f);
                    break;
                case LogLevel::ERROR:
                    style->Colors[ImGuiCol_Text] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
                    break;
                case LogLevel::FATAL:
                    style->Colors[ImGuiCol_Text] = ImVec4(0.8f, 0.2f, 0.2f, 1.0f);
                    break;
                case LogLevel::BLANK:
                    break;
            }
        }

        void resetLogLevelColor() { ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); }

        bool startsWith(const std::string& str, const std::string prefix) { return str.rfind(prefix, 0) == 0; }

        bool isFloat(const std::string& s) {
            std::istringstream iss(s);
            float f;
            char c;
            return iss >> f && !(iss >> c);
        }

       public:
        Logger(){};

        void SetLoggerWindowSize(int& logger_window_size) { this->logger_window_size = &logger_window_size; };

        void Log(const std::string& message, Utils::LogLevel level) {
            messages.push_back(std::make_pair(message, level));
            // Replace all ~ in message with ASCII_COLOR_BOLD_GREEN and ASCII_COLOR_RESET alternately
            // std::string color_message = "";
            // std::string color = ASCII_COLOR_BOLD_GREEN;
            // for (int i = 0; i < message.length(); i++) {
            //     if (message[i] == '~') {
            //         color_message += color;
            //         color = (color == ASCII_COLOR_BOLD_GREEN) ? ASCII_COLOR_RESET : ASCII_COLOR_BOLD_GREEN;
            //     } else {
            //         color_message += message[i];
            //     }
            // }
            std::string color = ASCII_COLOR_GRAY;
            if (level == LogLevel::WARNING) color = ASCII_COLOR_BOLD_YELLOW;
            if (level == LogLevel::ERROR) color = ASCII_COLOR_BOLD_RED;
            if (level == LogLevel::FATAL) color = ASCII_COLOR_DARK_RED;
            std::string timestamp = this->getTimestapm();
            std::string contex = std::format("{}{}{} {}{}{}", color, "[", timestamp, getLogLevel(level), "]: ", ASCII_COLOR_RESET);
            int contex_len = 4 + timestamp.length();
            std::vector<std::string> lines = splitStringByNewline(message);
            std::cout << contex << ASCII_COLOR_RESET;
            int i = 0;
            for (auto& line : lines) {
                if (i != 0) {
                    std::cout << std::string(contex_len, ' ');
                }
                std::cout << line << std::endl;
                i++;
            }
        };

        void Info(const std::string& message) { Log(message, LogLevel::INFO); };
        void Warn(const std::string& message) { Log(message, LogLevel::WARNING); };
        void Error(const std::string& message) { Log(message, LogLevel::ERROR); };
        void Fatal(const std::string& message) {
            Log(message, LogLevel::FATAL);
            exit(1);
        };

        void Render() {
            float display_height = ImGui::GetIO().DisplaySize.y;
            ImGui::SetNextWindowPos(ImVec2(0, (float)(display_height - *this->logger_window_size)));
            ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, (float)*this->logger_window_size));
            ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w = 0.75f;
            ImGui::GetStyle().Colors[ImGuiCol_TitleBg].w = 0.75f;
            ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive].w = 0.75f;
            ImGui::Begin("Log", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings);

            for (auto& message : messages) {
                this->setLogLevelColor(message.second);
                if (message.second == LogLevel::BLANK) {
                    ImGui::Text("%s%s", this->getLogLevel(message.second), message.first.c_str());
                } else {
                    ImGui::Text("%s: %s", this->getLogLevel(message.second), message.first.c_str());
                }
                this->resetLogLevelColor();
            }

            static char str0[128] = "";
            if (ImGui::InputTextWithHint(" ", "Enter a command here; 'help' for help", str0, IM_ARRAYSIZE(str0), ImGuiInputTextFlags_EnterReturnsTrue)) {
                std::string message = str0;
                str0[0] = '\0';
                if (message == "help") {
                    this->blank("Available commands:");
                    this->blank("  - help: Display this help message");
                    this->blank("  - clear: Clear the log");
                    // this->blank("  - tp <X> <Y> <Z>: Teleport player to the specified coordinates");
                } else if (message == "clear") {
                    messages.clear();
                }
                // else if (startsWith(message, "tp")) {
                //     std::vector<std::string> args;
                //     std::istringstream stream(message);
                //     std::string arg;
                //     while (stream >> arg) {
                //         args.push_back(arg);
                //     }
                //     if (args.size() != 4) {
                //         this->Warning("Invalid number of arguments");
                //     } else {
                //         bool success = true;
                //         for (auto& arg : args) {
                //             if (arg == "tp") {
                //                 continue;
                //             }
                //             if (!isFloat(arg)) {
                //                 this->Warning("Invalid argument: " + arg);
                //                 success = false;
                //             }
                //         }
                //         if (success) {
                //             this->Info("Teleporting player to " + args[1] + ", " + args[2] + ", " + args[3]);
                //         }
                //     }
                // }
                else {
                    this->Warn("Unknown command");
                }
            }

            if (ImGui::GetScrollY() < ImGui::GetScrollMaxY()) {
                this->scrolled = true;
            } else {
                this->scrolled = false;
            }
            if (!this->scrolled) {
                // Scroll to the bottom
                ImGui::SetScrollHereY(1.0f);
            }

            ImGui::End();
            ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w = 1.0f;
            ImGui::GetStyle().Colors[ImGuiCol_TitleBg].w = 1.0f;
            ImGui::GetStyle().Colors[ImGuiCol_TitleBgActive].w = 1.0f;
        };
    };
}  // namespace Utils
