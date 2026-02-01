#include "NodeTreeWidgetBuilder.h"

#include "editor/ComponentWidget.h"
#include "editor/EngineController.h"
#include "editor/NodeWidget.h"
#include "editor/SceneNodeTree.h"
#include "editor/TreeWidgetBuilderHelper.h"
#include "editor/Utils.h"

#include "engine/TransformComponent.h"
#include "engine/MeshComponent.h"
#include "engine/MaterialComponent.h"
#include "engine/CameraComponent.h"
#include "engine/FlipbookAnimationComponent.h"
#include "engine/MouseEventFilterComponent.h"
#include "engine/RenderScopeComponent.h"
#include "engine/RenderPassComponent.h"
#include "engine/LightSourceComponent.h"

#include "engine/Context.h"
#include "engine/Node.h"
#include "engine/Helpers.h"
#include "engine/Utils.h"
#include "engine/TextureStore.h"
#include "engine/ShaderStore.h"
#include "engine/MeshStore.h"
#include "engine/RenderPassStore.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGraphicsOpacityEffect>
#include <QComboBox>
#include <QPushButton>

#include <QtCore/qnamespace.h>
#include <string>
#include <functional>
#include <algorithm>
#include <glm/glm.hpp>

#include <limits>

namespace editor {

ComponentWidget* TreeWidgetBuilder::buildTransformWidget(const std::shared_ptr<engine::TransformComponent>& transform)
{
    ComponentWidget* transform_widget = new ComponentWidget;
    m_tree_widget_builder_helper->subscribeOnActiveComponent(transform_widget, transform);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);

    std::string name = "Transform name \"" + transform->name() + "\"";
    auto* label = new QLabel(QString::fromStdString(name));
    decorateLabel(label);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    layout->addWidget(label);

    auto positionXChangeHandler = [transform](const std::string& value) {
        float param = 0.0f;
        if (parseFloat(value, param)) {
            transform->setPosition({ param, transform->getPosition().y, transform->getPosition().z });
        }
    };
    auto positionYChangeHandler = [transform](const std::string& value) {
        float param = 0.0f;
        if (parseFloat(value, param)) {
            transform->setPosition({ transform->getPosition().x, param, transform->getPosition().z });
        }
    };
    auto positionZChangeHandler = [transform](const std::string& value) {
        float param = 0.0f;
        if (parseFloat(value, param)) {
            transform->setPosition({ transform->getPosition().x, transform->getPosition().y, param });
        }
    };

    auto positionXUpdater = [transform]() {
        if (!transform->isValid()) {
            return std::string();
        }
        return formatFloat(transform->getPosition().x);
    };
    auto positionYUpdater = [transform]() {
        if (!transform->isValid()) {
            return std::string();
        }
        return formatFloat(transform->getPosition().y);
    };
    auto positionZUpdater = [transform]() {
        if (!transform->isValid()) {
            return std::string();
        }
        return formatFloat(transform->getPosition().z);
    };

    std::vector<EditorBlockLayoutData> position_data = {
        { "X", formatFloat(transform->getPosition().x), positionXChangeHandler, positionXUpdater, false, false, {}, true, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max() },
        { "Y", formatFloat(transform->getPosition().y), positionYChangeHandler, positionYUpdater, false, false, {}, true, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max() },
        { "Z", formatFloat(transform->getPosition().z), positionZChangeHandler, positionZUpdater, false, false, {}, true, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max() },
    };
    auto position_layout = createEditorBlockLayout("Position", position_data, m_engine_controller);
    layout->addLayout(position_layout);

    auto rotationXChangeHandler = [transform](const std::string& value) {
        float param = 0.0f;
        if (parseFloat(value, param)) {
            transform->setRotation({ param, transform->getRotation().y, transform->getRotation().z });
        }
    };
    auto rotationYChangeHandler = [transform](const std::string& value) {
        float param = 0.0f;
        if (parseFloat(value, param)) {
            transform->setRotation({ transform->getRotation().x, param, transform->getRotation().z });
        }
    };
    auto rotationZChangeHandler = [transform](const std::string& value) {
        float param = 0.0f;
        if (parseFloat(value, param)) {
            transform->setRotation({ transform->getRotation().x, transform->getRotation().y, param });
        }
    };

    auto rotationXUpdater = [transform]() {
        if (!transform->isValid()) {
            return std::string();
        }
        return formatFloat(transform->getRotation().x);
    };
    auto rotationYUpdater = [transform]() {
        if (!transform->isValid()) {
            return std::string();
        }
        return formatFloat(transform->getRotation().y);
    };
    auto rotationZUpdater = [transform]() {
        if (!transform->isValid()) {
            return std::string();
        }
        return formatFloat(transform->getRotation().z);
    };

    std::vector<EditorBlockLayoutData> rotation_data = {
        { "X", formatFloat(transform->getRotation().x), rotationXChangeHandler, rotationXUpdater, false, false, {}, true, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max() },
        { "Y", formatFloat(transform->getRotation().y), rotationYChangeHandler, rotationYUpdater, false, false, {}, true, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max() },
        { "Z", formatFloat(transform->getRotation().z), rotationZChangeHandler, rotationZUpdater, false, false, {}, true, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max() },
    };
    auto rotation_layout = createEditorBlockLayout("Rotation", rotation_data, m_engine_controller);
    layout->addLayout(rotation_layout);

    auto scaleXChangeHandler = [transform](const std::string& value) {
        float param = 0.0f;
        if (parseFloat(value, param)) {
            transform->setScale({ param, transform->getScale().y, transform->getScale().z });
        }
    };
    auto scaleYChangeHandler = [transform](const std::string& value) {
        float param = 0.0f;
        if (parseFloat(value, param)) {
            transform->setScale({ transform->getScale().x, param, transform->getScale().z });
        }
    };
    auto scaleZChangeHandler = [transform](const std::string& value) {
        float param = 0.0f;
        if (parseFloat(value, param)) {
            transform->setScale({ transform->getScale().x, transform->getScale().y, param});
        }
    };

    auto scaleXUpdater = [transform]() {
        if (!transform->isValid()) {
            return std::string();
        }
        return formatFloat(transform->getScale().x);
    };
    auto scaleYUpdater = [transform]() {
        if (!transform->isValid()) {
            return std::string();
        }
        return formatFloat(transform->getScale().y);
    };
    auto scaleZUpdater = [transform]() {
        if (!transform->isValid()) {
            return std::string();
        }
        return formatFloat(transform->getScale().z);
    };

