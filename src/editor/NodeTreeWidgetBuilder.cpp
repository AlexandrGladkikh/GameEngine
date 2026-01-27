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
#include "engine/RenderComponent.h"
#include "engine/Context.h"
#include "engine/Node.h"
#include "engine/Helpers.h"
#include "engine/Utils.h"
#include "engine/TextureStore.h"
#include "engine/ShaderStore.h"
#include "engine/MeshStore.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QGraphicsOpacityEffect>

#include <QtCore/qnamespace.h>
#include <string>
#include <functional>

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
        { "X", formatFloat(transform->getPosition().x), positionXChangeHandler, positionXUpdater, false, false, {}, true, -1000.0f, 1000.0f },
        { "Y", formatFloat(transform->getPosition().y), positionYChangeHandler, positionYUpdater, false, false, {}, true, -1000.0f, 1000.0f },
        { "Z", formatFloat(transform->getPosition().z), positionZChangeHandler, positionZUpdater, false, false, {}, true, -1000.0f, 1000.0f },
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
        { "X", formatFloat(transform->getRotation().x), rotationXChangeHandler, rotationXUpdater, false, false, {}, true, -1000.0f, 1000.0f },
        { "Y", formatFloat(transform->getRotation().y), rotationYChangeHandler, rotationYUpdater, false, false, {}, true, -1000.0f, 1000.0f },
        { "Z", formatFloat(transform->getRotation().z), rotationZChangeHandler, rotationZUpdater, false, false, {}, true, -1000.0f, 1000.0f },
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
        { "X", formatFloat(transform->getScale().x), scaleXChangeHandler, scaleXUpdater, false, false, {}, true, 0.0f, 1000.0f },
        { "Y", formatFloat(transform->getScale().y), scaleYChangeHandler, scaleYUpdater, false, false, {}, true, 0.0f, 1000.0f },
        { "Z", formatFloat(transform->getScale().z), scaleZChangeHandler, scaleZUpdater, false, false, {}, true, 0.0f, 1000.0f },
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
        ortho_data.push_back({ "left", formatFloat(ortho.left), orthoLeftChangeHandler, orthoLeftUpdater, false, false, {}, true, -1000.0f, 1000.0f });
        ortho_data.push_back({ "right", formatFloat(ortho.right), orthoRightChangeHandler, orthoRightUpdater, false, false, {}, true, -1000.0f, 1000.0f });
        ortho_data.push_back({ "top", formatFloat(ortho.top), orthoTopChangeHandler, orthoTopUpdater, false, false, {}, true, -1000.0f, 1000.0f });
        ortho_data.push_back({ "bottom", formatFloat(ortho.bottom), orthoBottomChangeHandler, orthoBottomUpdater, false, false, {}, true, -1000.0f, 1000.0f });
        ortho_data.push_back({ "near", formatFloat(camera->getNear()), orthoNearChangeHandler, orthoNearUpdater, false, false, {}, true, -1000.0f, 1000.0f });
        ortho_data.push_back({ "far", formatFloat(camera->getFar()), orthoFarChangeHandler, orthoFarUpdater, false, false, {}, true, -1000.0f, 1000.0f });

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
            { "near", formatFloat(camera->getNear()), nearChangeHandler, nearUpdater, false, false, {}, true, 0.0f, 1000.0f }
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
            { "far", formatFloat(camera->getFar()), farChangeHandler, farUpdater, false, false, {}, true, 0.0f, 1000.0f }
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
            { "aspect", formatFloat(camera->getAspect()), aspectChangeHandler, aspectUpdater, false, false, {}, true, 0.0f, 1000.0f }
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
        { "yaw", formatFloat(camera->getYaw()), yawChangeHandler, yawUpdater, false, false, {}, true, -1000.0f, 1000.0f }
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
        { "pitch", formatFloat(camera->getPitch()), pitchChangeHandler, pitchUpdater, false, false, {}, true, -1000.0f, 1000.0f }
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
        { "update time", formatFloat(animation->updateTime()), updateTimeChangeHandler, updateTimeUpdater, false, false, {}, true, 0.0f, 1000.0f }
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

ComponentWidget* TreeWidgetBuilder::buildRenderWidget(const std::shared_ptr<engine::RenderComponent>& render)
{
    ComponentWidget* render_widget = new ComponentWidget;
    m_tree_widget_builder_helper->subscribeOnActiveComponent(render_widget, render);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);

    std::string name = "Render name \"" + render->name() + "\"";
    auto* label = new QLabel(QString::fromStdString(name));
    decorateLabel(label);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    layout->addWidget(label);

    std::vector<std::string> is_sprite = { "Yes", "No" };
    std::map<std::string, bool> sprite_map = { {"Yes", true},
                                               {"No", false} };
    auto sprite_widget = createComboBoxWidget("Sprite", is_sprite, [render, sprite_map](const std::string& value) {
        render->setSprite(sprite_map.at(value));
    }, render->isSprite() ? "Yes" : "No");
    layout->addWidget(sprite_widget);

    layout->addStretch();

    render_widget->setLayout(layout);

    return render_widget;
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
    } else if (component->type() == "render") {
        return buildRenderWidget(std::dynamic_pointer_cast<engine::RenderComponent>(component));
    }

    return std::nullopt;
}

}
