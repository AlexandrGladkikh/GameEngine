#include "ComponentBuilder.h"

#include "CameraComponent.h"
#include "MaterialComponent.h"
#include "MeshComponent.h"
#include "TransformComponent.h"
#include "Logger.h"
#include "Utils.h"
#include "FlipbookAnimationComponent.h"
#include "MouseEventFilterComponent.h"
#include "RenderScopeComponent.h"
#include "RenderPassComponent.h"
#include "LightSourceComponent.h"

#include <optional>

namespace engine {

auto buildMaterialComponent(const rapidjson::Value& componentData) -> std::optional<std::unique_ptr<MaterialComponent>>
{
    auto id = componentData["id"].GetUint();
    auto name = componentData["name"].GetString();
    auto owner_node = componentData["owner_node"].GetUint();
    auto owner_scene = componentData["owner_scene"].GetUint();

    auto component = std::make_unique<MaterialComponent>(id, name, owner_node, owner_scene);

    auto shader_id = componentData["shader"].GetUint();
    auto texture_id = componentData["texture"].GetUint();

    component->setShader(shader_id);
    component->setTexture(texture_id);

    return component;
}

auto saveMaterialComponent(const std::shared_ptr<MaterialComponent>& component, rapidjson::Value& component_json, rapidjson::Document::AllocatorType& allocator)
{
    rapidjson::Value value;
    value.SetString(component->name().c_str(), allocator);
    component_json.AddMember("type", "material", allocator);
    component_json.AddMember("id", component->id(), allocator);
    component_json.AddMember("name", value, allocator);
    component_json.AddMember("owner_node", component->ownerNode(), allocator);
    component_json.AddMember("owner_scene", component->ownerScene(), allocator);
    component_json.AddMember("shader", component->shaderId(), allocator);
    component_json.AddMember("texture", component->textureId(), allocator);
}

auto buildMeshComponent(const rapidjson::Value& componentData) -> std::optional<std::unique_ptr<MeshComponent>>
{
    auto id = componentData["id"].GetUint();
    auto name = componentData["name"].GetString();
    auto owner_node = componentData["owner_node"].GetUint();
    auto owner_scene = componentData["owner_scene"].GetUint();

    auto component = std::make_unique<MeshComponent>(id, name, owner_node, owner_scene);

    auto meshId = componentData["mesh"].GetUint();

    component->setMesh(meshId);

    return component;
}

auto saveMeshComponent(const std::shared_ptr<MeshComponent>& component, rapidjson::Value& component_json, rapidjson::Document::AllocatorType& allocator)
{
    rapidjson::Value value;
    value.SetString(component->name().c_str(), allocator);
    component_json.AddMember("type", "mesh", allocator);
    component_json.AddMember("id", component->id(), allocator);
    component_json.AddMember("name", value, allocator);
    component_json.AddMember("owner_node", component->ownerNode(), allocator);
    component_json.AddMember("owner_scene", component->ownerScene(), allocator);
    component_json.AddMember("mesh", component->meshId(), allocator);
}

auto buildCameraComponent(const rapidjson::Value& componentData) -> std::optional<std::unique_ptr<CameraComponent>>
{
    auto id = componentData["id"].GetUint();
    auto name = componentData["name"].GetString();
    auto owner_node = componentData["owner_node"].GetUint();
    auto owner_scene = componentData["owner_scene"].GetUint();

    auto projection = componentData["projection"].GetObject();

    auto left = projection["left"].GetFloat();
    auto right = projection["right"].GetFloat();
    auto top = projection["top"].GetFloat();
    auto bottom = projection["bottom"].GetFloat();

    auto fov = componentData["fov"].GetFloat();
    auto aspect = componentData["aspect"].GetFloat();

    auto near = componentData["near"].GetFloat();
    auto far = componentData["far"].GetFloat();

    auto projection_type = static_cast<CameraComponent::ProjectionType>(componentData["projection_type"].GetInt());

    auto component = std::make_unique<CameraComponent>(id, name, owner_node, owner_scene);

    if (projection_type == CameraComponent::ProjectionType::Orthographic) {
        component->setProjectionType(projection_type);
        component->setOrtho(left, right, top, bottom, near, far);
    } else if (projection_type == CameraComponent::ProjectionType::Perspective) {
        component->setProjectionType(projection_type);
        component->setFov(fov);
        component->setNear(near);
        component->setFar(far);
        component->setAspect(aspect);
    }

    return component;
}

void saveCameraComponent(const std::shared_ptr<CameraComponent>& component, rapidjson::Value& component_json, rapidjson::Document::AllocatorType& allocator)
{
    rapidjson::Value value;
    value.SetString(component->name().c_str(), allocator);
    component_json.AddMember("type", "camera", allocator);
    component_json.AddMember("id", component->id(), allocator);
    component_json.AddMember("name", value, allocator);
    component_json.AddMember("owner_node", component->ownerNode(), allocator);
    component_json.AddMember("owner_scene", component->ownerScene(), allocator);

    auto ortho = component->getOrtho();

    rapidjson::Value projection(rapidjson::kObjectType);
    projection.AddMember("left", ortho.left, allocator);
    projection.AddMember("right", ortho.right, allocator);
    projection.AddMember("top", ortho.top, allocator);
    projection.AddMember("bottom", ortho.bottom, allocator);
    component_json.AddMember("projection", projection, allocator);

    component_json.AddMember("fov", component->getFov(), allocator);
    component_json.AddMember("aspect", component->getAspect(), allocator);

    component_json.AddMember("near", component->getNear(), allocator);
    component_json.AddMember("far", component->getFar(), allocator);

    component_json.AddMember("projection_type", static_cast<int>(component->projectionType()), allocator);
}

auto buildTransformComponent(const rapidjson::Value& componentData) -> std::optional<std::unique_ptr<TransformComponent>>
{
    Logger::debug(__FUNCTION__);

    auto id = componentData["id"].GetUint();
    auto name = componentData["name"].GetString();
    auto owner_node = componentData["owner_node"].GetUint();
    auto owner_scene = componentData["owner_scene"].GetUint();

    auto component = std::make_unique<TransformComponent>(id, name, owner_node, owner_scene);

    auto position_json = componentData["position"].GetArray();
    auto position = glm::vec3(position_json[0].GetFloat(), position_json[1].GetFloat(), position_json[2].GetFloat());
    component->setPosition(position);

    auto rotation_json = componentData["rotation"].GetArray();
    auto rotation = glm::vec3(rotation_json[0].GetFloat(), rotation_json[1].GetFloat(), rotation_json[2].GetFloat());
    component->setRotation(rotation);

    auto scale_json = componentData["scale"].GetArray();
    auto scale = glm::vec3(scale_json[0].GetFloat(), scale_json[1].GetFloat(), scale_json[2].GetFloat());
    component->setScale(scale);

    return component;
}

void saveTransformComponent(const std::shared_ptr<TransformComponent>& component, rapidjson::Value& component_json, rapidjson::Document::AllocatorType& allocator)
{
    rapidjson::Value value;
    value.SetString(component->name().c_str(), allocator);
    component_json.AddMember("type", "transform", allocator);
    component_json.AddMember("id", component->id(), allocator);
    component_json.AddMember("name", value, allocator);
    component_json.AddMember("owner_node", component->ownerNode(), allocator);
    component_json.AddMember("owner_scene", component->ownerScene(), allocator);

    rapidjson::Value position(rapidjson::kArrayType);
    position.PushBack(component->getPosition().x, allocator);
    position.PushBack(component->getPosition().y, allocator);
    position.PushBack(component->getPosition().z, allocator);
    component_json.AddMember("position", position, allocator);

    rapidjson::Value rotation(rapidjson::kArrayType);
    rotation.PushBack(component->getRotation().x, allocator);
    rotation.PushBack(component->getRotation().y, allocator);
    rotation.PushBack(component->getRotation().z, allocator);
    component_json.AddMember("rotation", rotation, allocator);

    rapidjson::Value scale(rapidjson::kArrayType);
    scale.PushBack(component->getScale().x, allocator);
    scale.PushBack(component->getScale().y, allocator);
    scale.PushBack(component->getScale().z, allocator);
    component_json.AddMember("scale", scale, allocator);
}

auto buildFlipbookAnimationComponent(const rapidjson::Value& componentData) -> std::optional<std::unique_ptr<FlipbookAnimationComponent>>
{
    Logger::debug(__FUNCTION__);

    auto id = componentData["id"].GetUint();
    auto name = componentData["name"].GetString();
    auto owner_node = componentData["owner_node"].GetUint();
    auto owner_scene = componentData["owner_scene"].GetUint();

    bool running = componentData["running"].GetBool();

    auto component = std::make_unique<FlipbookAnimationComponent>(id, name, owner_node, owner_scene);

    if (running) {
        component->start();
    } else {
        component->stop();
    }

    auto materials = componentData["materials"].GetArray();
    for (rapidjson::SizeType i = 0; i < materials.Size(); ++i) {
        component->addMaterial(materials[i].GetUint());
    }

    auto update_time = componentData["update_time"].GetUint();
    component->setUpdateTime(update_time);

    return component;
}

void saveFlipbookAnimationComponent(const std::shared_ptr<FlipbookAnimationComponent>& component, rapidjson::Value& component_json, rapidjson::Document::AllocatorType& allocator)
{
    rapidjson::Value value;
    value.SetString(component->name().c_str(), allocator);
    component_json.AddMember("type", "flipbook_animation", allocator);
    component_json.AddMember("id", component->id(), allocator);
    component_json.AddMember("name", value, allocator);
    component_json.AddMember("owner_node", component->ownerNode(), allocator);
    component_json.AddMember("owner_scene", component->ownerScene(), allocator);

    component_json.AddMember("running", component->isRunning(), allocator);

    rapidjson::Value materials(rapidjson::kArrayType);
    for (const auto material : component->materialIds()) {
        materials.PushBack(material, allocator);
    }
    component_json.AddMember("materials", materials, allocator);

    component_json.AddMember("update_time", component->updateTime(), allocator);
}

auto buildMouseEventFilterComponent(const rapidjson::Value& componentData) -> std::optional<std::unique_ptr<Component>>
{
    Logger::debug(__FUNCTION__);

    auto id = componentData["id"].GetUint();
    auto name = componentData["name"].GetString();
    auto owner_node = componentData["owner_node"].GetUint();
    auto owner_scene = componentData["owner_scene"].GetUint();
    auto key = componentData["key"].GetInt();
    auto action = componentData["action"].GetInt();

    auto component = std::make_unique<MouseEventFilterComponent>(id, name, owner_node, owner_scene);
    component->setKey(key);
    component->setAction(action);

    return component;
}

void saveMouseEventFilterComponent(const std::shared_ptr<MouseEventFilterComponent>& component, rapidjson::Value& component_json, rapidjson::Document::AllocatorType& allocator)
{
    rapidjson::Value value;
    value.SetString(component->name().c_str(), allocator);
    component_json.AddMember("type", "mouse_event_filter", allocator);
    component_json.AddMember("id", component->id(), allocator);
    component_json.AddMember("name", value, allocator);
    component_json.AddMember("owner_node", component->ownerNode(), allocator);
    component_json.AddMember("owner_scene", component->ownerScene(), allocator);
    component_json.AddMember("key", component->key(), allocator);
    component_json.AddMember("action", component->action(), allocator);
}

auto buildRenderScopeComponent(const rapidjson::Value& componentData) -> std::optional<std::unique_ptr<RenderScopeComponent>>
{
    Logger::debug(__FUNCTION__);

    auto id = componentData["id"].GetUint();
    auto name = componentData["name"].GetString();
    auto owner_node = componentData["owner_node"].GetUint();
    auto owner_scene = componentData["owner_scene"].GetUint();

    auto component = std::make_unique<RenderScopeComponent>(id, name, owner_node, owner_scene);

    RenderScopeComponent::RenderData render_data;
    if (componentData.HasMember("render_data") && componentData["render_data"].IsObject()) {
        const auto& render_data_obj = componentData["render_data"];
        if (render_data_obj.HasMember("is_sprite") && render_data_obj["is_sprite"].IsBool()) {
            render_data.is_sprite = render_data_obj["is_sprite"].GetBool();
        }
        if (render_data_obj.HasMember("uniforms") && render_data_obj["uniforms"].IsObject()) {
            auto render_data_src = render_data_obj["uniforms"].GetObject();
            auto read_vec = [](const rapidjson::Value& value, int size) -> std::optional<std::vector<float>> {
                if (!value.IsArray() || static_cast<int>(value.Size()) != size) {
                    return std::nullopt;
                }
                std::vector<float> out;
                out.reserve(size);
                for (rapidjson::SizeType i = 0; i < value.Size(); ++i) {
                    if (!value[i].IsNumber()) {
                        return std::nullopt;
                    }
                    out.push_back(static_cast<float>(value[i].GetDouble()));
                }
                return out;
            };
            auto read_mat_flat = [&read_vec](const rapidjson::Value& value, int size) -> std::optional<std::vector<float>> {
                if (value.IsArray()) {
                    if (value.Size() == static_cast<rapidjson::SizeType>(size)) {
                        return read_vec(value, size);
                    }
                    if (value.Size() == static_cast<rapidjson::SizeType>(std::sqrt(size))) {
                        std::vector<float> out;
                        out.reserve(size);
                        for (rapidjson::SizeType c = 0; c < value.Size(); ++c) {
                            if (!value[c].IsArray() || value[c].Size() != value.Size()) {
                                return std::nullopt;
                            }
                            for (rapidjson::SizeType r = 0; r < value[c].Size(); ++r) {
                                if (!value[c][r].IsNumber()) {
                                    return std::nullopt;
                                }
                                out.push_back(static_cast<float>(value[c][r].GetDouble()));
                            }
                        }
                        if (out.size() == static_cast<size_t>(size)) {
                            return out;
                        }
                    }
                }
                return std::nullopt;
            };

            for (rapidjson::Value::ConstMemberIterator itr = render_data_src.MemberBegin(); itr != render_data_src.MemberEnd(); ++itr) {
                const std::string uniform_name = itr->name.GetString();
                const auto& uniform_value = itr->value;

                if (uniform_value.IsObject() && uniform_value.HasMember("type") && uniform_value["type"].IsString() &&
                    uniform_value.HasMember("value")) {
                    const std::string type = uniform_value["type"].GetString();
                    const auto& value = uniform_value["value"];

                    if (type == "Float" && value.IsNumber()) {
                        render_data.uniforms[uniform_name] = static_cast<float>(value.GetDouble());
                    } else if (type == "Double" && value.IsNumber()) {
                        render_data.uniforms[uniform_name] = static_cast<double>(value.GetDouble());
                    } else if (type == "Int" && value.IsInt()) {
                        render_data.uniforms[uniform_name] = value.GetInt();
                    } else if (type == "UInt" && value.IsUint()) {
                        render_data.uniforms[uniform_name] = value.GetUint();
                    } else if (type == "Bool" && value.IsBool()) {
                        render_data.uniforms[uniform_name] = value.GetBool();
                    } else if (type == "Vec2") {
                        auto vec = read_vec(value, 2);
                        if (vec.has_value()) {
                            render_data.uniforms[uniform_name] = glm::vec2((*vec)[0], (*vec)[1]);
                        }
                    } else if (type == "Vec3") {
                        auto vec = read_vec(value, 3);
                        if (vec.has_value()) {
                            render_data.uniforms[uniform_name] = glm::vec3((*vec)[0], (*vec)[1], (*vec)[2]);
                        }
                    } else if (type == "Vec4") {
                        auto vec = read_vec(value, 4);
                        if (vec.has_value()) {
                            render_data.uniforms[uniform_name] = glm::vec4((*vec)[0], (*vec)[1], (*vec)[2], (*vec)[3]);
                        }
                    } else if (type == "Mat2") {
                        auto mat = read_mat_flat(value, 4);
                        if (mat.has_value()) {
                            glm::mat2 out(1.0f);
                            out[0][0] = (*mat)[0];
                            out[0][1] = (*mat)[1];
                            out[1][0] = (*mat)[2];
                            out[1][1] = (*mat)[3];
                            render_data.uniforms[uniform_name] = out;
                        }
                    } else if (type == "Mat3") {
                        auto mat = read_mat_flat(value, 9);
                        if (mat.has_value()) {
                            glm::mat3 out(1.0f);
                            int idx = 0;
                            for (int c = 0; c < 3; ++c) {
                                for (int r = 0; r < 3; ++r) {
                                    out[c][r] = (*mat)[idx++];
                                }
                            }
                            render_data.uniforms[uniform_name] = out;
                        }
                    } else if (type == "Mat4") {
                        auto mat = read_mat_flat(value, 16);
                        if (mat.has_value()) {
                            glm::mat4 out(1.0f);
                            int idx = 0;
                            for (int c = 0; c < 4; ++c) {
                                for (int r = 0; r < 4; ++r) {
                                    out[c][r] = (*mat)[idx++];
                                }
                            }
                            render_data.uniforms[uniform_name] = out;
                        }
                    }
                }
            }
        }
    }

    component->setRenderData(render_data);

    return component;
}

void saveRenderScopeComponent(const std::shared_ptr<RenderScopeComponent>& component, rapidjson::Value& component_json, rapidjson::Document::AllocatorType& allocator)
{
    rapidjson::Value value;
    value.SetString(component->name().c_str(), allocator);
    component_json.AddMember("type", "render_scope", allocator);
    component_json.AddMember("id", component->id(), allocator);
    component_json.AddMember("name", value, allocator);
    component_json.AddMember("owner_node", component->ownerNode(), allocator);
    component_json.AddMember("owner_scene", component->ownerScene(), allocator);

    rapidjson::Value render_data(rapidjson::kObjectType);
    render_data.AddMember("is_sprite", component->isSprite(), allocator);
    rapidjson::Value uniforms_json(rapidjson::kObjectType);
    for (const auto& uniform : component->renderData().uniforms) {
        rapidjson::Value uniform_key;
        uniform_key.SetString(uniform.first.c_str(), allocator);

        rapidjson::Value uniform_obj(rapidjson::kObjectType);
        rapidjson::Value type_value;
        rapidjson::Value data_value;

        if (uniform.second.type() == typeid(float)) {
            type_value.SetString("Float", allocator);
            data_value.SetFloat(std::any_cast<float>(uniform.second));
        } else if (uniform.second.type() == typeid(double)) {
            type_value.SetString("Double", allocator);
            data_value.SetDouble(std::any_cast<double>(uniform.second));
        } else if (uniform.second.type() == typeid(int)) {
            type_value.SetString("Int", allocator);
            data_value.SetInt(std::any_cast<int>(uniform.second));
        } else if (uniform.second.type() == typeid(uint32_t)) {
            type_value.SetString("UInt", allocator);
            data_value.SetUint(std::any_cast<uint32_t>(uniform.second));
        } else if (uniform.second.type() == typeid(bool)) {
            type_value.SetString("Bool", allocator);
            data_value.SetBool(std::any_cast<bool>(uniform.second));
        } else if (uniform.second.type() == typeid(glm::vec2)) {
            type_value.SetString("Vec2", allocator);
            auto vec = std::any_cast<glm::vec2>(uniform.second);
            data_value.SetArray();
            data_value.PushBack(vec.x, allocator);
            data_value.PushBack(vec.y, allocator);
        } else if (uniform.second.type() == typeid(glm::vec3)) {
            type_value.SetString("Vec3", allocator);
            auto vec = std::any_cast<glm::vec3>(uniform.second);
            data_value.SetArray();
            data_value.PushBack(vec.x, allocator);
            data_value.PushBack(vec.y, allocator);
            data_value.PushBack(vec.z, allocator);
        } else if (uniform.second.type() == typeid(glm::vec4)) {
            type_value.SetString("Vec4", allocator);
            auto vec = std::any_cast<glm::vec4>(uniform.second);
            data_value.SetArray();
            data_value.PushBack(vec.x, allocator);
            data_value.PushBack(vec.y, allocator);
            data_value.PushBack(vec.z, allocator);
            data_value.PushBack(vec.w, allocator);
        } else if (uniform.second.type() == typeid(glm::mat2)) {
            type_value.SetString("Mat2", allocator);
            auto mat = std::any_cast<glm::mat2>(uniform.second);
            data_value.SetArray();
            for (int c = 0; c < 2; ++c) {
                rapidjson::Value col(rapidjson::kArrayType);
                col.PushBack(mat[c][0], allocator);
                col.PushBack(mat[c][1], allocator);
                data_value.PushBack(col, allocator);
            }
        } else if (uniform.second.type() == typeid(glm::mat3)) {
            type_value.SetString("Mat3", allocator);
            auto mat = std::any_cast<glm::mat3>(uniform.second);
            data_value.SetArray();
            for (int c = 0; c < 3; ++c) {
                rapidjson::Value col(rapidjson::kArrayType);
                col.PushBack(mat[c][0], allocator);
                col.PushBack(mat[c][1], allocator);
                col.PushBack(mat[c][2], allocator);
                data_value.PushBack(col, allocator);
            }
        } else if (uniform.second.type() == typeid(glm::mat4)) {
            type_value.SetString("Mat4", allocator);
            auto mat = std::any_cast<glm::mat4>(uniform.second);
            data_value.SetArray();
            for (int c = 0; c < 4; ++c) {
                rapidjson::Value col(rapidjson::kArrayType);
                col.PushBack(mat[c][0], allocator);
                col.PushBack(mat[c][1], allocator);
                col.PushBack(mat[c][2], allocator);
                col.PushBack(mat[c][3], allocator);
                data_value.PushBack(col, allocator);
            }
        } else {
            continue;
        }

        uniform_obj.AddMember("type", type_value, allocator);
        uniform_obj.AddMember("value", data_value, allocator);
        uniforms_json.AddMember(uniform_key, uniform_obj, allocator);
    }
    render_data.AddMember("uniforms", uniforms_json, allocator);
    component_json.AddMember("render_data", render_data, allocator);
}

auto buildRenderPassComponent(const rapidjson::Value& componentData) -> std::optional<std::unique_ptr<RenderPassComponent>>
{
    Logger::debug(__FUNCTION__);

    auto id = componentData["id"].GetUint();
    auto name = componentData["name"].GetString();
    auto owner_node = componentData["owner_node"].GetUint();
    auto owner_scene = componentData["owner_scene"].GetUint();

    auto component = std::make_unique<RenderPassComponent>(id, name, owner_node, owner_scene);
    component->setRenderPassName(componentData["render_pass_name"].GetString());

    return component;
}

void saveRenderPassComponent(const std::shared_ptr<RenderPassComponent>& component, rapidjson::Value& component_json, rapidjson::Document::AllocatorType& allocator)
{
    rapidjson::Value name_value;
    name_value.SetString(component->name().c_str(), allocator);
    component_json.AddMember("type", "render_pass", allocator);
    component_json.AddMember("id", component->id(), allocator);
    component_json.AddMember("name", name_value, allocator);
    component_json.AddMember("owner_node", component->ownerNode(), allocator);
    component_json.AddMember("owner_scene", component->ownerScene(), allocator);
    rapidjson::Value render_pass_name_value;
    render_pass_name_value.SetString(component->renderPassName().c_str(), allocator);
    component_json.AddMember("render_pass_name", render_pass_name_value, allocator);
}

auto buildLightSourceComponent(const rapidjson::Value& componentData) -> std::optional<std::unique_ptr<LightSourceComponent>>
{
    Logger::debug(__FUNCTION__);

    auto id = componentData["id"].GetUint();
    auto name = componentData["name"].GetString();
    auto owner_node = componentData["owner_node"].GetUint();
    auto owner_scene = componentData["owner_scene"].GetUint();

    auto component = std::make_unique<LightSourceComponent>(id, name, owner_node, owner_scene);
    component->setColor(glm::vec3(componentData["color"].GetArray()[0].GetFloat(), componentData["color"].GetArray()[1].GetFloat(), componentData["color"].GetArray()[2].GetFloat()));
    component->setIntensity(componentData["intensity"].GetFloat());

    return component;
}

void saveLightSourceComponent(const std::shared_ptr<LightSourceComponent>& component, rapidjson::Value& component_json, rapidjson::Document::AllocatorType& allocator)
{
    rapidjson::Value value;
    value.SetString(component->name().c_str(), allocator);
    component_json.AddMember("type", "light_source", allocator);
    component_json.AddMember("id", component->id(), allocator);
    component_json.AddMember("name", value, allocator);
    component_json.AddMember("owner_node", component->ownerNode(), allocator);
    component_json.AddMember("owner_scene", component->ownerScene(), allocator);
    rapidjson::Value color_value(rapidjson::kArrayType);
    color_value.PushBack(component->color().x, allocator);
    color_value.PushBack(component->color().y, allocator);
    color_value.PushBack(component->color().z, allocator);
    component_json.AddMember("color", color_value, allocator);
    component_json.AddMember("intensity", component->intensity(), allocator);
}

auto ComponentBuilder::componentTypes() -> const std::vector<std::string>&
{
    static const std::vector<std::string> types = {"material", "mesh", "camera", "transform", "flipbook_animation", "mouse_event_filter", "render_scope", "render_pass", "light_source"};
    return types;
}

auto ComponentBuilder::buildFromJson(const std::string& type, const rapidjson::Value& component) -> std::optional<std::unique_ptr<Component>>
{
    Logger::debug(std::string(__FUNCTION__) + " component type: {}", type);

    if (type == "material") {
        return buildMaterialComponent(component);
    } else if (type == "mesh") {
        return buildMeshComponent(component);
    } else if (type == "camera") {
        return buildCameraComponent(component);
    } else if (type == "transform") {
        return buildTransformComponent(component);
    } else if (type == "flipbook_animation") {
        return buildFlipbookAnimationComponent(component);
    } else if (type == "mouse_event_filter") {
        return buildMouseEventFilterComponent(component);
    } else if (type == "render_scope") {
        return buildRenderScopeComponent(component);
    } else if (type == "render_pass") {
        return buildRenderPassComponent(component);
    } else if (type == "light_source") {
        return buildLightSourceComponent(component);
    }

    return std::nullopt;
}

void ComponentBuilder::saveToJson(const std::shared_ptr<Component>& component, rapidjson::Value& component_json, rapidjson::Document::AllocatorType& allocator)
{
    if (component->type() == "material") {
        saveMaterialComponent(std::dynamic_pointer_cast<MaterialComponent>(component), component_json, allocator);
    } else if (component->type() == "mesh") {
        saveMeshComponent(std::dynamic_pointer_cast<MeshComponent>(component), component_json, allocator);
    } else if (component->type() == "camera") {
        saveCameraComponent(std::dynamic_pointer_cast<CameraComponent>(component), component_json, allocator);
    } else if (component->type() == "transform") {
        saveTransformComponent(std::dynamic_pointer_cast<TransformComponent>(component), component_json, allocator);
    } else if (component->type() == "flipbook_animation") {
        saveFlipbookAnimationComponent(std::dynamic_pointer_cast<FlipbookAnimationComponent>(component), component_json, allocator);
    } else if (component->type() == "mouse_event_filter") {
        saveMouseEventFilterComponent(std::dynamic_pointer_cast<MouseEventFilterComponent>(component), component_json, allocator);
    } else if (component->type() == "render_scope") {
        saveRenderScopeComponent(std::dynamic_pointer_cast<RenderScopeComponent>(component), component_json, allocator);
    } else if (component->type() == "render_pass") {
        saveRenderPassComponent(std::dynamic_pointer_cast<RenderPassComponent>(component), component_json, allocator);
    } else if (component->type() == "light_source") {
        saveLightSourceComponent(std::dynamic_pointer_cast<LightSourceComponent>(component), component_json, allocator);
    }
}

auto ComponentBuilder::buildEmptyComponent(const std::string& type, const std::string& name, uint32_t owner_node, uint32_t owner_scene) -> std::optional<std::unique_ptr<Component>>
{
    Logger::debug(std::string(__FUNCTION__) + " component type: {}", type);

    if (type == "material") {
        return std::make_unique<MaterialComponent>(generateUniqueId(), name, owner_node, owner_scene);
    } else if (type == "mesh") {
        return std::make_unique<MeshComponent>(generateUniqueId(), name, owner_node, owner_scene);
    } else if (type == "camera") {
        return std::make_unique<CameraComponent>(generateUniqueId(), name, owner_node, owner_scene);
    } else if (type == "transform") {
        return std::make_unique<TransformComponent>(generateUniqueId(), name, owner_node, owner_scene);
    } else if (type == "flipbook_animation") {
        return std::make_unique<FlipbookAnimationComponent>(generateUniqueId(), name, owner_node, owner_scene);
    } else if (type == "mouse_event_filter") {
        return std::make_unique<MouseEventFilterComponent>(generateUniqueId(), name, owner_node, owner_scene);
    } else if (type == "render_scope") {
        return std::make_unique<RenderScopeComponent>(generateUniqueId(), name, owner_node, owner_scene);
    } else if (type == "render_pass") {
        return std::make_unique<RenderPassComponent>(generateUniqueId(), name, owner_node, owner_scene);
    } else if (type == "light_source") {
        return std::make_unique<LightSourceComponent>(generateUniqueId(), name, owner_node, owner_scene);
    }

    return std::nullopt;
}

template<>
std::optional<std::unique_ptr<CameraComponent>> ComponentBuilder::buildEmptyComponent<CameraComponent>(const std::string& name, uint32_t owner_node, uint32_t owner_scene)
{
    return std::make_unique<CameraComponent>(generateUniqueId(), name, owner_node, owner_scene);
}

template<>
std::optional<std::unique_ptr<MeshComponent>> ComponentBuilder::buildEmptyComponent<MeshComponent>(const std::string& name, uint32_t owner_node, uint32_t owner_scene)
{
    return std::make_unique<MeshComponent>(generateUniqueId(), name, owner_node, owner_scene);
}

template<>
std::optional<std::unique_ptr<MaterialComponent>> ComponentBuilder::buildEmptyComponent<MaterialComponent>(const std::string& name, uint32_t owner_node, uint32_t owner_scene)
{
    return std::make_unique<MaterialComponent>(generateUniqueId(), name, owner_node, owner_scene);
}

template<>
std::optional<std::unique_ptr<TransformComponent>> ComponentBuilder::buildEmptyComponent<TransformComponent>(const std::string& name, uint32_t owner_node, uint32_t owner_scene)
{
    return std::make_unique<TransformComponent>(generateUniqueId(), name, owner_node, owner_scene);
}

template<>
std::optional<std::unique_ptr<FlipbookAnimationComponent>> ComponentBuilder::buildEmptyComponent<FlipbookAnimationComponent>(const std::string& name, uint32_t owner_node, uint32_t owner_scene)
{
    return std::make_unique<FlipbookAnimationComponent>(generateUniqueId(), name, owner_node, owner_scene);
}

template <>
std::optional<std::unique_ptr<MouseEventFilterComponent>> ComponentBuilder::buildEmptyComponent<MouseEventFilterComponent>(const std::string& name, uint32_t owner_node, uint32_t owner_scene)
{
    return std::make_unique<MouseEventFilterComponent>(generateUniqueId(), name, owner_node, owner_scene);
}

template<>
std::optional<std::unique_ptr<RenderScopeComponent>> ComponentBuilder::buildEmptyComponent<RenderScopeComponent>(const std::string& name, uint32_t owner_node, uint32_t owner_scene)
{
    return std::make_unique<RenderScopeComponent>(generateUniqueId(), name, owner_node, owner_scene);
}

template<>
std::optional<std::unique_ptr<RenderPassComponent>> ComponentBuilder::buildEmptyComponent<RenderPassComponent>(const std::string& name, uint32_t owner_node, uint32_t owner_scene)
{
    return std::make_unique<RenderPassComponent>(generateUniqueId(), name, owner_node, owner_scene);
}

template<>
std::optional<std::unique_ptr<LightSourceComponent>> ComponentBuilder::buildEmptyComponent<LightSourceComponent>(const std::string& name, uint32_t owner_node, uint32_t owner_scene)
{
    return std::make_unique<LightSourceComponent>(generateUniqueId(), name, owner_node, owner_scene);
}

}
