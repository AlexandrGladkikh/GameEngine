#include "SceneTransition.h"
#include "Context.h"
#include "SceneStore.h"
#include "Scene.h"

namespace engine {

bool SceneTransition::transition(const std::string& scene_id_from, const std::string& scene_id_to)
{
    auto scene_from = m_context->sceneStore->get(scene_id_from);
    auto scene_to = m_context->sceneStore->get(scene_id_to);
    if (!scene_from.has_value() || !scene_from.value()->isActive() || !scene_to.has_value() || !scene_to.value()->isActive()) {
            return false;
    }

    scene_from.value()->setActive(false);

    auto resources = scene_from.value()->getResources();

    return true;
}

}