    std::vector<EditorBlockLayoutData> scale_data = {
        { "X", formatFloat(transform->getScale().x), scaleXChangeHandler, scaleXUpdater, false, false, {}, true, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max() },
        { "Y", formatFloat(transform->getScale().y), scaleYChangeHandler, scaleYUpdater, false, false, {}, true, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max() },
        { "Z", formatFloat(transform->getScale().z), scaleZChangeHandler, scaleZUpdater, false, false, {}, true, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max() },
    };
    auto scale_layout = createEditorBlockLayout("Scale", scale_data, m_engine_controller);
    layout->addLayout(scale_layout);

    layout->addStretch();

    transform_widget->setLayout(layout);

    return transform_widget;
}

ComponentWidget* TreeWidgetBuilder::buildMaterialWidget(const std::shared_ptr<engine::MaterialComponent>& material)
{
    ComponentWidget* material_widget = new ComponentWidget;
    m_tree_widget_builder_helper->subscribeOnActiveComponent(material_widget, material);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);

    std::string name = "Material name \"" + material->name() + "\"";
    auto* label = new QLabel(QString::fromStdString(name));
    decorateLabel(label);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    layout->addWidget(label);

    auto textureChangeHandler = [material](const std::string& value) {
        material->setTexture(value);
    };

    auto shaderChangeHandler = [material](const std::string& value) {
        material->setShader(value);
    };

    auto textureUpdater = [material]() {
        if (!material->isValid()) {
            return std::string();
        }
        return material->textureName();
    };

    auto shaderUpdater = [material]() {
        if (!material->isValid()) {
            return std::string();
        }
        return material->shaderName();
    };

    std::vector<EditorBlockLayoutData> texture_data = {
        { "texture", material->textureName(), textureChangeHandler, textureUpdater, true, true, material->context().lock()->textureStore->names() },
    };
    auto texture_layout = createEditorBlockLayout("Texture", texture_data, m_engine_controller);
    layout->addLayout(texture_layout);

    std::vector<EditorBlockLayoutData> shader_data = {
        { "shader", material->shaderName(), shaderChangeHandler, shaderUpdater, true, true, material->context().lock()->shaderStore->names() },
    };
    auto shader_layout = createEditorBlockLayout("Shader", shader_data, m_engine_controller);
    layout->addLayout(shader_layout);

    layout->addStretch();

    material_widget->setLayout(layout);

    return material_widget;
}

ComponentWidget* TreeWidgetBuilder::buildMeshWidget(const std::shared_ptr<engine::MeshComponent>& mesh)
{
    ComponentWidget* mesh_widget = new ComponentWidget;
    m_tree_widget_builder_helper->subscribeOnActiveComponent(mesh_widget, mesh);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);

    std::string name = "Mesh name \"" + mesh->name() + "\"";
    auto* label = new QLabel(QString::fromStdString(name));
    decorateLabel(label);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    layout->addWidget(label);

    auto meshChangeHandler = [mesh](const std::string& value) {
        mesh->setMesh(value);
    };

    auto meshUpdater = [mesh]() {
        if (!mesh->isValid()) {
            return std::string();
        }
        return mesh->meshName();
    };

    std::vector<EditorBlockLayoutData> mesh_data = {
        { "mesh", mesh->meshName(), meshChangeHandler, meshUpdater, false, true, mesh->context().lock()->meshStore->names() },
    };
    auto mesh_layout = createEditorBlockLayout("Mesh", mesh_data, m_engine_controller);
    layout->addLayout(mesh_layout);

    layout->addStretch();

    mesh_widget->setLayout(layout);

    return mesh_widget;
}

