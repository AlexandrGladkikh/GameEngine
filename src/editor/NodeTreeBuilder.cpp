#include "NodeTreeBuilder.h"

#include "ComponentWidget.h"
#include "NodeWidget.h"
#include "Utils.h"

#include "engine/TransformComponent.h"
#include "engine/MeshComponent.h"
#include "engine/MaterialComponent.h"
#include "engine/CameraComponent.h"
#include "engine/FlipbookAnimationComponent.h"
#include "engine/Context.h"
#include "engine/Node.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>

#include <string>
#include <functional>

namespace editor {

ComponentWidget* buildTransformWidget(const std::shared_ptr<engine::TransformComponent>& transform)
{
    ComponentWidget* transform_widget = new ComponentWidget;

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);

    std::string name = "Transform name \"" + transform->name() + "\"";
    auto* label = new QLabel(QString::fromStdString(name));
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

    QHBoxLayout* position_layout = new QHBoxLayout();
    position_layout->setSpacing(1);
    position_layout->setContentsMargins(0, 0, 0, 0);
    label = new QLabel("Position");
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    position_layout->addWidget(label);
    position_layout->addWidget(createLabelLineEditorWidget("X", formatFloat(transform->getPosition().x), positionXChangeHandler, positionXUpdater));
    position_layout->addWidget(createLabelLineEditorWidget("Y", formatFloat(transform->getPosition().y), positionYChangeHandler, positionYUpdater));
    position_layout->addWidget(createLabelLineEditorWidget("Z", formatFloat(transform->getPosition().z), positionZChangeHandler, positionZUpdater));
    position_layout->addStretch();
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

    QHBoxLayout* rotation_layout = new QHBoxLayout();
    rotation_layout->setSpacing(1);
    rotation_layout->setContentsMargins(0, 0, 0, 0);
    label = new QLabel("Rotation");
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    rotation_layout->addWidget(label);
    rotation_layout->addWidget(createLabelLineEditorWidget("X", formatFloat(transform->getRotation().x), rotationXChangeHandler, rotationXUpdater));
    rotation_layout->addWidget(createLabelLineEditorWidget("Y", formatFloat(transform->getRotation().y), rotationYChangeHandler, rotationYUpdater));
    rotation_layout->addWidget(createLabelLineEditorWidget("Z", formatFloat(transform->getRotation().z), rotationZChangeHandler, rotationZUpdater));
    rotation_layout->addStretch();
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

    QHBoxLayout* scale_layout = new QHBoxLayout();
    scale_layout->setSpacing(1);
    scale_layout->setContentsMargins(0, 0, 0, 0);
    label = new QLabel("Scale");
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    scale_layout->addWidget(label);
    scale_layout->addWidget(createLabelLineEditorWidget("X", formatFloat(transform->getScale().x), scaleXChangeHandler, scaleXUpdater));
    scale_layout->addWidget(createLabelLineEditorWidget("Y", formatFloat(transform->getScale().y), scaleYChangeHandler, scaleYUpdater));
    scale_layout->addWidget(createLabelLineEditorWidget("Z", formatFloat(transform->getScale().z), scaleZChangeHandler, scaleZUpdater));
    scale_layout->addStretch();
    layout->addLayout(scale_layout);

    layout->addStretch();

    transform_widget->setLayout(layout);

    return transform_widget;
}

ComponentWidget* buildMaterialWidget(const std::shared_ptr<engine::MaterialComponent>& material)
{
    ComponentWidget* material_widget = new ComponentWidget;

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);

    std::string name = "Material name \"" + material->name() + "\"";
    auto* label = new QLabel(QString::fromStdString(name));
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

    QHBoxLayout* texture_layout = new QHBoxLayout();
    texture_layout->setSpacing(1);
    texture_layout->setContentsMargins(0, 0, 0, 0);
    texture_layout->addWidget(createLabelLineEditorWidget("texture", material->textureName(), textureChangeHandler, textureUpdater));
    texture_layout->addStretch();
    layout->addLayout(texture_layout);

    QHBoxLayout* shader_layout = new QHBoxLayout();
    shader_layout->setSpacing(1);
    shader_layout->setContentsMargins(0, 0, 0, 0);
    shader_layout->addWidget(createLabelLineEditorWidget("shader", material->shaderName(), shaderChangeHandler, shaderUpdater));
    shader_layout->addStretch();
    layout->addLayout(shader_layout);

    layout->addStretch();

    material_widget->setLayout(layout);

    return material_widget;
}

