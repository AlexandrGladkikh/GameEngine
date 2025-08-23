#include "EngineObserver.h"

#include "engine/Engine.h"

#include <QWidget>
#include <QTimer>

namespace editor {

EngineObserver::EngineObserver(engine::Engine* engine, QWidget* parent)
{
    m_timer = new QTimer(parent);
    m_timer->setInterval(100);
    m_timer->setTimerType(Qt::PreciseTimer);
    QObject::connect(m_timer, &QTimer::timeout, [this, engine]() {
        engine->pause();

        for (auto& handler : m_handlers) {
            handler.second();
        }

        engine->resume();
    });
}

EngineObserver::~EngineObserver()
{
    stop();
}

void EngineObserver::start()
{
    m_timer->start();
}

void EngineObserver::stop()
{
    m_timer->stop();
}

void EngineObserver::addHandler(std::uintptr_t id, std::function<void()> handler)
{
    m_handlers[id] = handler;
}

void EngineObserver::removeHandler(std::uintptr_t id)
{
    m_handlers.erase(id);
}

}
