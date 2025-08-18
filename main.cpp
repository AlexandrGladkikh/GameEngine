#include "engine/Engine.h"
#include "engine/UserComponentsBuilder.h"
#include "engine/Component.h"
#include "engine/InputManager.h"
#include "engine/TransformComponent.h"
#include "engine/Node.h"
#include "engine/Logger.h"
#include "engine/MaterialComponent.h"
#include "engine/Window.h"
#include "engine/MeshComponent.h"
#include "engine/FlipbookAnimationComponent.h"
#include "engine/Utils.h"

#include <GLFW/glfw3.h>

#ifdef ENABLE_EDITOR
#include <QApplication>

#include "editor/SceneNodeTree.h"
#include "editor/UserNodeTreeBuilder.h"
#include "editor/ComponentWidget.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
class EditorComponentBuilder : public editor::UserNodeTreeBuilder {
public:
    [[nodiscard]]
    std::optional<editor::ComponentWidget*> buildWidgetForComponent(const std::shared_ptr<engine::Component>& component) const override;
};

#endif

class EngineComponentBuilder : public engine::UserComponentsBuilder {
public:
    std::optional<std::unique_ptr<engine::Component>> buildComponent(const std::string& type, rapidjson::Value& component) const override;
    auto buildEmptyComponent(const std::string& type, const std::string& name, uint32_t owner_node, uint32_t owner_scene) -> std::optional<std::unique_ptr<engine::Component>> override;
    auto componentTypes() const -> const std::vector<std::string>& override;
};

int main(int argc, char *argv[])
{
    int appResult = 0;

    engine::Engine engine;
    engine.setUserComponentsBuilder(std::make_unique<EngineComponentBuilder>());
    if (!engine.initialize("../configs/engine.json")) {
        return 1;
    }

#ifdef ENABLE_EDITOR
    QApplication a(argc, argv);

    auto context = engine.context();

    auto currentScene = context->sceneStore->get(engine.getActiveSceneId());

    editor::SceneNodeTree sceneNodeTree(&engine);
    sceneNodeTree.setUserComponentsBuilder(std::make_unique<EditorComponentBuilder>());
    sceneNodeTree.build(currentScene);
    sceneNodeTree.show();

#endif

    engine.run();
    appResult = a.exec();

    return appResult;
}

class MoveComponent : public engine::Component {
public:
    MoveComponent(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene) : Component(id, name, owner_node, owner_scene)
    {

    }

    std::shared_ptr<engine::Node> m_birdNode;

    const float m_gravity = -9.81f;
    float m_currentBirdSpeed = 0.0f;

