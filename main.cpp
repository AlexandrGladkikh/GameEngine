#include "Engine.h"

int main()
{
    engine::Engine engine;
    if (engine.initialize("../configs/engine.json")) {
        engine.run();
    }

    return 0;
}

// #include "Shader.h"
//
// #include <glad/glad.h>
// #include <GLFW/glfw3.h>
//
// #define STB_IMAGE_IMPLEMENTATION
// #include <stb/stb_image.h>
//
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>
//
// #include <iostream>
// #include <thread>
// #include <cmath>
//
// void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
//     glViewport(0, 0, width, height);
// }
//
// GLfloat deltaTime = 0.0f;
//
// bool keys[1024];
//
// glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
// glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
// glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
//
// GLfloat lastX = 400, lastY = 300;
// GLfloat yaw = -90.0f;
// GLfloat pitch = 0.0f;
//
// GLfloat fov = 45.0f;
//
// void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
//     GLfloat cameraSpeed = 0.05f;
//     if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//         glfwSetWindowShouldClose(window, true);
//     else if (key == GLFW_KEY_W && action == GLFW_PRESS)
//         keys[GLFW_KEY_W] = true;
//     else if (key == GLFW_KEY_S && action == GLFW_PRESS)
//         keys[GLFW_KEY_S] = true;
//     else if (key == GLFW_KEY_A && action == GLFW_PRESS)
//         keys[GLFW_KEY_A] = true;
//     else if (key == GLFW_KEY_D && action == GLFW_PRESS)
//         keys[GLFW_KEY_D] = true;
//     else if (key == GLFW_KEY_W && action == GLFW_RELEASE)
//         keys[GLFW_KEY_W] = false;
//     else if (key == GLFW_KEY_S && action == GLFW_RELEASE)
//         keys[GLFW_KEY_S] = false;
//     else if (key == GLFW_KEY_A && action == GLFW_RELEASE)
//         keys[GLFW_KEY_A] = false;
//     else if (key == GLFW_KEY_D && action == GLFW_RELEASE)
//         keys[GLFW_KEY_D] = false;
// }
//
// void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
//     static bool firstMouse = true;
//     if (firstMouse) {
//         lastX = xpos;
//         lastY = ypos;
//         firstMouse = false;
//     }
//
//     GLfloat xoffset = xpos - lastX;
//     GLfloat yoffset = lastY - ypos;
//     lastX = xpos;
//     lastY = ypos;
//
//     GLfloat sensitivity = 0.05f;
//     xoffset *= sensitivity;
//     yoffset *= sensitivity;
//
//     yaw += xoffset;
//     pitch += yoffset;
//
//     if (pitch > 89.0f)
//         pitch = 89.0f;
//     if (pitch < -89.0f)
//         pitch = -89.0f;
//
//     glm::vec3 front;
//     front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
//     front.y = sin(glm::radians(pitch));
//     front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
//     cameraFront = glm::normalize(front);
// }
//
// void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
//     if (fov >= 1.0f && fov <= 45.0f)
//         fov -= yoffset;
//     if (fov <= 1.0f)
//         fov = 1.0f;
//     if (fov >= 45.0f)
//         fov = 45.0f;
// }
//
// void do_movement() {
//     GLfloat cameraSpeed = 5.0f * deltaTime;
//     if (keys[GLFW_KEY_W])
//         cameraPos += cameraSpeed * cameraFront;
//     if (keys[GLFW_KEY_S])
//         cameraPos -= cameraSpeed * cameraFront;
//     if (keys[GLFW_KEY_A])
//         cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
//     if (keys[GLFW_KEY_D])
//         cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
// }
//
// int main() {
//     glfwInit();
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//     glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
//
//     GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
//
//     if (window == nullptr) {
//         std::cout << "Failed to create GLFW window" << std::endl;
//         glfwTerminate();
//         return -1;
//     }
//
//     glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//
//     glfwMakeContextCurrent(window);
//
//     if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
//         std::cout << "Failed to initialize GLAD" << std::endl;
//         glfwTerminate();
//         return -1;
//     }
//
//     glEnable(GL_DEPTH_TEST);
//
//     int width, height;
//     glfwGetFramebufferSize(window, &width, &height);
//     glViewport(0, 0, width, height);
//
//     glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//
//     glfwSetKeyCallback(window, key_callback);
//     glfwSetCursorPosCallback(window, mouse_callback);
//     glfwSetScrollCallback(window, scroll_callback);
//
//     float vertices[] = {
//         -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
//          0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
//          0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//          0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
//         -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
//
//         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//          0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
//          0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
//          0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
//         -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
//         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//
//         -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//         -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//         -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//
//          0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//          0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//          0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//          0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//          0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//          0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//
//         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//          0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
//          0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
//          0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
//         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
//         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
//
//         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
//          0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
//          0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//          0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
//         -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
//         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
//     };
//
//     glm::vec3 cubePositions[] = {
//         glm::vec3( 0.0f,  0.0f,  0.0f),
//         glm::vec3( 2.0f,  5.0f, -15.0f),
//         glm::vec3(-1.5f, -2.2f, -2.5f),
//         glm::vec3(-3.8f, -2.0f, -12.3f),
//         glm::vec3( 2.4f, -0.4f, -3.5f),
//         glm::vec3(-1.7f,  3.0f, -7.5f),
//         glm::vec3( 1.3f, -2.0f, -2.5f),
//         glm::vec3( 1.5f,  2.0f, -2.5f),
//         glm::vec3( 1.5f,  0.2f, -1.5f),
//         glm::vec3(-1.3f,  1.0f, -1.5f)
//       };
//
//     int widthTex, heightTex, nrChannels;
//     unsigned char* data = stbi_load("../textures/container.jpg", &widthTex, &heightTex, &nrChannels, 0);
//
//     GLuint texture1;
//     glGenTextures(1, &texture1);
//     glBindTexture(GL_TEXTURE_2D, texture1);
//
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthTex, heightTex, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//     glGenerateMipmap(GL_TEXTURE_2D);
//
//     stbi_image_free(data);
//     glBindTexture(GL_TEXTURE_2D, 0);
//
//     stbi_set_flip_vertically_on_load(true);
//
//     data = stbi_load("../textures/awesomeface.png", &widthTex, &heightTex, &nrChannels, 0);
//
//     stbi_set_flip_vertically_on_load(true);
//
//     GLuint texture2;
//     glGenTextures(1, &texture2);
//     glBindTexture(GL_TEXTURE_2D, texture2);
//
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthTex, heightTex, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//     glGenerateMipmap(GL_TEXTURE_2D);
//
//     stbi_image_free(data);
//     glBindTexture(GL_TEXTURE_2D, 0);
//
//     GLuint VAO, VBO;
//     glGenVertexArrays(1, &VAO);
//     glGenBuffers(1, &VBO);
//
//     glBindVertexArray(VAO);
//
//     glBindBuffer(GL_ARRAY_BUFFER, VBO);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
//     glEnableVertexAttribArray(0);
//
//     glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
//     glEnableVertexAttribArray(1);
//
//     glBindVertexArray(0);
//     glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//     Shader shaderProgram("../shaders/shader.vs", "../shaders/shader.frag");
//
//     GLuint modeLocation = glGetUniformLocation(shaderProgram.program, "model");
//     GLuint viewLocation = glGetUniformLocation(shaderProgram.program, "view");
//     GLuint projectionLocation = glGetUniformLocation(shaderProgram.program, "projection");
//     GLuint texture1Location = glGetUniformLocation(shaderProgram.program, "texture1");
//     GLuint texture2Location = glGetUniformLocation(shaderProgram.program, "texture2");
//
//     GLfloat lastFrame = 0.0f;
//
//     while (!glfwWindowShouldClose(window)) {
//         GLfloat currentFrame = glfwGetTime();
//         deltaTime = currentFrame - lastFrame;
//         lastFrame = currentFrame;
//
//         glfwPollEvents();
//
//         do_movement();
//
//         glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//         shaderProgram.use();
//
//         glm::mat4 view = glm::mat4(1.0f);
//         view = glm::lookAt(cameraPos,
//             cameraPos + cameraFront,
//             cameraUp);
//
//         glm::mat4 projection = glm::mat4(1.0f);
//         projection = glm::perspective(glm::radians(fov), (float)width / (float)height, 0.1f, 100.0f);
//
//
//         glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
//         glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
//
//         glActiveTexture(GL_TEXTURE0);
//         glBindTexture(GL_TEXTURE_2D, texture1);
//         glUniform1i(texture1Location, 0);
//         glActiveTexture(GL_TEXTURE1);
//         glBindTexture(GL_TEXTURE_2D, texture2);
//         glUniform1i(texture2Location, 1);
//
//         glBindVertexArray(VAO);
//
//         for (int i = 0; i < 10; i++) {
//             glm::mat4 model = glm::mat4(1.0f);
//             model = glm::translate(model, cubePositions[i]);
//             GLfloat angle = 20.0f * i;
//             model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
//             glUniformMatrix4fv(modeLocation, 1, GL_FALSE, glm::value_ptr(model));
//             glDrawArrays(GL_TRIANGLES, 0, 36);
//         }
//
//         glBindVertexArray(0);
//
//         glfwSwapBuffers(window);
//
//         std::this_thread::sleep_for(std::chrono::milliseconds(16));
//     }
//
//     glfwTerminate();
//
//     return 0;
// }