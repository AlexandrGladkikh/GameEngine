#include "EngineController.h"

#include "engine/Engine.h"

#include <QWidget>
#include <QTimer>

namespace editor {

EngineController::EngineController(engine::Engine* engine, QWidget* parent)
{
    m_timer = new QTimer(parent);
    m_timer->setInterval(10);
    m_timer->setTimerType(Qt::PreciseTimer);
    auto update_time = std::chrono::steady_clock::now();
    QObject::connect(m_timer, &QTimer::timeout, [this, engine, update_time]() mutable {
        for (auto& handler : m_handlers) {
            handler.second();
        }
        
        const auto now = std::chrono::steady_clock::now();
        const uint64_t delta_time = static_cast<uint64_t>(
            std::chrono::duration_cast<std::chrono::microseconds>(now - update_time).count()
        );
        
        engine->performTick(delta_time);
        update_time = now;
    });
}

EngineController::~EngineController()
{
    stop();
}

void EngineController::start()
{
    m_timer->start();
}

void EngineController::stop()
{
    m_timer->stop();
}

void EngineController::addHandler(std::uintptr_t id, std::function<void()> handler)
{
    m_handlers[id] = handler;
}

void EngineController::removeHandler(std::uintptr_t id)
{
    m_handlers.erase(id);
}

}
