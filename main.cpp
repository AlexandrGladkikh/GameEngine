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
#include "engine/CameraComponent.h"

#include <GLFW/glfw3.h>

#ifdef ENABLE_EDITOR
#include <QApplication>

#include "editor/SceneNodeTree.h"
#include "editor/UserNodeTreeBuilder.h"
#include "editor/ComponentWidget.h"
#include "editor/Utils.h"

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

    void saveToJson(const std::shared_ptr<engine::Component>& component, rapidjson::Value& component_json, rapidjson::Document::AllocatorType& allocator) const override;

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

#ifdef ENABLE_EDITOR
    appResult = a.exec();
#endif

    return appResult;
}

class CameraFollowComponent : public engine::Component {
public:
    CameraFollowComponent(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene) : Component(id, name, owner_node, owner_scene)
    {
    }

    void setCameraId(uint32_t camera_id)
    {
        m_camera_id = camera_id;
        init();
    }
    uint32_t getCameraId() const { return m_camera_id; }

    void init() override
    {
        auto ctx = context().lock();
        if (!ctx) {
            return;
        }

        auto scene = ctx->sceneStore->get(ownerScene());
        if (!scene.has_value()) {
            return;
        }

        auto camera = scene.value()->getComponent(m_camera_id);
        if (!camera.has_value()) {
            return;
        }

        if (!std::dynamic_pointer_cast<engine::CameraComponent>(camera.value())) {
            return;
        }

        auto camera_node = camera.value()->getNode();
        if (!camera_node) {
            return;
        }

        auto camera_transform = camera_node.value()->getComponent<engine::TransformComponent>();
        if (!camera_transform.has_value()) {
            return;
        }

        auto self_node = getNode();
        if (!self_node) {
            return;
        }

        auto self_transform = self_node.value()->getComponent<engine::TransformComponent>();
        if (!self_transform.has_value()) {
            return;
        }

        m_camera_offset = self_transform.value()->getPosition() - camera_transform.value()->getPosition();

        m_self_transform = self_transform;
        m_camera_transform = camera_transform;
    }

    void update(uint64_t dt) override
    {
        if (!m_camera_transform.has_value() || !m_self_transform.has_value()) {
            return;
        }

        auto camera_transform = m_camera_transform.value();
        auto self_transform = m_self_transform.value();

        if (m_previous_position == self_transform->getPosition()) {
            return;
        }

        camera_transform->setPosition(self_transform->getPosition() - m_camera_offset);

        m_previous_position = self_transform->getPosition();
    }

    [[nodiscard]]
    bool isDirty() const override { return true; }
    void markDirty() override {}
    void clearDirty() override {}

    [[nodiscard]]
    std::string type() const override { return "camera_follow"; }

private:
    uint32_t m_camera_id = 0;

    glm::vec3 m_camera_offset = { 0.0f, 0.0f, 0.0f };

    glm::vec3 m_previous_position = { 0.0f, 0.0f, 0.0f };

    std::optional<std::shared_ptr<engine::TransformComponent>> m_camera_transform = std::nullopt;
    std::optional<std::shared_ptr<engine::TransformComponent>> m_self_transform = std::nullopt;
};

class MoveComponent : public engine::Component {
public:
    MoveComponent(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene) : Component(id, name, owner_node, owner_scene)
    {

    }

    void setSpeed(float speed) { m_speed = speed; }
    float getSpeed() const { return m_speed; }

    std::shared_ptr<engine::Node> m_birdNode;

    const float m_gravity = -9.81f;
    float m_currentBirdSpeed = 0.0f;

    float m_speed = 200.0f;

    void init() override
    {
        context().lock()->inputManager->registerHandler(GLFW_KEY_W, [this](int action) {
            if (action == GLFW_PRESS) {
                m_up = true;
            } else if (action == GLFW_RELEASE) {
                m_up = false;
            }
        });
        context().lock()->inputManager->registerHandler(GLFW_KEY_S, [this](int action) {
            if (action == GLFW_PRESS) {
                m_down = true;
            } else if (action == GLFW_RELEASE) {
                m_down = false;
            }
        });
        context().lock()->inputManager->registerHandler(GLFW_KEY_A, [this](int action) {
            if (action == GLFW_PRESS) {
                m_left = true;
            } else if (action == GLFW_RELEASE) {
                m_left = false;
            }
        });
        context().lock()->inputManager->registerHandler(GLFW_KEY_D, [this](int action) {
            if (action == GLFW_PRESS) {
                m_right = true;
            } else if (action == GLFW_RELEASE) {
                m_right = false;
            }
        });

        context().lock()->inputManager->registerHandler(GLFW_KEY_ESCAPE, [this](int action) {
            if (action == GLFW_PRESS) {
                context().lock()->engineAccessor->stop();
            }
        });

        context().lock()->inputManager->registerHandler(GLFW_KEY_SPACE, [this](int action) {
            if (action == GLFW_PRESS) {
                m_space = true;
            } else {
                m_space = false;
            }
        });
    }

