#include "NodeTreeWidgetBuilder.h"

#include "editor/ComponentWidget.h"
#include "editor/NodeWidget.h"
#include "editor/Utils.h"

#include "engine/TransformComponent.h"
#include "engine/MeshComponent.h"
#include "engine/MaterialComponent.h"
#include "engine/CameraComponent.h"
#include "engine/FlipbookAnimationComponent.h"
#include "engine/Context.h"
#include "engine/Node.h"
#include "engine/Helpers.h"
#include "engine/Utils.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>
#include <QTreeWidget>

#include <string>
#include <functional>

namespace editor {

ComponentWidget* NodeTreeWidgetBuilder::buildTransformWidget(const std::shared_ptr<engine::TransformComponent>& transform)
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

    std::vector<EditorBlockLayoutData> position_data = {
        { "X", formatFloat(transform->getPosition().x), positionXChangeHandler, positionXUpdater },
        { "Y", formatFloat(transform->getPosition().y), positionYChangeHandler, positionYUpdater },
        { "Z", formatFloat(transform->getPosition().z), positionZChangeHandler, positionZUpdater },
    };
    auto position_layout = createEditorBlockLayout("Position", position_data);
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
        { "X", formatFloat(transform->getRotation().x), rotationXChangeHandler, rotationXUpdater },
        { "Y", formatFloat(transform->getRotation().y), rotationYChangeHandler, rotationYUpdater },
        { "Z", formatFloat(transform->getRotation().z), rotationZChangeHandler, rotationZUpdater },
    };
    auto rotation_layout = createEditorBlockLayout("Rotation", rotation_data);
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
        { "X", formatFloat(transform->getScale().x), scaleXChangeHandler, scaleXUpdater },
        { "Y", formatFloat(transform->getScale().y), scaleYChangeHandler, scaleYUpdater },
        { "Z", formatFloat(transform->getScale().z), scaleZChangeHandler, scaleZUpdater },
    };
    auto scale_layout = createEditorBlockLayout("Scale", scale_data);
    layout->addLayout(scale_layout);

    layout->addStretch();

    transform_widget->setLayout(layout);

    return transform_widget;
}

ComponentWidget* NodeTreeWidgetBuilder::buildMaterialWidget(const std::shared_ptr<engine::MaterialComponent>& material)
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

    std::vector<EditorBlockLayoutData> texture_data = {
        { "texture", material->textureName(), textureChangeHandler, textureUpdater },
    };
    auto texture_layout = createEditorBlockLayout("Texture", texture_data);
    layout->addLayout(texture_layout);

    std::vector<EditorBlockLayoutData> shader_data = {
        { "shader", material->shaderName(), shaderChangeHandler, shaderUpdater },
    };
    auto shader_layout = createEditorBlockLayout("Shader", shader_data);
    layout->addLayout(shader_layout);

    layout->addStretch();

    material_widget->setLayout(layout);

    return material_widget;
}

ComponentWidget* NodeTreeWidgetBuilder::buildMeshWidget(const std::shared_ptr<engine::MeshComponent>& mesh)
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

    std::vector<EditorBlockLayoutData> mesh_data = {
        { "mesh", mesh->meshName(), meshChangeHandler, meshUpdater },
    };
    auto mesh_layout = createEditorBlockLayout("Mesh", mesh_data);
    layout->addLayout(mesh_layout);

    layout->addStretch();

    mesh_widget->setLayout(layout);

    return mesh_widget;
}

ComponentWidget* NodeTreeWidgetBuilder::buildCameraWidget(const std::shared_ptr<engine::CameraComponent>& camera)
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

    std::vector<EditorBlockLayoutData> ortho_data = {};
    ortho_data.push_back({ "left", formatFloat(ortho.left), orthoLeftChangeHandler, orthoLeftUpdater });
    ortho_data.push_back({ "right", formatFloat(ortho.right), orthoRightChangeHandler, orthoRightUpdater });
    ortho_data.push_back({ "top", formatFloat(ortho.top), orthoTopChangeHandler, orthoTopUpdater });
    ortho_data.push_back({ "bottom", formatFloat(ortho.bottom), orthoBottomChangeHandler, orthoBottomUpdater });
    ortho_data.push_back({ "near", formatFloat(ortho.near), orthoNearChangeHandler, orthoNearUpdater });
    ortho_data.push_back({ "far", formatFloat(ortho.far), orthoFarChangeHandler, orthoFarUpdater });

    QHBoxLayout* ortho_layout = createEditorBlockLayout("Ortho", ortho_data);
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

    std::vector<EditorBlockLayoutData> yaw_data = {
        { "yaw", formatFloat(camera->getYaw()), yawChangeHandler, yawUpdater }
    };

    QHBoxLayout* yaw_layout = createEditorBlockLayout("Yaw", yaw_data);
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
        { "pitch", formatFloat(camera->getPitch()), pitchChangeHandler, pitchUpdater }
    };

    QHBoxLayout* pitch_layout = createEditorBlockLayout("Pitch", pitch_data);
    layout->addLayout(pitch_layout);

    layout->addStretch();

    camera_widget->setLayout(layout);

    return camera_widget;
}

