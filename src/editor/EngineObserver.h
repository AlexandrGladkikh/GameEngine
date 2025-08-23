#pragma once

#include <unordered_map>
#include <functional>
#include <QTimer>

namespace engine {
class Engine;
}

class QWidget;

namespace editor {

class EngineObserver final {
public:
    explicit EngineObserver(engine::Engine* engine, QWidget* parent = nullptr);
    ~EngineObserver();

    void start();
    void stop();

    void addHandler(std::uintptr_t id, std::function<void()> handler);
    void removeHandler(std::uintptr_t id);

private:
    QTimer* m_timer = nullptr;

    std::unordered_map<std::uintptr_t, std::function<void()>> m_handlers;
};
}
