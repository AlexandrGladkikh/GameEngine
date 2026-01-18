#include "engine/Engine.h"
#include "engine/UserComponentsBuilder.h"
#include "engine/Component.h"
#include "engine/InputManager.h"
#include "engine/TransformComponent.h"
#include "engine/Node.h"
#include "engine/Logger.h"
#include "engine/Utils.h"
#include "engine/CameraComponent.h"
#include "engine/MouseEventFilterComponent.h"

#include <GLFW/glfw3.h>
#include <cstdint>

#ifdef ENABLE_EDITOR
#include <QApplication>

#include "editor/ComponentWidget.h"
#include "editor/EngineController.h"
#include "editor/SceneNodeTree.h"
#include "editor/TreeWidgetBuilderHelper.h"
#include "editor/UserTreeWidgetBuilder.h"
#include "editor/Utils.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

class EditorComponentBuilder : public editor::UserTreeWidgetBuilder {
public:
    [[nodiscard]]
    std::optional<editor::ComponentWidget*> buildWidgetForComponent(const std::shared_ptr<engine::Component>& component) const override;
};

#endif

class EngineComponentBuilder : public engine::UserComponentsBuilder {
public:
    std::optional<std::unique_ptr<engine::Component>> buildComponent(const std::string& type, const rapidjson::Value& component) const override;
    auto buildEmptyComponent(const std::string& type, const std::string& name, uint32_t owner_node, uint32_t owner_scene) -> std::optional<std::unique_ptr<engine::Component>> override;

    void saveToJson(const std::shared_ptr<engine::Component>& component, rapidjson::Value& component_json, rapidjson::Document::AllocatorType& allocator) const override;

    auto componentTypes() const -> const std::vector<std::string>& override;
};

