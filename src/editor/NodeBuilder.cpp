#include "NodeBuilder.h"

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
#include <sstream>
#include <functional>

namespace editor {

QWidget* createLabelLineEditorWidget(const std::string& label, const std::string& value,
    const std::function<void(const std::string&)>& changeHandler, const std::function<std::string()>& updateHandler)
{
    QWidget* widget = new QWidget;

    QHBoxLayout* layout = new QHBoxLayout;
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(new QLabel(label.c_str()));
    auto* lineEditor = new QLineEdit(value.c_str());

    if (updateHandler) {
        auto timer = new QTimer(lineEditor);
        timer->setInterval(100);
        timer->setTimerType(Qt::PreciseTimer);
        QObject::connect(timer, &QTimer::timeout, [lineEditor, updateHandler]() {
            auto current_text = lineEditor->text();
            if (current_text != updateHandler().c_str()) {
                lineEditor->setText(updateHandler().c_str());
            }
        });
        timer->start();
    }
    if (changeHandler) {
        QObject::connect(lineEditor, &QLineEdit::textChanged, [changeHandler](const QString& value) {
            changeHandler(value.toStdString());
        });
    }
    layout->addWidget(lineEditor);

    widget->setLayout(layout);

    return widget;
}

std::string formatFloat(float value)
{
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2) << value;
    return ss.str();
}