ComponentWidget* TreeWidgetBuilder::buildCameraWidget(const std::shared_ptr<engine::CameraComponent>& camera, QTreeWidgetItem* item)
{
    ComponentWidget* camera_widget = new ComponentWidget;
    m_tree_widget_builder_helper->subscribeOnActiveComponent(camera_widget, camera);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);

    std::string name = "Camera name \"" + camera->name() + "\"";
    auto* label = new QLabel(QString::fromStdString(name));
    decorateLabel(label);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    layout->addWidget(label);

    auto projectionTypeUpdater = [camera]() {
        return camera->projectionType() == engine::CameraComponent::ProjectionType::Orthographic ? "orthographic" : "perspective";
    };

    std::vector<std::string> projection_type_values = { "orthographic", "perspective" };
    std::map<std::string, engine::CameraComponent::ProjectionType> projection_type_map = {
        {"orthographic", engine::CameraComponent::ProjectionType::Orthographic},
        {"perspective", engine::CameraComponent::ProjectionType::Perspective} };
    engine::CameraComponent::ProjectionType projection_type = projection_type_map.at(projectionTypeUpdater());
    
    auto projectionTypeChangeHandler = [this, camera, projection_type_map, item](const std::string& value) {
        camera->setProjectionType(projection_type_map.at(value));
        m_scene_node_tree->rebuildComponentWidget(item);
    };

    std::vector<EditorBlockLayoutData> projection_type_data = {
        { "projection type", projection_type_values[static_cast<int>(projection_type)], projectionTypeChangeHandler, projectionTypeUpdater, false, true, projection_type_values }
    };
    QHBoxLayout* projection_type_layout = createEditorBlockLayout("Projection type", projection_type_data, m_engine_controller);
    layout->addLayout(projection_type_layout);

    if (camera->projectionType() == engine::CameraComponent::ProjectionType::Orthographic) {
        auto orthoLeftChangeHandler = [camera](const std::string& value) {
            float param = 0.0f;
            if (parseFloat(value, param)) {
                auto ortho = camera->getOrtho();
                ortho.left = param;
                camera->setOrtho(ortho);
            }
        };

        auto orthoRightChangeHandler = [camera](const std::string& value) {
            float param = 0.0f;
            if (parseFloat(value, param)) {
                auto ortho = camera->getOrtho();
                ortho.right = param;
                camera->setOrtho(ortho);
            }
        };

        auto orthoTopChangeHandler = [camera](const std::string& value) {
            float param = 0.0f;
            if (parseFloat(value, param)) {
                auto ortho = camera->getOrtho();
                ortho.top = param;
                camera->setOrtho(ortho);
            }
        };

        auto orthoBottomChangeHandler = [camera](const std::string& value) {
            float param = 0.0f;
            if (parseFloat(value, param)) {
                auto ortho = camera->getOrtho();
                ortho.bottom = param;
                camera->setOrtho(ortho);
            }
        };

        auto orthoNearChangeHandler = [camera](const std::string& value) {
            float param = 0.0f;
            if (parseFloat(value, param)) {
                camera->setNear(param);
            }
        };

        auto orthoFarChangeHandler = [camera](const std::string& value) {
            float param = 0.0f;
            if (parseFloat(value, param)) {
                camera->setFar(param);
            }
        };

        auto orthoLeftUpdater = [camera]() {
            auto ortho = camera->getOrtho();
            return formatFloat(ortho.left);
        };

        auto orthoRightUpdater = [camera]() {
            auto ortho = camera->getOrtho();
            return formatFloat(ortho.right);
        };

        auto orthoTopUpdater = [camera]() {
            auto ortho = camera->getOrtho();
            return formatFloat(ortho.top);
        };

        auto orthoBottomUpdater = [camera]() {
            auto ortho = camera->getOrtho();
            return formatFloat(ortho.bottom);
        };

        auto orthoNearUpdater = [camera]() {
            return formatFloat(camera->getNear());
        };

        auto orthoFarUpdater = [camera]() {
            return formatFloat(camera->getFar());
        };

        auto ortho = camera->getOrtho();

        std::vector<EditorBlockLayoutData> ortho_data = {};
        ortho_data.push_back({ "left", formatFloat(ortho.left), orthoLeftChangeHandler, orthoLeftUpdater, false, false, {}, true, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max() });
        ortho_data.push_back({ "right", formatFloat(ortho.right), orthoRightChangeHandler, orthoRightUpdater, false, false, {}, true, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max() });
        ortho_data.push_back({ "top", formatFloat(ortho.top), orthoTopChangeHandler, orthoTopUpdater, false, false, {}, true, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max() });
        ortho_data.push_back({ "bottom", formatFloat(ortho.bottom), orthoBottomChangeHandler, orthoBottomUpdater, false, false, {}, true, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max() });
        ortho_data.push_back({ "near", formatFloat(camera->getNear()), orthoNearChangeHandler, orthoNearUpdater, false, false, {}, true, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max() });
        ortho_data.push_back({ "far", formatFloat(camera->getFar()), orthoFarChangeHandler, orthoFarUpdater, false, false, {}, true, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max() });

        QHBoxLayout* ortho_layout = createEditorBlockLayout("Ortho", ortho_data, m_engine_controller);
        layout->addLayout(ortho_layout);
    } else if (camera->projectionType() == engine::CameraComponent::ProjectionType::Perspective) {
        auto fovChangeHandler = [camera](const std::string& value) {
            float param = 0.0f;
            if (parseFloat(value, param)) {
                camera->setFov(param);
            }
        };

        auto fovUpdater = [camera]() {
            if (!camera->isValid()) {
                return std::string();
            }
            return formatFloat(camera->getFov());
        };
    
        std::vector<EditorBlockLayoutData> fov_data = {
            { "fov", formatFloat(camera->getFov()), fovChangeHandler, fovUpdater, false, false, {}, true, 0.0f, 180.0f }
        };
        QHBoxLayout* fov_layout = createEditorBlockLayout("Fov", fov_data, m_engine_controller);
        layout->addLayout(fov_layout);

        auto nearChangeHandler = [camera](const std::string& value) {
            float param = 0.0f;
            if (parseFloat(value, param)) {
                camera->setNear(param);
            }
        };
        
        auto nearUpdater = [camera]() {
            if (!camera->isValid()) {
                return std::string();
            }
            return formatFloat(camera->getNear());
        };
        std::vector<EditorBlockLayoutData> near_data = {
            { "near", formatFloat(camera->getNear()), nearChangeHandler, nearUpdater, false, false, {}, true, 0.0f, std::numeric_limits<float>::max() }
        };
        QHBoxLayout* near_layout = createEditorBlockLayout("Near", near_data, m_engine_controller);
        layout->addLayout(near_layout);

        auto farChangeHandler = [camera](const std::string& value) {
            float param = 0.0f;
            if (parseFloat(value, param)) {
                camera->setFar(param);
            }
        };
        auto farUpdater = [camera]() {
            if (!camera->isValid()) {
                return std::string();
            }
            return formatFloat(camera->getFar());
        };
        std::vector<EditorBlockLayoutData> far_data = {
            { "far", formatFloat(camera->getFar()), farChangeHandler, farUpdater, false, false, {}, true, 0.0f, std::numeric_limits<float>::max() }
        };
        QHBoxLayout* far_layout = createEditorBlockLayout("Far", far_data, m_engine_controller);
        layout->addLayout(far_layout);

        auto aspectChangeHandler = [camera](const std::string& value) {
            float param = 0.0f;
            if (parseFloat(value, param)) {
                camera->setAspect(param);
            }
        };
        auto aspectUpdater = [camera]() {
            if (!camera->isValid()) {
                return std::string();
            }
            return formatFloat(camera->getAspect());
        };
        std::vector<EditorBlockLayoutData> aspect_data = {
            { "aspect", formatFloat(camera->getAspect()), aspectChangeHandler, aspectUpdater, false, false, {}, true, 0.0f, std::numeric_limits<float>::max() }
        };
        QHBoxLayout* aspect_layout = createEditorBlockLayout("Aspect", aspect_data, m_engine_controller);
        layout->addLayout(aspect_layout);
    }

    auto yawChangeHandler = [camera](const std::string& value) {
        float param = 0.0f;
        if (parseFloat(value, param)) {
            camera->setYaw(param);
        }
    };

    auto yawUpdater = [camera]() {
        if (!camera->isValid()) {
            return std::string();
        }
        return formatFloat(camera->getYaw());
    };

    std::vector<EditorBlockLayoutData> yaw_data = {
        { "yaw", formatFloat(camera->getYaw()), yawChangeHandler, yawUpdater, false, false, {}, true, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max() }
    };

    QHBoxLayout* yaw_layout = createEditorBlockLayout("Yaw", yaw_data, m_engine_controller);
    layout->addLayout(yaw_layout);

    auto pitchChangeHandler = [camera](const std::string& value) {
        float param = 0.0f;
        if (parseFloat(value, param)) {
            camera->setPitch(param);
        }
    };

    auto pitchUpdater = [camera]() {
        if (!camera->isValid()) {
            return std::string();
        }
        return formatFloat(camera->getPitch());
    };

    std::vector<EditorBlockLayoutData> pitch_data = {
        { "pitch", formatFloat(camera->getPitch()), pitchChangeHandler, pitchUpdater, false, false, {}, true, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max() }
    };

    QHBoxLayout* pitch_layout = createEditorBlockLayout("Pitch", pitch_data, m_engine_controller);
    layout->addLayout(pitch_layout);

    layout->addStretch();

    camera_widget->setLayout(layout);

    return camera_widget;
}