int main(int argc, char *argv[])
{
    int appResult = 0;

#ifdef ENABLE_EDITOR
    engine::Engine engine;
    engine.setUserComponentsBuilder(std::make_unique<EngineComponentBuilder>());
    if (!engine.initialize("../configs/engine.json")) {
        return 1;
    }

    QApplication a(argc, argv);

    auto context = engine.context();

    auto currentScene = context->sceneStore->get(engine.getActiveSceneId());

    editor::SceneNodeTree* sceneNodeTree = new editor::SceneNodeTree(&engine);
    sceneNodeTree->setUserTreeWidgetBuilder(std::make_unique<EditorComponentBuilder>());
    sceneNodeTree->build(currentScene);
    sceneNodeTree->show();

    appResult = a.exec();
#else
    engine::Engine engine;
    engine.setUserComponentsBuilder(std::make_unique<EngineComponentBuilder>());
    if (!engine.initialize("../configs/engine.json")) {
        return -1;
    }

    engine.run();
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

    auto clone(uint32_t owner_node_id) const -> std::unique_ptr<Component> override
    {
        auto clone_component = std::make_unique<CameraFollowComponent>(engine::generateUniqueId(), name(), owner_node_id, ownerScene());
        clone_component->setContext(context());
        clone_component->setValid(isValid());
        clone_component->setActive(isActive());
        clone_component->markDirty();

        clone_component->setCameraId(m_camera_id);

        return clone_component;
    }

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

    void setMoveNodeId(uint32_t node_id) { m_move_node_id = node_id; }
    uint32_t getMoveNodeId() const { return m_move_node_id; }

    void setRestNodeId(uint32_t node_id) { m_rest_node_id = node_id; }
    uint32_t getRestNodeId() const { return m_rest_node_id; }

    void init() override
    {
        context().lock()->inputManager->registerKeyHandler(GLFW_KEY_W, [this](int action) {
            m_up = (action == GLFW_PRESS || action == GLFW_REPEAT);
        });
        context().lock()->inputManager->registerKeyHandler(GLFW_KEY_S, [this](int action) {
            m_down = (action == GLFW_PRESS || action == GLFW_REPEAT);
        });
        context().lock()->inputManager->registerKeyHandler(GLFW_KEY_A, [this](int action) {
            m_left = (action == GLFW_PRESS || action == GLFW_REPEAT);
        });
        context().lock()->inputManager->registerKeyHandler(GLFW_KEY_D, [this](int action) {
            m_right = (action == GLFW_PRESS || action == GLFW_REPEAT);
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
            auto scale = transform.value()->getScale();
            transform.value()->setScale(transform.value()->getScale() * glm::vec3((scale.x < 0.0f ? 1.0f : -1.0f), 1.0f, 1.0f));
        }
        if (m_right) {
            transform.value()->setPosition(transform.value()->getPosition() + glm::vec3(1.0f * m_speed * dt / 1000000, 0.0f, 0.0f));
            auto scale = transform.value()->getScale();
            transform.value()->setScale(transform.value()->getScale() * glm::vec3((scale.x < 0.0f ? -1.0f : 1.0f), 1.0f, 1.0f));
        }

        auto node = getNode().value();
        auto scene = node->getScene().value();

        auto move_node = scene->getNode(m_move_node_id);
        auto rest_node = scene->getNode(m_rest_node_id);
        if (!move_node.has_value() || !rest_node.has_value()) {
            return;
        }

        if (m_up || m_down || m_left || m_right) {
            if (!move_node.value()->isActive() || rest_node.value()->isActive()) {
                move_node.value()->setActive(true);
                rest_node.value()->setActive(false);
            }
        } else {
            if (move_node.value()->isActive() || !rest_node.value()->isActive()) {
                move_node.value()->setActive(false);
                rest_node.value()->setActive(true);
            }
        }
    }

    [[nodiscard]]
    bool isDirty() const override { return true; }
    void markDirty() override {}
    void clearDirty() override {}

    [[nodiscard]]
    std::string type() const override { return "move"; }

    auto clone(uint32_t owner_node_id) const -> std::unique_ptr<Component> override
    {
        auto clone_component = std::make_unique<MoveComponent>(engine::generateUniqueId(), name(), owner_node_id, ownerScene());
        clone_component->setContext(context());
        clone_component->setValid(isValid());
        clone_component->setActive(isActive());
        clone_component->markDirty();

        clone_component->setSpeed(m_speed);

        return clone_component;
    }

private:
    const float m_gravity = -9.81f;

    float m_speed = 200.0f;

    uint32_t m_move_node_id = 0;
    uint32_t m_rest_node_id = 0;

    bool m_up = false;
    bool m_down = false;
    bool m_left = false;
    bool m_right = false;
};

class RotateComponent : public engine::Component {
public:
    enum Axis {
        X,
        Y,
        Z
    };

    explicit RotateComponent(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene) : Component(id, name, owner_node, owner_scene)
    {
    }

    void update(uint64_t dt) override
    {
        engine::Logger::info("RotateComponent::update");
        if (m_rotate) {
            auto transform = getNode().value()->getComponent<engine::TransformComponent>();
            if (transform.has_value()) {
                transform.value()->setRotation(transform.value()->getRotation() +
                                               glm::vec3(m_axis == Axis::X ? m_degrees : 0.0f, m_axis == Axis::Y ? m_degrees : 0.0f, m_axis == Axis::Z ? m_degrees : 0.0f));
            }
            m_rotate = false;
        }
    }

    void init() override
    {
        auto mouse_event_filter = getNode().value()->getComponent<engine::MouseEventFilterComponent>();
        if (!mouse_event_filter.has_value()) {
            return;
        }

        mouse_event_filter.value()->setMouseClickCallback([this](int x, int y) {
            m_rotate = true;
        });
    }

    [[nodiscard]]
    bool isDirty() const override { return true; }
    void markDirty() override {}
    void clearDirty() override {}

    [[nodiscard]]
    std::string type() const override { return "rotate"; }

    auto clone(uint32_t owner_node_id) const -> std::unique_ptr<Component> override
    {
        auto clone_component = std::make_unique<RotateComponent>(engine::generateUniqueId(), name(), owner_node_id, ownerScene());
        clone_component->setContext(context());
        clone_component->setValid(isValid());
        clone_component->setActive(isActive());
        clone_component->markDirty();

        clone_component->setAxis(m_axis);
        clone_component->setDegrees(m_degrees);

        return clone_component;
    }

    void setDegrees(float degrees) { m_degrees = degrees; }
    float getDegrees() const { return m_degrees; }

    void setAxis(Axis axis) { m_axis = axis; }
    Axis getAxis() const { return m_axis; }

private:
    Axis m_axis = Axis::Z;

    bool m_rotate = false;
    float m_degrees = 0.0f;

};

#ifdef ENABLE_EDITOR
std::optional<editor::ComponentWidget*> EditorComponentBuilder::buildWidgetForComponent(const std::shared_ptr<engine::Component>& component) const
{
    editor::ComponentWidget* widget = nullptr;
    if (component->type() == "move") {
        auto move = std::dynamic_pointer_cast<MoveComponent>(component);

        widget = new editor::ComponentWidget;
        treeWidgetBuilderHelper()->subscribeOnActiveComponent(widget, component);

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

        std::vector<editor::EditorBlockLayoutData> speed_data = {
            { "speed", editor::formatFloat(move->getSpeed()), speedChangeHandler, speedUpdater }
        };
        QHBoxLayout* speed_layout = editor::createEditorBlockLayout("", speed_data, engineObserver());
        layout->addLayout(speed_layout);

        auto moveNodeChangeHandler = [move](const std::string& value) {
            uint32_t id;
            if (!editor::parseUint32(value, id)) {
                return;
            }

            move->setMoveNodeId(id);
        };

        auto moveNodeUpdater = [move]() {
            if (!move->isValid()) {
                return std::string();
            }
            return editor::formatUint32(move->getMoveNodeId());
        };

        std::vector<editor::EditorBlockLayoutData> move_node_id_data = {
            { "move node id", editor::formatFloat(move->getMoveNodeId()), moveNodeChangeHandler, moveNodeUpdater, true }
        };
        QHBoxLayout* move_node_id_layout = editor::createEditorBlockLayout("", move_node_id_data, engineObserver());
        layout->addLayout(move_node_id_layout);

        auto restNodeChangeHandler = [move](const std::string& value) {
            uint32_t id;
            if (!editor::parseUint32(value, id)) {
                return;
            }

            move->setRestNodeId(id);
        };

        auto restNodeUpdater = [move]() {
            if (!move->isValid()) {
                return std::string();
            }
            return editor::formatUint32(move->getRestNodeId());
        };

        std::vector<editor::EditorBlockLayoutData> rest_node_id_data = {
            { "rest node id", editor::formatFloat(move->getRestNodeId()), restNodeChangeHandler, restNodeUpdater, true }
        };
        QHBoxLayout* rest_node_id_layout = editor::createEditorBlockLayout("", rest_node_id_data, engineObserver());
        layout->addLayout(rest_node_id_layout);

        layout->addStretch();

        widget->setLayout(layout);
    } else if (component->type() == "camera_follow") {
        auto camera_follow = std::dynamic_pointer_cast<CameraFollowComponent>(component);

        widget = new editor::ComponentWidget;
        std::uintptr_t id = reinterpret_cast<std::uintptr_t>(widget);
        treeWidgetBuilderHelper()->subscribeOnActiveComponent(widget, component);

        QVBoxLayout* layout = new QVBoxLayout;
        layout->setSpacing(1);
        layout->setContentsMargins(0, 0, 0, 0);

        auto* label = new QLabel("Camera follow");
        label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        layout->addWidget(label);

        auto cameraIdChangeHandler = [camera_follow](const std::string& value) {
            uint32_t id = 0;
            if (editor::parseUint32(value, id)) {
                camera_follow->setCameraId(id);
            }
        };

        auto cameraIdUpdater = [camera_follow]() {
            if (!camera_follow->isValid()) {
                return std::string();
            }
            return editor::formatUint32(camera_follow->getCameraId());
        };

        std::vector<editor::EditorBlockLayoutData> camera_id_data = {
            { "camera id", editor::formatUint32(camera_follow->getCameraId()), cameraIdChangeHandler, cameraIdUpdater }
        };
        QHBoxLayout* camera_id_layout = editor::createEditorBlockLayout("", camera_id_data, engineObserver());
        layout->addLayout(camera_id_layout);

        layout->addStretch();

        widget->setLayout(layout);
    } else if (component->type() == "rotate") {
        auto rotate = std::dynamic_pointer_cast<RotateComponent>(component);

        widget = new editor::ComponentWidget;
        treeWidgetBuilderHelper()->subscribeOnActiveComponent(widget, component);

        QVBoxLayout* layout = new QVBoxLayout;
        layout->setSpacing(1);
        layout->setContentsMargins(0, 0, 0, 0);

        auto* label = new QLabel("Rotate");
        label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        layout->addWidget(label);

        auto degreesChangeHandler = [rotate](const std::string& value) {
            rotate->setDegrees(std::stof(value));
        };

        auto degreesUpdater = [rotate]() {
            if (!rotate->isValid()) {
                return std::string();
            }
            return editor::formatFloat(rotate->getDegrees());
        };

        std::vector<editor::EditorBlockLayoutData> degrees_data = {
            { "degrees", editor::formatFloat(rotate->getDegrees()), degreesChangeHandler, degreesUpdater }
        };
        QHBoxLayout* degrees_layout = editor::createEditorBlockLayout("", degrees_data, engineObserver());
        layout->addLayout(degrees_layout);

        std::vector<std::string> axes = { "X", "Y", "Z" };
        std::map<std::string, RotateComponent::Axis> axes_map = { {"X", RotateComponent::Axis::X},
                                                {"Y", RotateComponent::Axis::Y},
                                                {"Z", RotateComponent::Axis::Z}
                                              };
        std::function<void(const std::string&)> axis_handler = [rotate, axes_map](const std::string& value) {
            engine::Logger::info("RotateComponent::axisChangeHandler: " + value);
            rotate->setAxis(axes_map.at(value));
        };
        auto axis_widget = editor::createComboBoxWidget("Axis", axes, axis_handler);
        layout->addWidget(axis_widget);

        layout->addStretch();

        widget->setLayout(layout);
    }   
    return widget;
}
#endif

std::optional<std::unique_ptr<engine::Component>> EngineComponentBuilder::buildComponent(const std::string& type, const rapidjson::Value& component) const
{
    if (type == "move") {
        auto id = component["id"].GetUint();
        auto name = component["name"].GetString();
        auto owner_node = component["owner_node"].GetUint();
        auto owner_scene = component["owner_scene"].GetUint();

        auto move = component["move"].GetFloat();

        auto move_node_id = component["move_node_id"].GetUint();
        auto rest_node_id = component["rest_node_id"].GetUint();

        auto new_component = std::make_unique<MoveComponent>(id, name, owner_node, owner_scene);
        new_component->setSpeed(move);
        new_component->setMoveNodeId(move_node_id);
        new_component->setRestNodeId(rest_node_id);

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
    } else if (type == "rotate") {
        engine::Logger::info("EngineComponentBuilder::buildComponent: rotate");
        auto id = component["id"].GetUint();
        auto name = component["name"].GetString();
        auto owner_node = component["owner_node"].GetUint();
        auto owner_scene = component["owner_scene"].GetUint();

        auto new_component = std::make_unique<RotateComponent>(id, name, owner_node, owner_scene);
        std::string axis = component["axis"].GetString();
        if (axis == "x") {
            new_component->setAxis(RotateComponent::Axis::X);
        } else if (axis == "y") {
            new_component->setAxis(RotateComponent::Axis::Y);
        } else if (axis == "z") {
            new_component->setAxis(RotateComponent::Axis::Z);
        }

        auto degrees = component["degrees"].GetFloat();
        new_component->setDegrees(degrees);

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
    } else if (type == "rotate") {
        return std::make_unique<RotateComponent>(engine::generateUniqueId(), name, owner_node, owner_scene);
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
        component_json.AddMember("move_node_id", move->getMoveNodeId(), allocator);
        component_json.AddMember("rest_node_id", move->getRestNodeId(), allocator);
    } else if (component->type() == "camera_follow") {
        auto camera_follow = std::dynamic_pointer_cast<CameraFollowComponent>(component);

        component_json.AddMember("type", "camera_follow", allocator);
        component_json.AddMember("id", camera_follow->id(), allocator);
        component_json.AddMember("name", value_name, allocator);
        component_json.AddMember("owner_node", camera_follow->ownerNode(), allocator);
        component_json.AddMember("owner_scene", camera_follow->ownerScene(), allocator);
        component_json.AddMember("camera_id", camera_follow->getCameraId(), allocator);
    } else if (component->type() == "rotate") {
        auto rotate = std::dynamic_pointer_cast<RotateComponent>(component);

        component_json.AddMember("type", "rotate", allocator);
        component_json.AddMember("id", rotate->id(), allocator);
        component_json.AddMember("name", value_name, allocator);
        component_json.AddMember("owner_node", rotate->ownerNode(), allocator);
        component_json.AddMember("owner_scene", rotate->ownerScene(), allocator);
        component_json.AddMember("degrees", rotate->getDegrees(), allocator);
        component_json.AddMember("axis", rotate->getAxis() == RotateComponent::Axis::X ? "x" : rotate->getAxis() == RotateComponent::Axis::Y ? "y" : "z", allocator);
    }
}

auto EngineComponentBuilder::componentTypes() const -> const std::vector<std::string>&
{
    static const std::vector<std::string> types = {"move", "camera_follow", "rotate"};
    return types;
}