    void init() override
    {
        context().lock()->inputManager->registerHandler(GLFW_KEY_W, [this](int key, int action) {
            if (action == GLFW_PRESS) {
                m_up = true;
            } else if (action == GLFW_RELEASE) {
                m_up = false;
            }
        });
        context().lock()->inputManager->registerHandler(GLFW_KEY_S, [this](int key, int action) {
            if (action == GLFW_PRESS) {
                m_down = true;
            } else if (action == GLFW_RELEASE) {
                m_down = false;
            }
        });
        context().lock()->inputManager->registerHandler(GLFW_KEY_A, [this](int key, int action) {
            if (action == GLFW_PRESS) {
                m_left = true;
            } else if (action == GLFW_RELEASE) {
                m_left = false;
            }
        });
        context().lock()->inputManager->registerHandler(GLFW_KEY_D, [this](int key, int action) {
            if (action == GLFW_PRESS) {
                m_right = true;
            } else if (action == GLFW_RELEASE) {
                m_right = false;
            }
        });

        context().lock()->inputManager->registerHandler(GLFW_KEY_ESCAPE, [this](int key, int action) {
            if (action == GLFW_PRESS) {
                context().lock()->engineAccessor->stop();
            }
        });

        context().lock()->inputManager->registerHandler(GLFW_KEY_SPACE, [this](int key, int action) {
            if (action == GLFW_PRESS) {
                m_space = true;
            } else {
                m_space = false;
            }
        });

        auto windowSize = context().lock()->window->size();

        auto selfNodeOpt = context().lock()->sceneStore->get(ownerScene()).value()->getNode("root");
        if (!selfNodeOpt.has_value()) {
            return;
        }

        auto selfNode = selfNodeOpt.value();

        auto topPipe = selfNode->addChild("topPipe");
        auto topTransform = topPipe->addComponent<engine::TransformComponent>("top pipe transform");
        if (!topTransform.has_value()) {
            return;
        }

        auto topPipeMaterial = topPipe->addComponent<engine::MaterialComponent>("top pipe texture");
        if (!topPipeMaterial.has_value()) {
            return;
        }
        topPipeMaterial.value()->setTexture(4);
        topPipeMaterial.value()->setShader(1);

        auto topPipeSize = topPipeMaterial.value()->textureSize();

        topTransform.value()->setPosition(glm::vec3(windowSize.first / 2, windowSize.second - topPipeSize.second / 2 + 100, -100));

        auto topMesh = topPipe->addComponent<engine::MeshComponent>("top pipe mesh");
        if (!topMesh.has_value()) {
            return;
        }
        topMesh.value()->setMesh(1);

        auto bottomPipe = selfNode->addChild("bottomPipe");
        auto bottomTransform = bottomPipe->addComponent<engine::TransformComponent>("bottom pipe transform");
        if (!bottomTransform.has_value()) {
            return;
        }
        bottomTransform.value()->setScale(glm::vec3(1, -1, 1));

        auto bottomPipeMaterial = bottomPipe->addComponent<engine::MaterialComponent>("bottom pipe texture");
        if (!bottomPipeMaterial.has_value()) {
            return;
        }
        bottomPipeMaterial.value()->setTexture(4);
        bottomPipeMaterial.value()->setShader(1);

        auto bottomPipeSize = bottomPipeMaterial.value()->textureSize();

        bottomTransform.value()->setPosition(glm::vec3(windowSize.first / 2, bottomPipeSize.second / 2 - 100, -100));

        auto bottomMesh = bottomPipe->addComponent<engine::MeshComponent>("top pipe mesh");
        if (!bottomMesh.has_value()) {
            return;
        }
        bottomMesh.value()->setMesh(1);

        auto backgroundCity = selfNode->addChild("background");

        auto backgroundPipeMaterial = backgroundCity->addComponent<engine::MaterialComponent>("background texture");
        if (!backgroundPipeMaterial.has_value()) {
            return;
        }
        backgroundPipeMaterial.value()->setTexture(2);
        backgroundPipeMaterial.value()->setShader(1);

        auto backgroundTransform = backgroundCity->addComponent<engine::TransformComponent>("background transform");
        if (!backgroundTransform.has_value()) {
            return;
        }
        backgroundTransform.value()->setPosition(glm::vec3(windowSize.first / 2, windowSize.second / 2, -200));

        auto backgroundSize = backgroundPipeMaterial.value()->textureSize();

        auto scaleBackground = glm::vec3(static_cast<GLfloat>(windowSize.first) / backgroundSize.first, static_cast<GLfloat>(-windowSize.second) / backgroundSize.second, 1);
        backgroundTransform.value()->setScale(scaleBackground);

        auto backgroundMesh = backgroundCity->addComponent<engine::MeshComponent>("background mesh");
        if (!backgroundMesh.has_value()) {
            return;
        }
        backgroundMesh.value()->setMesh(1);

        m_birdNode = selfNode->addChild("bird");
        auto birdTransform = m_birdNode->addComponent<engine::TransformComponent>("bird transform");
        birdTransform.value()->setPosition(glm::vec3(windowSize.first / 4, windowSize.second / 2, -100));

        auto birdMid = m_birdNode->addChild("bird mid");
        auto birdMidTransform = birdMid->addComponent<engine::TransformComponent>("bird mid transform");
        birdMidTransform.value()->setScale(glm::vec3(1.0f, -1.0f, 1.0f));
        auto birdMidMaterial = birdMid->addComponent<engine::MaterialComponent>("bird mid material");
        birdMidMaterial.value()->setTexture("yellowbird-midflap");
        birdMidMaterial.value()->setShader("default");
        auto birdMidMesh = birdMid->addComponent<engine::MeshComponent>("bird mid mesh");
        birdMidMesh.value()->setMesh("quad");

        auto birdTop = m_birdNode->addChild("bird top");
        auto birdTopTransform = birdTop->addComponent<engine::TransformComponent>("bird top transform");
        birdTopTransform.value()->setScale(glm::vec3(1.0f, -1.0f, 1.0f));
        auto birdTopMaterial = birdTop->addComponent<engine::MaterialComponent>("bird top material");
        birdTopMaterial.value()->setTexture("yellowbird-upflap");
        birdTopMaterial.value()->setShader("default");
        auto birdTopMesh = birdTop->addComponent<engine::MeshComponent>("bird top mesh");
        birdTopMesh.value()->setMesh("quad");

        auto birdBottom = m_birdNode->addChild("bird bottom");
        auto birdBottomTransform = birdBottom->addComponent<engine::TransformComponent>("bird bottom transform");
        birdBottomTransform.value()->setScale(glm::vec3(1.0f, -1.0f, 1.0f));
        auto birdBottomMaterial = birdBottom->addComponent<engine::MaterialComponent>("bird bottom material");
        birdBottomMaterial.value()->setTexture("yellowbird-downflap");
        birdBottomMaterial.value()->setShader("default");
        auto birdBottomMesh = birdBottom->addComponent<engine::MeshComponent>("bird bottom mesh");
        birdBottomMesh.value()->setMesh("quad");

        auto flipbookAnimation = selfNode->addComponent<engine::FlipbookAnimationComponent>("flipbook animation");
        if (!flipbookAnimation.has_value()) {
            return;
        }
        flipbookAnimation.value()->addMaterial("bird mid material");
        flipbookAnimation.value()->addMaterial("bird top material");
        flipbookAnimation.value()->addMaterial("bird bottom material");
        flipbookAnimation.value()->setUpdateTime(100000);
        flipbookAnimation.value()->start();
    }