ComponentWidget* buildMeshWidget(const std::shared_ptr<engine::MeshComponent>& mesh)
{
    ComponentWidget* mesh_widget = new ComponentWidget;

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);

    std::string name = "Mesh name \"" + mesh->name() + "\"";
    auto* label = new QLabel(QString::fromStdString(name));
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

    QHBoxLayout* mesh_layout = new QHBoxLayout();
    mesh_layout->setSpacing(1);
    mesh_layout->setContentsMargins(0, 0, 0, 0);
    mesh_layout->addWidget(createLabelLineEditorWidget("mesh", mesh->meshName(), meshChangeHandler, meshUpdater));
    mesh_layout->addStretch();
    layout->addLayout(mesh_layout);

    layout->addStretch();

    mesh_widget->setLayout(layout);

    return mesh_widget;
}

ComponentWidget* buildCameraWidget(const std::shared_ptr<engine::CameraComponent>& camera)
{
    ComponentWidget* camera_widget = new ComponentWidget;

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);

    std::string name = "Camera name \"" + camera->name() + "\"";
    auto* label = new QLabel(QString::fromStdString(name));
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    layout->addWidget(label);

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
            auto ortho = camera->getOrtho();
            ortho.near = param;
            camera->setOrtho(ortho);
        }
    };

    auto orthoFarChangeHandler = [camera](const std::string& value) {
        float param = 0.0f;
        if (parseFloat(value, param)) {
            auto ortho = camera->getOrtho();
            ortho.far = param;
            camera->setOrtho(ortho);
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
        auto ortho = camera->getOrtho();
        return formatFloat(ortho.near);
    };

    auto orthoFarUpdater = [camera]() {
        auto ortho = camera->getOrtho();
        return formatFloat(ortho.far);
    };

    auto ortho = camera->getOrtho();
    QHBoxLayout* ortho_layout = new QHBoxLayout();
    ortho_layout->setSpacing(1);
    ortho_layout->setContentsMargins(0, 0, 0, 0);
    label = new QLabel("Ortho");
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    ortho_layout->addWidget(label);
    ortho_layout->addWidget(createLabelLineEditorWidget("left", formatFloat(ortho.left), orthoLeftChangeHandler, orthoLeftUpdater));
    ortho_layout->addWidget(createLabelLineEditorWidget("right", formatFloat(ortho.right), orthoRightChangeHandler, orthoRightUpdater));
    ortho_layout->addWidget(createLabelLineEditorWidget("top", formatFloat(ortho.top), orthoTopChangeHandler, orthoTopUpdater));
    ortho_layout->addWidget(createLabelLineEditorWidget("bottom", formatFloat(ortho.bottom), orthoBottomChangeHandler, orthoBottomUpdater));
    ortho_layout->addWidget(createLabelLineEditorWidget("near", formatFloat(ortho.near), orthoNearChangeHandler, orthoNearUpdater));
    ortho_layout->addWidget(createLabelLineEditorWidget("far", formatFloat(ortho.far), orthoFarChangeHandler, orthoFarUpdater));
    ortho_layout->addStretch();
    layout->addLayout(ortho_layout);

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

    QHBoxLayout* yaw_layout = new QHBoxLayout();
    yaw_layout->setSpacing(1);
    yaw_layout->setContentsMargins(0, 0, 0, 0);
    yaw_layout->addWidget(createLabelLineEditorWidget("Yaw", formatFloat(camera->getYaw()), yawChangeHandler, yawUpdater));
    yaw_layout->addStretch();
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

    QHBoxLayout* pitch_layout = new QHBoxLayout();
    pitch_layout->setSpacing(1);
    pitch_layout->setContentsMargins(0, 0, 0, 0);
    pitch_layout->addWidget(createLabelLineEditorWidget("Pitch", formatFloat(camera->getPitch()), pitchChangeHandler, pitchUpdater));
    pitch_layout->addStretch();
    layout->addLayout(pitch_layout);

    layout->addStretch();

    camera_widget->setLayout(layout);

    return camera_widget;
}