QWidget* buildTransformWidget(const std::shared_ptr<engine::TransformComponent>& transform)
{
    QWidget* transform_widget = new QWidget;

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* label = new QLabel("Transform");
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    layout->addWidget(label);

    auto positionXChangeHandler = [transform](const std::string& value) {
        if (value.empty()) {
            return;
        }
        transform->setPosition({ std::stof(value), transform->getPosition().y, transform->getPosition().z });
    };
    auto positionYChangeHandler = [transform](const std::string& value) {
        if (value.empty()) {
            return;
        }
        transform->setPosition({ transform->getPosition().x, std::stof(value), transform->getPosition().z });
    };
    auto positionZChangeHandler = [transform](const std::string& value) {
        if (value.empty()) {
            return;
        }
        transform->setPosition({ transform->getPosition().x, transform->getPosition().y, std::stof(value) });
    };

    auto positionXUpdater = [transform]() {
        return formatFloat(transform->getPosition().x);
    };
    auto positionYUpdater = [transform]() {
        return formatFloat(transform->getPosition().y);
    };
    auto positionZUpdater = [transform]() {
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
        if (value.empty()) {
            return;
        }
        transform->setRotation({ std::stof(value), transform->getRotation().y, transform->getRotation().z });
    };
    auto rotationYChangeHandler = [transform](const std::string& value) {
        if (value.empty()) {
            return;
        }
        transform->setRotation({ transform->getRotation().x, std::stof(value), transform->getRotation().z });
    };
    auto rotationZChangeHandler = [transform](const std::string& value) {
        if (value.empty()) {
            return;
        }
        transform->setRotation({ transform->getRotation().x, transform->getRotation().y, std::stof(value) });
    };

    auto rotationXUpdater = [transform]() {
        return formatFloat(transform->getRotation().x);
    };
    auto rotationYUpdater = [transform]() {
        return formatFloat(transform->getRotation().y);
    };
    auto rotationZUpdater = [transform]() {
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
        if (value.empty()) {
            return;
        }
        transform->setScale({ std::stof(value), transform->getScale().y, transform->getScale().z });
    };
    auto scaleYChangeHandler = [transform](const std::string& value) {
        if (value.empty()) {
            return;
        }
        transform->setScale({ transform->getScale().x, std::stof(value), transform->getScale().z });
    };
    auto scaleZChangeHandler = [transform](const std::string& value) {
        if (value.empty()) {
            return;
        }
        transform->setScale({ transform->getScale().x, transform->getScale().y, std::stof(value) });
    };

    auto scaleXUpdater = [transform]() {
        return formatFloat(transform->getScale().x);
    };
    auto scaleYUpdater = [transform]() {
        return formatFloat(transform->getScale().y);
    };
    auto scaleZUpdater = [transform]() {
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

QWidget* buildMaterialWidget(const std::shared_ptr<engine::MaterialComponent>& material)
{
    QWidget* material_widget = new QWidget;

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* label = new QLabel("Material");
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    layout->addWidget(label);

    auto textureChangeHandler = [material](const std::string& value) {
        material->setTexture(value);
    };
    auto shaderChangeHandler = [material](const std::string& value) {
        material->setShader(value);
    };

    auto textureUpdater = [material]() {
        return material->textureName();
    };

    auto shaderUpdater = [material]() {
        return material->shaderName();
    };

    QHBoxLayout* texture_layout = new QHBoxLayout();
    texture_layout->setSpacing(1);
    texture_layout->setContentsMargins(0, 0, 0, 0);
    label = new QLabel("Texture");
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    texture_layout->addWidget(label);
    texture_layout->addWidget(createLabelLineEditorWidget("texture", material->textureName(), textureChangeHandler, textureUpdater));
    texture_layout->addStretch();
    layout->addLayout(texture_layout);

    QHBoxLayout* shader_layout = new QHBoxLayout();
    shader_layout->setSpacing(1);
    shader_layout->setContentsMargins(0, 0, 0, 0);
    label = new QLabel("Shader");
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    shader_layout->addWidget(label);
    shader_layout->addWidget(createLabelLineEditorWidget("shader", material->shaderName(), shaderChangeHandler, shaderUpdater));
    shader_layout->addStretch();
    layout->addLayout(shader_layout);

    layout->addStretch();

    material_widget->setLayout(layout);

    return material_widget;
}

QWidget* buildMeshWidget(const std::shared_ptr<engine::MeshComponent>& mesh)
{
    QWidget* mesh_widget = new QWidget;

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* label = new QLabel("Mesh");
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    layout->addWidget(label);

    auto meshChangeHandler = [mesh](const std::string& value) {
        mesh->setMesh(value);
    };

    auto meshUpdater = [mesh]() {
        return mesh->meshName();
    };

    QHBoxLayout* mesh_layout = new QHBoxLayout();
    mesh_layout->setSpacing(1);
    mesh_layout->setContentsMargins(0, 0, 0, 0);
    label = new QLabel("Mesh");
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    mesh_layout->addWidget(label);
    mesh_layout->addWidget(createLabelLineEditorWidget("mesh", mesh->meshName(), meshChangeHandler, meshUpdater));
    mesh_layout->addStretch();
    layout->addLayout(mesh_layout);

    layout->addStretch();

    mesh_widget->setLayout(layout);

    return mesh_widget;
}

QWidget* buildCameraWidget(const std::shared_ptr<engine::CameraComponent>& camera)
{
    QWidget* camera_widget = new QWidget;

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* label = new QLabel("Camera");
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    layout->addWidget(label);

    auto yawChangeHandler = [camera](const std::string& value) {
        if (value.empty()) {
            return;
        }
        camera->setYaw(std::stof(value));
    };

    auto yawUpdater = [camera]() {
        return formatFloat(camera->getYaw());
    };

    QHBoxLayout* yaw_layout = new QHBoxLayout();
    yaw_layout->setSpacing(1);
    yaw_layout->setContentsMargins(0, 0, 0, 0);
    label = new QLabel("Yaw");
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    yaw_layout->addWidget(label);
    yaw_layout->addWidget(createLabelLineEditorWidget("Yaw", formatFloat(camera->getYaw()), yawChangeHandler, yawUpdater));
    yaw_layout->addStretch();
    layout->addLayout(yaw_layout);

    auto pitchChangeHandler = [camera](const std::string& value) {
        if (value.empty()) {
            return;
        }
        camera->setPitch(std::stof(value));
    };

    auto pitchUpdater = [camera]() {
        return formatFloat(camera->getPitch());
    };

    QHBoxLayout* pitch_layout = new QHBoxLayout();
    pitch_layout->setSpacing(1);
    pitch_layout->setContentsMargins(0, 0, 0, 0);
    label = new QLabel("Pitch");
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    pitch_layout->addWidget(label);
    pitch_layout->addWidget(createLabelLineEditorWidget("Pitch", formatFloat(camera->getPitch()), pitchChangeHandler, pitchUpdater));
    pitch_layout->addStretch();
    layout->addLayout(pitch_layout);

    layout->addStretch();

    camera_widget->setLayout(layout);

    return camera_widget;
}

QWidget* buildFlipbookAnimationWidget(const std::shared_ptr<engine::FlipbookAnimationComponent>& animation)
{
    QWidget* flipbook_widget = new QWidget;

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* label = new QLabel("Flipbook Animation");
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
        if (value.empty()) {
            return;
        }
        animation->setUpdateTime(std::stof(value));
    };

    auto updateTimeUpdater = [animation]() {
        return formatFloat(animation->updateTime());
    };

    QHBoxLayout* update_time_layout = new QHBoxLayout();
    update_time_layout->setSpacing(1);
    update_time_layout->setContentsMargins(0, 0, 0, 0);
    label = new QLabel("update time");
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    update_time_layout->addWidget(label);
    update_time_layout->addWidget(createLabelLineEditorWidget("update time", formatFloat(animation->updateTime()), updateTimeChangeHandler, updateTimeUpdater));
    update_time_layout->addStretch();
    layout->addLayout(update_time_layout);

    for (auto material_id : materials_ids) {
        auto material = scene.value()->getComponent(material_id);
        if (!material) {
            continue;
        }

        auto material_value = std::dynamic_pointer_cast<engine::MaterialComponent>(material.value());
        if (!material_value) {
            continue;
        }

        QHBoxLayout* animation_layout = new QHBoxLayout();
        animation_layout->setSpacing(1);
        animation_layout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel("Animation");
        label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        animation_layout->addWidget(label);
        animation_layout->addWidget(createLabelLineEditorWidget("Animation", material_value->name(), nullptr, nullptr));
        animation_layout->addStretch();
        layout->addLayout(animation_layout);
    }

    layout->addStretch();

    flipbook_widget->setLayout(layout);

    return flipbook_widget;
}

QWidget* NodeBuilder::buildComponent(std::shared_ptr<engine::Component> component)
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

    return nullptr;
}

}