ComponentWidget* TreeWidgetBuilder::buildFlipbookAnimationWidget(const std::shared_ptr<engine::FlipbookAnimationComponent>& animation, QTreeWidgetItem* item)
{
    ComponentWidget* flipbook_widget = new ComponentWidget;
    m_tree_widget_builder_helper->subscribeOnActiveComponent(flipbook_widget, animation);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);

    std::string name = "Flipbook Animation name \"" + animation->name() + "\"";
    auto* label = new QLabel(QString::fromStdString(name));
    decorateLabel(label);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    layout->addWidget(label);

    auto materials_ids = animation->materialIds();

    auto context = animation->context().lock();
    if (!context) {
        return nullptr;
    }

    auto scene = context->sceneStore->get(animation->ownerScene());
    if (!scene.has_value()) {
        return nullptr;
    }

    auto updateTimeChangeHandler = [animation](const std::string& value) {
        float param = 0.0f;
        if (parseFloat(value, param)) {
            animation->setUpdateTime(param);
        }
    };

    auto updateTimeUpdater = [animation]() {
        if (!animation->isValid()) {
            return std::string();
        }
        return formatFloat(animation->updateTime());
    };

    std::vector<EditorBlockLayoutData> update_time_data = {
        { "update time", formatFloat(animation->updateTime()), updateTimeChangeHandler, updateTimeUpdater, false, false, {}, true, 0.0f, std::numeric_limits<float>::max() }
    };
    QHBoxLayout* update_time_layout = createEditorBlockLayout("Update time", update_time_data, m_engine_controller);
    layout->addLayout(update_time_layout);

    std::shared_ptr<std::vector<QHBoxLayout*>> material_layouts(new std::vector<QHBoxLayout*>);
    for (size_t i = 0; i < materials_ids.size(); ++i) {
        material_layouts->push_back(new QHBoxLayout);
    }

    std::vector<std::string> buttons_names = {"Add", "Delete"};

    auto add_handler = [this, animation, item]() {
        animation->addMaterial(engine::generateUniqueId());
        m_scene_node_tree->rebuildComponentWidget(item);
    };

    auto delete_handler = [this, animation, item]() {
        auto materials_ids = animation->materialIds();

        if (materials_ids.empty()) {
            return;
        }

        animation->removeMaterial(materials_ids.back());

        m_scene_node_tree->rebuildComponentWidget(item);
    };

    layout->addWidget(createButtonLineWidget(buttons_names, { add_handler, delete_handler }));

    for (size_t i = 0; i < materials_ids.size(); ++i) {
        auto material_id = materials_ids[i];
        auto material = scene.value()->getComponent(material_id);

        std::string init_value;
        if (material.has_value()) {
            auto material_value = std::dynamic_pointer_cast<engine::MaterialComponent>(material.value());
            init_value = material_value->name();
        }

        auto materialChangeHandler = [animation, i](const std::string& value) {
            uint32_t id = 0;
            if (!parseUint32(value, id)) {
                return;
            }

            auto material = engine::getComponentById<engine::MaterialComponent>(animation, id);
            if (!material) {
                return;
            }

            auto materials_ids = animation->materialIds();
            animation->replaceMaterial(materials_ids[i], material->id());
        };

        auto materialUpdater = [i, animation]() {
            auto materials_ids = animation->materialIds();
            auto material = engine::getComponentById<engine::MaterialComponent>(animation, materials_ids[i]);
            if (!material) {
                return std::string();
            }

            return formatUint32(material->id());
        };

        std::vector<EditorBlockLayoutData> animation_data = {
            { "animation", init_value, materialChangeHandler, materialUpdater, true }
        };
        setupEditorBlockLayout(material_layouts->at(i), "", animation_data, m_engine_controller);
        layout->addLayout(material_layouts->at(i));
    }

    layout->addStretch();

    flipbook_widget->setLayout(layout);

    return flipbook_widget;
}

ComponentWidget* TreeWidgetBuilder::buildMouseEventFilterWidget(const std::shared_ptr<engine::MouseEventFilterComponent>& mouse_event_filter)
{
    ComponentWidget* mouse_event_filter_widget = new ComponentWidget;
    m_tree_widget_builder_helper->subscribeOnActiveComponent(mouse_event_filter_widget, mouse_event_filter);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);

    std::string name = "Mouse Event Filter name \"" + mouse_event_filter->name() + "\"";
    auto* label = new QLabel(QString::fromStdString(name));
    decorateLabel(label);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    layout->addWidget(label);

    std::vector<std::string> keys = { "MouseButtonLeft", "MouseButtonRight", "MouseButtonMiddle" };
    std::map<std::string, int> keys_map = { {"MouseButtonLeft", 0},
                                            {"MouseButtonRight", 1},
                                            {"MouseButtonMiddle", 2}
                                          };
    std::function<void(const std::string&)> key_handler = [mouse_event_filter, keys_map](const std::string& value) {
        mouse_event_filter->setKey(keys_map.at(value));
    };
    int key = mouse_event_filter->key();
    auto key_widget = createComboBoxWidget("Key", keys, key_handler, keys[key]);
    layout->addWidget(key_widget);

    std::vector<std::string> actions = { "Press", "Release", "Repeat" };
    std::map<std::string, int> actions_map = { {"Press", 1},
                                               {"Release", 0},
                                               {"Repeat", 2} };
    std::function<void(const std::string&)> action_handler = [mouse_event_filter, actions_map](const std::string& value) {
        mouse_event_filter->setAction(actions_map.at(value));
    };
    bool action = mouse_event_filter->action();
    auto action_widget = createComboBoxWidget("Action", actions, action_handler, action == 1 ? "Press" : action == 0 ? "Release" : "Repeat");
    layout->addWidget(action_widget);

    layout->addStretch();

    mouse_event_filter_widget->setLayout(layout);

    return mouse_event_filter_widget;
}