ComponentWidget* buildFlipbookAnimationWidget(const std::shared_ptr<engine::FlipbookAnimationComponent>& animation)
{
    ComponentWidget* flipbook_widget = new ComponentWidget;

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);

    std::string name = "Flipbook Animation name \"" + animation->name() + "\"";
    auto* label = new QLabel(QString::fromStdString(name));
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

    QHBoxLayout* update_time_layout = new QHBoxLayout();
    update_time_layout->setSpacing(1);
    update_time_layout->setContentsMargins(0, 0, 0, 0);
    update_time_layout->addWidget(createLabelLineEditorWidget("update time", formatFloat(animation->updateTime()), updateTimeChangeHandler, updateTimeUpdater));
    update_time_layout->addStretch();
    layout->addLayout(update_time_layout);

    for (size_t i = 0; i < materials_ids.size(); i++) {
        auto material_id = materials_ids[i];
        auto material = scene.value()->getComponent(material_id);
        if (!material) {
            continue;
        }

        auto material_value = std::dynamic_pointer_cast<engine::MaterialComponent>(material.value());
        if (!material_value) {
            continue;
        }

        auto materialChangeHandler = [animation, i](const std::string& value) {
            auto node = animation->getNode();
            if (!node.has_value()) {
                return;
            }

            auto scene = node.value()->getScene();
            if (!scene.has_value()) {
                return;
            }

            auto material = scene.value()->getComponent(value);
            if (!material) {
                return;
            }

            auto new_material_value = std::dynamic_pointer_cast<engine::MaterialComponent>(material.value());
            if (!new_material_value) {
                return;
            }

            auto materials_ids = animation->materialIds();
            animation->replaceMaterial(materials_ids[i], new_material_value->id());
        };

        auto materialUpdater = [i, animation]() {
            auto node = animation->getNode();
            if (!node.has_value()) {
                return std::string();
            }

            auto scene = node.value()->getScene();
            if (!scene.has_value()) {
                return std::string();
            }

            auto materials_ids = animation->materialIds();
            auto material = scene.value()->getComponent(materials_ids[i]);
            if (!material) {
                return std::string();
            }

            auto material_value = std::dynamic_pointer_cast<engine::MaterialComponent>(material.value());
            if (!material_value) {
                return std::string();
            }

            if (!material_value->isValid()) {
                return std::string();
            }

            return material_value->name();
        };

        QHBoxLayout* animation_layout = new QHBoxLayout();
        animation_layout->setSpacing(1);
        animation_layout->setContentsMargins(0, 0, 0, 0);
        animation_layout->addWidget(createLabelLineEditorWidget("animation", material_value->name(), materialChangeHandler, materialUpdater));
        animation_layout->addStretch();
        layout->addLayout(animation_layout);
    }

    layout->addStretch();

    flipbook_widget->setLayout(layout);

    return flipbook_widget;
}

auto NodeTreeBuilder::buildWidgetForNode(const std::string& node_name) -> std::optional<NodeWidget*>
{
    NodeWidget* new_widget = new NodeWidget();
    auto label = new QLabel(new_widget);
    label->setText(QString::fromStdString(node_name));

    return new_widget;
}

auto NodeTreeBuilder::buildWidgetForComponent(std::shared_ptr<engine::Component> component) -> std::optional<ComponentWidget*>
{
    if (component->type() == "material") {
        return buildMaterialWidget(std::dynamic_pointer_cast<engine::MaterialComponent>(component));
    } else if (component->type() == "mesh") {
        return buildMeshWidget(std::dynamic_pointer_cast<engine::MeshComponent>(component));
    } else if (component->type() == "camera") {
        return buildCameraWidget(std::dynamic_pointer_cast<engine::CameraComponent>(component));
    } else if (component->type() == "transform") {
        return buildTransformWidget(std::dynamic_pointer_cast<engine::TransformComponent>(component));
    } else if (component->type() == "flipbook_animation") {
        return buildFlipbookAnimationWidget(std::dynamic_pointer_cast<engine::FlipbookAnimationComponent>(component));
    }

    return std::nullopt;
}

}