ComponentWidget* NodeTreeWidgetBuilder::buildFlipbookAnimationWidget(const std::shared_ptr<engine::FlipbookAnimationComponent>& animation, QTreeWidget* tree, QTreeWidgetItem* item)
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

    std::vector<EditorBlockLayoutData> update_time_data = {
        { "update time", formatFloat(animation->updateTime()), updateTimeChangeHandler, updateTimeUpdater }
    };
    QHBoxLayout* update_time_layout = createEditorBlockLayout("Update time", update_time_data);
    layout->addLayout(update_time_layout);

    std::shared_ptr<std::vector<QHBoxLayout*>> material_layouts(new std::vector<QHBoxLayout*>);
    for (size_t i = 0; i < materials_ids.size(); ++i) {
        material_layouts->push_back(new QHBoxLayout);
    }

    std::vector<std::string> buttons_names = {"Add", "Delete"};

    auto add_handler = [animation, layout, material_layouts, tree, item]() {
        animation->addMaterial(engine::generateUniqueId());
        auto materials_ids = animation->materialIds();

        size_t i = materials_ids.size() - 1;
        auto materialChangeHandler = [animation, i](const std::string& value) {
            auto material = engine::getComponentByName<engine::MaterialComponent>(animation, value);
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

            if (!material->isValid()) {
                return std::string();
            }

            return material->name();
        };

        std::vector<EditorBlockLayoutData> animation_data = {
            { "animation", "", materialChangeHandler, materialUpdater }
        };
        QHBoxLayout* animation_layout = createEditorBlockLayout("", animation_data);
        layout->addLayout(animation_layout);

        material_layouts.get()->push_back(animation_layout);

        QTreeWidgetItem* temp = new QTreeWidgetItem(item);
        temp->setText(0, "temp");
        tree->setItemWidget(temp, 0, nullptr);
        delete temp;
    };

    auto delete_handler = [animation, material_layouts, tree, item]() {
        auto materials_ids = animation->materialIds();

        if (materials_ids.empty()) {
            return;
        }

        animation->removeMaterial(materials_ids.back());

        material_layouts->back()->itemAt(0)->widget()->deleteLater();

        delete material_layouts->back();
        material_layouts->pop_back();

        QTreeWidgetItem* temp = new QTreeWidgetItem(item);
        temp->setText(0, "temp");
        tree->setItemWidget(temp, 0, nullptr);
        delete temp;
    };

    layout->addWidget(createButtonLineWidget(buttons_names, { add_handler, delete_handler }));

    for (size_t i = 0; i < materials_ids.size(); ++i) {
        auto material_id = materials_ids[i];
        auto material = scene.value()->getComponent(material_id);
        if (!material) {
            animation->removeMaterial(material_id);
            continue;
        }

        auto material_value = std::dynamic_pointer_cast<engine::MaterialComponent>(material.value());
        if (!material_value) {
            continue;
        }

        auto materialChangeHandler = [animation, i](const std::string& value) {
            auto material = engine::getComponentByName<engine::MaterialComponent>(animation, value);
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

            return material->name();
        };

        std::vector<EditorBlockLayoutData> animation_data = {
            { "animation", material_value->name(), materialChangeHandler, materialUpdater }
        };
        setupEditorBlockLayout(material_layouts->at(i), "", animation_data);
        layout->addLayout(material_layouts->at(i));
    }

    layout->addStretch();

    flipbook_widget->setLayout(layout);

    return flipbook_widget;
}

NodeTreeWidgetBuilder::NodeTreeWidgetBuilder(SceneNodeTree* scene_node_tree) :
    m_scene_node_tree(scene_node_tree)
{

}

auto NodeTreeWidgetBuilder::buildWidgetForNode(const std::string& node_name) -> std::optional<NodeWidget*>
{
    NodeWidget* new_widget = new NodeWidget();
    auto label = new QLabel(new_widget);
    label->setText(QString::fromStdString(node_name));

    return new_widget;
}

auto NodeTreeWidgetBuilder::buildWidgetForComponent(std::shared_ptr<engine::Component> component, QTreeWidget* tree, QTreeWidgetItem* item) -> std::optional<ComponentWidget*>
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
        return buildFlipbookAnimationWidget(std::dynamic_pointer_cast<engine::FlipbookAnimationComponent>(component), tree, item);
    }

    return std::nullopt;
}

}