ComponentWidget* TreeWidgetBuilder::buildRenderScopeWidget(const std::shared_ptr<engine::RenderScopeComponent>& render_scope, QTreeWidgetItem* item)
{
    ComponentWidget* render_scope_widget = new ComponentWidget;
    m_tree_widget_builder_helper->subscribeOnActiveComponent(render_scope_widget, render_scope);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);

    std::string name = "Render scope name \"" + render_scope->name() + "\"";
    auto* label = new QLabel(QString::fromStdString(name));
    decorateLabel(label);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    layout->addWidget(label);

    std::vector<std::string> sprite_values = { "Yes", "No" };
    std::map<std::string, bool> sprite_map = { {"Yes", true},
                                               {"No", false} };
    auto spriteChangeHandler = [render_scope, sprite_map](const std::string& value) {
        render_scope->setIsSprite(sprite_map.at(value));
    };
    auto spriteUpdater = [render_scope]() {
        if (!render_scope->isValid()) {
            return std::string();
        }
        return render_scope->isSprite() ? std::string("Yes") : std::string("No");
    };

    std::vector<EditorBlockLayoutData> sprite_data = {
        { "sprite", render_scope->isSprite() ? std::string("Yes") : std::string("No"), spriteChangeHandler, spriteUpdater, false, true, sprite_values }
    };
    auto sprite_layout = createEditorBlockLayout("Sprite", sprite_data, m_engine_controller);
    layout->addLayout(sprite_layout);

    auto make_sorted_uniform_names = [render_scope]() {
        std::vector<std::string> names;
        for (const auto& uniform : render_scope->renderData().uniforms) {
            names.push_back(uniform.first);
        }
        std::sort(names.begin(), names.end());
        return names;
    };

    std::vector<std::string> uniform_type_values = {
        "Float", "Double", "Int", "UInt", "Bool",
        "Vec2", "Vec3", "Vec4", "Mat2", "Mat3", "Mat4"
    };
    auto selected_uniform_type = std::make_shared<std::string>(uniform_type_values.front());
    auto new_uniform_name = std::make_shared<std::string>();

    auto make_default_uniform = [](const std::string& type) -> std::any {
        if (type == "Float") return 0.0f;
        if (type == "Double") return 0.0;
        if (type == "Int") return 0;
        if (type == "UInt") return static_cast<uint32_t>(0);
        if (type == "Bool") return false;
        if (type == "Vec2") return glm::vec2(0.0f);
        if (type == "Vec3") return glm::vec3(0.0f);
        if (type == "Vec4") return glm::vec4(0.0f);
        if (type == "Mat2") return glm::mat2(1.0f);
        if (type == "Mat3") return glm::mat3(1.0f);
        return glm::mat4(1.0f);
    };

    auto add_handler = [this, render_scope, item, selected_uniform_type, new_uniform_name, make_default_uniform]() {
        if (new_uniform_name->empty()) {
            return;
        }
        auto data = render_scope->renderData();
        if (data.uniforms.find(*new_uniform_name) != data.uniforms.end()) {
            return;
        }
        render_scope->addRenderData(*new_uniform_name, make_default_uniform(*selected_uniform_type));
        m_scene_node_tree->rebuildComponentWidget(item);
    };

    auto uniform_add_widget = new ComponentWidget;
    auto* uniform_add_layout = new QHBoxLayout;
    uniform_add_layout->setSpacing(1);
    uniform_add_layout->setContentsMargins(0, 0, 0, 0);
    uniform_add_layout->addWidget(new QLabel("Type"));

    auto* uniform_type_combo = new QComboBox;
    for (const auto& value : uniform_type_values) {
        uniform_type_combo->addItem(value.c_str());
    }
    uniform_type_combo->setCurrentText(selected_uniform_type->c_str());
    QObject::connect(uniform_type_combo, &QComboBox::currentTextChanged, [selected_uniform_type](const QString& value) {
        *selected_uniform_type = value.toStdString();
    });
    uniform_add_layout->addWidget(uniform_type_combo);

    uniform_add_layout->addWidget(new QLabel("Name"));
    auto* uniform_name_edit = new QLineEdit;
    QObject::connect(uniform_name_edit, &QLineEdit::textChanged, [new_uniform_name](const QString& value) {
        *new_uniform_name = value.toStdString();
    });
    uniform_add_layout->addWidget(uniform_name_edit);

    auto* add_button = new QPushButton("Add");
    QObject::connect(add_button, &QPushButton::clicked, [add_handler]() {
        add_handler();
    });
    uniform_add_layout->addWidget(add_button);

    uniform_add_widget->setLayout(uniform_add_layout);
    layout->addWidget(uniform_add_widget);

    auto uniform_names = make_sorted_uniform_names();
    auto selected_uniform_name = std::make_shared<std::string>(uniform_names.empty() ? std::string() : uniform_names.front());

    auto delete_handler = [this, render_scope, item, selected_uniform_name]() {
        if (selected_uniform_name->empty()) {
            return;
        }
        auto data = render_scope->renderData();
        if (data.uniforms.erase(*selected_uniform_name) == 0) {
            return;
        }
        render_scope->setRenderData(data);
        m_scene_node_tree->rebuildComponentWidget(item);
    };

    auto uniform_delete_widget = new ComponentWidget;
    auto* uniform_delete_layout = new QHBoxLayout;
    uniform_delete_layout->setSpacing(1);
    uniform_delete_layout->setContentsMargins(0, 0, 0, 0);
    uniform_delete_layout->addWidget(new QLabel("Uniform"));

    auto* uniform_name_combo = new QComboBox;
    for (const auto& name : uniform_names) {
        uniform_name_combo->addItem(name.c_str());
    }
    uniform_name_combo->setCurrentText(selected_uniform_name->c_str());
    QObject::connect(uniform_name_combo, &QComboBox::currentTextChanged, [selected_uniform_name](const QString& value) {
        *selected_uniform_name = value.toStdString();
    });
    uniform_delete_layout->addWidget(uniform_name_combo);

    auto* delete_button = new QPushButton("Delete");
    QObject::connect(delete_button, &QPushButton::clicked, [delete_handler]() {
        delete_handler();
    });
    uniform_delete_layout->addWidget(delete_button);

    uniform_delete_widget->setLayout(uniform_delete_layout);
    layout->addWidget(uniform_delete_widget);

    const auto& uniforms = render_scope->renderData().uniforms;
    if (!uniforms.empty()) {
        auto* uniforms_label = new QLabel("Uniforms");
        decorateLabel(uniforms_label);
        uniforms_label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        layout->addWidget(uniforms_label);
    }

    auto get_uniform_value = [render_scope](const std::string& name, const std::function<std::string(const std::any&)>& getter) {
        if (!render_scope->isValid()) {
            return std::string();
        }
        auto data = render_scope->renderData();
        auto it = data.uniforms.find(name);
        if (it == data.uniforms.end()) {
            return std::string();
        }
        try {
            return getter(it->second);
        } catch (...) {
            return std::string();
        }
    };

    auto set_uniform_value = [render_scope](const std::string& name, const std::function<bool(std::any&)>& setter) {
        auto data = render_scope->renderData();
        auto it = data.uniforms.find(name);
        if (it == data.uniforms.end()) {
            return;
        }
        if (!setter(it->second)) {
            return;
        }
        render_scope->setRenderData(data);
    };

    auto add_single_uniform = [this, layout, get_uniform_value, set_uniform_value](const std::string& name,
                                                                                   const std::function<bool(const std::string&, std::any&)>& apply,
                                                                                   const std::function<std::string(const std::any&)>& format) {
        auto changeHandler = [set_uniform_value, name, apply](const std::string& value) {
            set_uniform_value(name, [apply, value](std::any& current) {
                return apply(value, current);
            });
        };
        auto updateHandler = [get_uniform_value, name, format]() {
            return get_uniform_value(name, format);
        };
        std::vector<EditorBlockLayoutData> data = {
            { name, updateHandler(), changeHandler, updateHandler }
        };
        layout->addLayout(createEditorBlockLayout("Uniform", data, m_engine_controller));
    };

    auto add_combo_uniform = [this, layout, get_uniform_value, set_uniform_value](const std::string& name,
                                                                                  const std::vector<std::string>& values,
                                                                                  const std::function<std::string(const std::any&)>& format,
                                                                                  const std::function<bool(const std::string&, std::any&)>& apply) {
        auto changeHandler = [set_uniform_value, name, apply](const std::string& value) {
            set_uniform_value(name, [apply, value](std::any& current) {
                return apply(value, current);
            });
        };
        auto updateHandler = [get_uniform_value, name, format]() {
            return get_uniform_value(name, format);
        };
        std::vector<EditorBlockLayoutData> data = {
            { name, updateHandler(), changeHandler, updateHandler, false, true, values }
        };
        layout->addLayout(createEditorBlockLayout("Uniform", data, m_engine_controller));
    };

    auto add_vec_uniform = [this, layout, get_uniform_value, set_uniform_value](const std::string& name,
                                                                                const std::vector<std::string>& labels,
                                                                                const std::function<float(const std::any&, int)>& get_comp,
                                                                                const std::function<bool(std::any&, int, float)>& set_comp) {
        std::vector<EditorBlockLayoutData> data;
        for (int index = 0; index < static_cast<int>(labels.size()); ++index) {
            auto changeHandler = [set_uniform_value, name, index, set_comp](const std::string& value) {
                float param = 0.0f;
                if (!parseFloat(value, param)) {
                    return;
                }
                set_uniform_value(name, [set_comp, index, param](std::any& current) {
                    return set_comp(current, index, param);
                });
            };
            auto updateHandler = [get_uniform_value, name, index, get_comp]() {
                return get_uniform_value(name, [get_comp, index](const std::any& value) {
                    return formatFloat(get_comp(value, index));
                });
            };
            data.push_back({ labels[index], updateHandler(), changeHandler, updateHandler, false, false, {}, true, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max() });
        }
        layout->addLayout(createEditorBlockLayout("Uniform " + name, data, m_engine_controller));
    };

    auto add_mat_uniform = [this, layout, get_uniform_value, set_uniform_value](const std::string& name, int size,
                                                                                const std::function<float(const std::any&, int, int)>& get_cell,
                                                                                const std::function<bool(std::any&, int, int, float)>& set_cell) {
        for (int row = 0; row < size; ++row) {
            std::vector<EditorBlockLayoutData> data;
            for (int col = 0; col < size; ++col) {
                auto changeHandler = [set_uniform_value, name, row, col, set_cell](const std::string& value) {
                    float param = 0.0f;
                    if (!parseFloat(value, param)) {
                        return;
                    }
                    set_uniform_value(name, [set_cell, row, col, param](std::any& current) {
                        return set_cell(current, row, col, param);
                    });
                };
                auto updateHandler = [get_uniform_value, name, row, col, get_cell]() {
                    return get_uniform_value(name, [get_cell, row, col](const std::any& value) {
                        return formatFloat(get_cell(value, row, col));
                    });
                };
                data.push_back({ std::to_string(col), updateHandler(), changeHandler, updateHandler, false, false, {}, true, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max() });
            }
            layout->addLayout(createEditorBlockLayout(row == 0 ? "Uniform " + name : "", data, m_engine_controller));
        }
    };

    for (const auto& uniform_name : make_sorted_uniform_names()) {
        auto data = render_scope->renderData();
        auto it = data.uniforms.find(uniform_name);
        if (it == data.uniforms.end()) {
            continue;
        }
        const std::type_info& uniform_type = it->second.type();

        if (uniform_type == typeid(float)) {
            add_single_uniform(uniform_name,
                [](const std::string& value, std::any& current) {
                    float param = 0.0f;
                    if (!parseFloat(value, param)) {
                        return false;
                    }
                    current = param;
                    return true;
                },
                [](const std::any& value) {
                    return formatFloat(std::any_cast<float>(value));
                });
        } else if (uniform_type == typeid(double)) {
            add_single_uniform(uniform_name,
                [](const std::string& value, std::any& current) {
                    float param = 0.0f;
                    if (!parseFloat(value, param)) {
                        return false;
                    }
                    current = static_cast<double>(param);
                    return true;
                },
                [](const std::any& value) {
                    return formatFloat(static_cast<float>(std::any_cast<double>(value)));
                });
        } else if (uniform_type == typeid(int)) {
            add_single_uniform(uniform_name,
                [](const std::string& value, std::any& current) {
                    float param = 0.0f;
                    if (!parseFloat(value, param)) {
                        return false;
                    }
                    current = static_cast<int>(param);
                    return true;
                },
                [](const std::any& value) {
                    return std::to_string(std::any_cast<int>(value));
                });
        } else if (uniform_type == typeid(uint32_t)) {
            add_single_uniform(uniform_name,
                [](const std::string& value, std::any& current) {
                    uint32_t param = 0;
                    if (!parseUint32(value, param)) {
                        return false;
                    }
                    current = param;
                    return true;
                },
                [](const std::any& value) {
                    return formatUint32(std::any_cast<uint32_t>(value));
                });
        } else if (uniform_type == typeid(bool)) {
            add_combo_uniform(uniform_name,
                { "True", "False" },
                [](const std::any& value) {
                    return std::any_cast<bool>(value) ? "True" : "False";
                },
                [](const std::string& value, std::any& current) {
                    current = (value == "True");
                    return true;
                });
        } else if (uniform_type == typeid(glm::vec2)) {
            add_vec_uniform(uniform_name, { "X", "Y" },
                [](const std::any& value, int index) {
                    auto vec = std::any_cast<glm::vec2>(value);
                    return index == 0 ? vec.x : vec.y;
                },
                [](std::any& current, int index, float param) {
                    try {
                        auto vec = std::any_cast<glm::vec2>(current);
                        if (index == 0) vec.x = param;
                        if (index == 1) vec.y = param;
                        current = vec;
                        return true;
                    } catch (...) {
                        return false;
                    }
                });
        } else if (uniform_type == typeid(glm::vec3)) {
            add_vec_uniform(uniform_name, { "X", "Y", "Z" },
                [](const std::any& value, int index) {
                    auto vec = std::any_cast<glm::vec3>(value);
                    if (index == 0) return vec.x;
                    if (index == 1) return vec.y;
                    return vec.z;
                },
                [](std::any& current, int index, float param) {
                    try {
                        auto vec = std::any_cast<glm::vec3>(current);
                        if (index == 0) vec.x = param;
                        if (index == 1) vec.y = param;
                        if (index == 2) vec.z = param;
                        current = vec;
                        return true;
                    } catch (...) {
                        return false;
                    }
                });
        } else if (uniform_type == typeid(glm::vec4)) {
            add_vec_uniform(uniform_name, { "X", "Y", "Z", "W" },
                [](const std::any& value, int index) {
                    auto vec = std::any_cast<glm::vec4>(value);
                    if (index == 0) return vec.x;
                    if (index == 1) return vec.y;
                    if (index == 2) return vec.z;
                    return vec.w;
                },
                [](std::any& current, int index, float param) {
                    try {
                        auto vec = std::any_cast<glm::vec4>(current);
                        if (index == 0) vec.x = param;
                        if (index == 1) vec.y = param;
                        if (index == 2) vec.z = param;
                        if (index == 3) vec.w = param;
                        current = vec;
                        return true;
                    } catch (...) {
                        return false;
                    }
                });
        } else if (uniform_type == typeid(glm::mat2)) {
            add_mat_uniform(uniform_name, 2,
                [](const std::any& value, int row, int col) {
                    auto mat = std::any_cast<glm::mat2>(value);
                    return mat[col][row];
                },
                [](std::any& current, int row, int col, float param) {
                    try {
                        auto mat = std::any_cast<glm::mat2>(current);
                        mat[col][row] = param;
                        current = mat;
                        return true;
                    } catch (...) {
                        return false;
                    }
                });
        } else if (uniform_type == typeid(glm::mat3)) {
            add_mat_uniform(uniform_name, 3,
                [](const std::any& value, int row, int col) {
                    auto mat = std::any_cast<glm::mat3>(value);
                    return mat[col][row];
                },
                [](std::any& current, int row, int col, float param) {
                    try {
                        auto mat = std::any_cast<glm::mat3>(current);
                        mat[col][row] = param;
                        current = mat;
                        return true;
                    } catch (...) {
                        return false;
                    }
                });
        } else if (uniform_type == typeid(glm::mat4)) {
            add_mat_uniform(uniform_name, 4,
                [](const std::any& value, int row, int col) {
                    auto mat = std::any_cast<glm::mat4>(value);
                    return mat[col][row];
                },
                [](std::any& current, int row, int col, float param) {
                    try {
                        auto mat = std::any_cast<glm::mat4>(current);
                        mat[col][row] = param;
                        current = mat;
                        return true;
                    } catch (...) {
                        return false;
                    }
                });
        }
    }

    layout->addStretch();

    render_scope_widget->setLayout(layout);

    return render_scope_widget;
}