    void update(uint64_t dt) override
    {
        engine::Logger::info("MoveComponent::update");
        std::optional<std::shared_ptr<engine::TransformComponent>> transform = getNode().value()->getComponent<engine::TransformComponent>();

        if (m_up) {
            transform.value()->setPosition(transform.value()->getPosition() + glm::vec3(0.0f, 1.0f * m_speed * dt / 1000000, 0.0f));
        }
        if (m_down) {
            transform.value()->setPosition(transform.value()->getPosition() + glm::vec3(0.0f, -1.0f * m_speed * dt / 1000000, 0.0f));
        }
        if (m_left) {
            transform.value()->setPosition(transform.value()->getPosition() + glm::vec3(-1.0f * m_speed * dt / 1000000, 0.0f, 0.0f));
        }
        if (m_right) {
            transform.value()->setPosition(transform.value()->getPosition() + glm::vec3(1.0f * m_speed * dt / 1000000, 0.0f, 0.0f));
        }
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
        auto move = std::dynamic_pointer_cast<MoveComponent>(component);

        widget = new editor::ComponentWidget;

        QVBoxLayout* layout = new QVBoxLayout;
        layout->setSpacing(1);
        layout->setContentsMargins(0, 0, 0, 0);

        auto* label = new QLabel("Move");
        label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        layout->addWidget(label);

        auto speedChangeHandler = [move](const std::string& value) {
            move->setSpeed(std::stof(value));
        };

        auto speedUpdater = [move]() {
            if (!move->isValid()) {
                return std::string();
            }
            return editor::formatFloat(move->getSpeed());
        };

        QHBoxLayout* speed_layout = new QHBoxLayout();
        speed_layout->setSpacing(1);
        speed_layout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel("Speed");
        label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        speed_layout->addWidget(label);
        speed_layout->addWidget(editor::createLabelLineEditorWidget("speed", editor::formatFloat(move->getSpeed()), speedChangeHandler, speedUpdater));
        speed_layout->addStretch();
        layout->addLayout(speed_layout);

        layout->addStretch();

        widget->setLayout(layout);
    } else if (component->type() == "camera_follow") {
        auto camera_follow = std::dynamic_pointer_cast<CameraFollowComponent>(component);

        widget = new editor::ComponentWidget;

        QVBoxLayout* layout = new QVBoxLayout;
        layout->setSpacing(1);
        layout->setContentsMargins(0, 0, 0, 0);

        auto* label = new QLabel("Camera follow");
        label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        layout->addWidget(label);

        auto cameraIdChangeHandler = [camera_follow](const std::string& value) {
            camera_follow->setCameraId(std::stof(value));
        };

        auto cameraIdUpdater = [camera_follow]() {
            if (!camera_follow->isValid()) {
                return std::string();
            }
            return editor::formatFloat(camera_follow->getCameraId());
        };

        QHBoxLayout* position_layout = new QHBoxLayout();
        position_layout->setSpacing(1);
        position_layout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel("Camera Id");
        label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        position_layout->addWidget(label);
        position_layout->addWidget(editor::createLabelLineEditorWidget("camera id", editor::formatFloat(camera_follow->getCameraId()), cameraIdChangeHandler, cameraIdUpdater));
        position_layout->addStretch();
        layout->addLayout(position_layout);

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

        auto move = component["move"].GetFloat();

        auto new_component = std::make_unique<MoveComponent>(id, name, owner_node, owner_scene);
        new_component->setSpeed(move);

        return new_component;
    } else if (type == "camera_follow") {
        auto id = component["id"].GetUint();
        auto name = component["name"].GetString();
        auto owner_node = component["owner_node"].GetUint();
        auto owner_scene = component["owner_scene"].GetUint();

        auto camera_id = component["camera_id"].GetUint();

        auto new_component = std::make_unique<CameraFollowComponent>(id, name, owner_node, owner_scene);
        new_component->setCameraId(camera_id);

        return new_component;
    }

    return {};
}

auto EngineComponentBuilder::buildEmptyComponent(const std::string& type, const std::string& name, uint32_t owner_node, uint32_t owner_scene) -> std::optional<std::unique_ptr<engine::Component>>
{
    if (type == "move") {
        return std::make_unique<MoveComponent>(engine::generateUniqueId(), name, owner_node, owner_scene);
    } else if (type == "camera_follow") {
        return std::make_unique<CameraFollowComponent>(engine::generateUniqueId(), name, owner_node, owner_scene);
    }

    return std::nullopt;
}

void EngineComponentBuilder::saveToJson(const std::shared_ptr<engine::Component>& component, rapidjson::Value& component_json, rapidjson::Document::AllocatorType& allocator) const
{
    rapidjson::Value value_name;
    value_name.SetString(component->name().c_str(), allocator);
    if (component->type() == "move") {
        auto move = std::dynamic_pointer_cast<MoveComponent>(component);

        component_json.AddMember("type", "move", allocator);
        component_json.AddMember("id", move->id(), allocator);
        component_json.AddMember("name", value_name, allocator);
        component_json.AddMember("owner_node", move->ownerNode(), allocator);
        component_json.AddMember("owner_scene", move->ownerScene(), allocator);
        component_json.AddMember("move", move->getSpeed(), allocator);
    } else if (component->type() == "camera_follow") {
        auto camera_follow = std::dynamic_pointer_cast<CameraFollowComponent>(component);

        component_json.AddMember("type", "camera_follow", allocator);
        component_json.AddMember("id", camera_follow->id(), allocator);
        component_json.AddMember("name", value_name, allocator);
        component_json.AddMember("owner_node", camera_follow->ownerNode(), allocator);
        component_json.AddMember("owner_scene", camera_follow->ownerScene(), allocator);
        component_json.AddMember("camera_id", camera_follow->getCameraId(), allocator);
    }
}

auto EngineComponentBuilder::componentTypes() const -> const std::vector<std::string>&
{
    static const std::vector<std::string> types = {"move", "camera_follow"};
    return types;
}