    void update(uint64_t dt) override
    {
        static float speed = 200;
        engine::Logger::info("MoveComponent::update");
        // std::optional<std::shared_ptr<engine::TransformComponent>> transform = getNode().value()->getComponent<engine::TransformComponent>();
        //
        // if (m_up) {
        //     transform.value()->setPosition(transform.value()->getPosition() + glm::vec3(0.0f, 1.0f * speed * dt / 1000000, 0.0f));
        // }
        // if (m_down) {
        //     transform.value()->setPosition(transform.value()->getPosition() + glm::vec3(0.0f, -1.0f * speed * dt / 1000000, 0.0f));
        // }
        // if (m_left) {
        //     transform.value()->setPosition(transform.value()->getPosition() + glm::vec3(-1.0f * speed * dt / 1000000, 0.0f, 0.0f));
        // }
        // if (m_right) {
        //     transform.value()->setPosition(transform.value()->getPosition() + glm::vec3(1.0f * speed * dt / 1000000, 0.0f, 0.0f));
        // }
        //
        // if (m_space) {
        //     m_currentBirdSpeed = 5.0f;
        // }
        //
        // m_currentBirdSpeed = m_currentBirdSpeed + m_gravity * dt / 1000000;
        // engine::Logger::info("m_currentBirdSpeed: {}", m_currentBirdSpeed);
        // auto birdTransform = m_birdNode->getComponent<engine::TransformComponent>();
        // // birdTransform.value()->setPosition(birdTransform.value()->getPosition() + glm::vec3(0.0f, m_currentBirdSpeed, 0.0f));
        // birdTransform.value()->setRotation(glm::vec3(0.0f, 0.0f, m_currentBirdSpeed * 10 > -90.0f ? m_currentBirdSpeed * 10 : -90.0f));
    }

    [[nodiscard]]
    bool isDirty() const override { return true; }
    void markDirty() override {}
    void clearDirty() override {}

    [[nodiscard]]
    std::string type() const override { return "move"; }

private:
    bool m_up = false;
    bool m_down = false;
    bool m_left = false;
    bool m_right = false;
    bool m_space = false;
};


#ifdef ENABLE_EDITOR
std::optional<editor::ComponentWidget*> EditorComponentBuilder::buildWidgetForComponent(const std::shared_ptr<engine::Component>& component) const
{
    editor::ComponentWidget* widget = nullptr;
    if (component->type() == "move") {
        widget = new editor::ComponentWidget;

        QVBoxLayout* layout = new QVBoxLayout;
        layout->setSpacing(1);
        layout->setContentsMargins(0, 0, 0, 0);

        auto* label = new QLabel("Move");
        label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        layout->addWidget(label);
        layout->addStretch();

        widget->setLayout(layout);
    }

    return widget;
}
#endif

std::optional<std::unique_ptr<engine::Component>> EngineComponentBuilder::buildComponent(const std::string& type, rapidjson::Value& component) const
{
    if (type == "move") {
        auto id = component["id"].GetUint();
        auto name = component["name"].GetString();
        auto owner_node = component["owner_node"].GetUint();
        auto owner_scene = component["owner_scene"].GetUint();

        return std::make_unique<MoveComponent>(id, name, owner_node, owner_scene);
    }

    return {};
}

auto EngineComponentBuilder::buildEmptyComponent(const std::string& type, const std::string& name, uint32_t owner_node, uint32_t owner_scene) -> std::optional<std::unique_ptr<engine::Component>>
{
    if (type == "move") {
        return std::make_unique<MoveComponent>(engine::generateUniqueId(), name, owner_node, owner_scene);
    }

    return std::nullopt;
}

auto EngineComponentBuilder::componentTypes() const -> const std::vector<std::string>&
{
    static const std::vector<std::string> types = {"move"};
    return types;
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