ComponentWidget* TreeWidgetBuilder::buildRenderPassWidget(const std::shared_ptr<engine::RenderPassComponent>& render_pass, QTreeWidgetItem* item)
{
    ComponentWidget* render_pass_widget = new ComponentWidget;
    m_tree_widget_builder_helper->subscribeOnActiveComponent(render_pass_widget, render_pass);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);

    std::string name = "Render pass name \"" + render_pass->name() + "\"";
    auto* label = new QLabel(QString::fromStdString(name));
    decorateLabel(label);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    layout->addWidget(label);

    auto render_pass_changeHandler = [render_pass](const std::string& value) {
        render_pass->setRenderPassName(value);
    };

    auto render_pass_updater = [render_pass]() {
        if (!render_pass->isValid()) {
            return std::string();
        }
        return render_pass->renderPassName();
    };

    std::vector<EditorBlockLayoutData> render_pass_data = {
        { "render_pass", render_pass->renderPassName(), render_pass_changeHandler, render_pass_updater, false, true, render_pass->context().lock()->renderPassStore->names() },
    };
    auto render_pass_layout = createEditorBlockLayout("Render pass", render_pass_data, m_engine_controller);
    layout->addLayout(render_pass_layout);

    layout->addStretch();

    render_pass_widget->setLayout(layout);

    return render_pass_widget;
}

ComponentWidget* TreeWidgetBuilder::buildLightSourceWidget(const std::shared_ptr<engine::LightSourceComponent>& light_source)
{
    ComponentWidget* light_source_widget = new ComponentWidget;
    m_tree_widget_builder_helper->subscribeOnActiveComponent(light_source_widget, light_source);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);

    std::string name = "Light source name \"" + light_source->name() + "\"";
    auto* label = new QLabel(QString::fromStdString(name));
    decorateLabel(label);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    layout->addWidget(label);

    auto colorRedChangeHandler = [light_source](const std::string& value) {
        float param = 0.0f;
        if (parseFloat(value, param)) {
            auto color = light_source->color();
            color.x = param;
            light_source->setColor(color);
        }
    };
    auto colorGreenChangeHandler = [light_source](const std::string& value) {
        float param = 0.0f;
        if (parseFloat(value, param)) {
            auto color = light_source->color();
            color.y = param;
            light_source->setColor(color);
        }
    };
    auto colorBlueChangeHandler = [light_source](const std::string& value) {
        float param = 0.0f;
        if (parseFloat(value, param)) {
            auto color = light_source->color();
            color.z = param;
            light_source->setColor(color);
        }
    };

    auto colorRedUpdater = [light_source]() {
        if (!light_source->isValid()) {
            return std::string();
        }
        return formatFloat(light_source->color().x);
    };
    auto colorGreenUpdater = [light_source]() {
        if (!light_source->isValid()) {
            return std::string();
        }
        return formatFloat(light_source->color().y);
    };
    auto colorBlueUpdater = [light_source]() {
        if (!light_source->isValid()) {
            return std::string();
        }
        return formatFloat(light_source->color().z);
    };

    std::vector<EditorBlockLayoutData> color_data = {
        { "Red", formatFloat(light_source->color().x), colorRedChangeHandler, colorRedUpdater, false, false, {}, true, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max() },
        { "Green", formatFloat(light_source->color().y), colorGreenChangeHandler, colorGreenUpdater, false, false, {}, true, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max() },
        { "Blue", formatFloat(light_source->color().z), colorBlueChangeHandler, colorBlueUpdater, false, false, {}, true, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max() },
    };
    auto color_layout = createEditorBlockLayout("Color", color_data, m_engine_controller);
    layout->addLayout(color_layout);

    auto intensityChangeHandler = [light_source](const std::string& value) {
        float param = 0.0f;
        if (parseFloat(value, param)) {
            light_source->setIntensity(param);
        }
    };
    auto intensityUpdater = [light_source]() {
        if (!light_source->isValid()) {
            return std::string();
        }
        return formatFloat(light_source->intensity());
    };
    std::vector<EditorBlockLayoutData> intensity_data = {
        { "intensity", formatFloat(light_source->intensity()), intensityChangeHandler, intensityUpdater, false, false, {}, true, 0.0f, std::numeric_limits<float>::max() },
    };
    auto intensity_layout = createEditorBlockLayout("Light source", intensity_data, m_engine_controller);
    layout->addLayout(intensity_layout);

    layout->addStretch();

    light_source_widget->setLayout(layout);

    return light_source_widget;
}

void TreeWidgetBuilder::decorateLabel(QLabel* label)
{
    label->setStyleSheet(
        "color:rgba(240, 185, 21, 0.81);"
        "font-weight: bold;"
        "font-size: 15px;"
    );
}

TreeWidgetBuilder::TreeWidgetBuilder(SceneNodeTree* scene_node_tree, const std::shared_ptr<EngineController>& engine_controller) :
    m_scene_node_tree(scene_node_tree),
    m_engine_controller(engine_controller),
    m_tree_widget_builder_helper(std::make_unique<TreeWidgetBuilderHelper>(engine_controller))
{

}

auto TreeWidgetBuilder::buildWidgetForNode(const std::shared_ptr<engine::Node>& node) -> std::optional<NodeWidget*>
{
    NodeWidget* new_widget = new NodeWidget();
    new_widget->setAttribute(Qt::WA_StyledBackground, true);
    new_widget->setStyleSheet(
        "background-color: #2b2f36;"
        "border: 1px solid #4a5664;"
        "border-radius: 4px;"
    );
    
    auto label = new QLabel(new_widget);
    label->setText(QString::fromStdString(node->name()));
    label->setStyleSheet(
        "color: #e8edf2;"
        "font-weight: bold;"
    );

    m_tree_widget_builder_helper->subscribeOnActiveNode(new_widget, node);

    return new_widget;
}

auto TreeWidgetBuilder::buildWidgetForComponent(std::shared_ptr<engine::Component> component, QTreeWidgetItem* item) -> std::optional<ComponentWidget*>
{
    if (component->type() == "material") {
        return buildMaterialWidget(std::dynamic_pointer_cast<engine::MaterialComponent>(component));
    } else if (component->type() == "mesh") {
        return buildMeshWidget(std::dynamic_pointer_cast<engine::MeshComponent>(component));
    } else if (component->type() == "camera") {
        return buildCameraWidget(std::dynamic_pointer_cast<engine::CameraComponent>(component), item);
    } else if (component->type() == "transform") {
        return buildTransformWidget(std::dynamic_pointer_cast<engine::TransformComponent>(component));
    } else if (component->type() == "flipbook_animation") {
        return buildFlipbookAnimationWidget(std::dynamic_pointer_cast<engine::FlipbookAnimationComponent>(component), item);
    } else if (component->type() == "mouse_event_filter") {
        return buildMouseEventFilterWidget(std::dynamic_pointer_cast<engine::MouseEventFilterComponent>(component));
    } else if (component->type() == "render_scope") {
        return buildRenderScopeWidget(std::dynamic_pointer_cast<engine::RenderScopeComponent>(component), item);
    } else if (component->type() == "render_pass") {
        return buildRenderPassWidget(std::dynamic_pointer_cast<engine::RenderPassComponent>(component), item);
    } else if (component->type() == "light_source") {
        return buildLightSourceWidget(std::dynamic_pointer_cast<engine::LightSourceComponent>(component));
    }

    return std::nullopt;
}

}
