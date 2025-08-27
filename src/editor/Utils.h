#pragma once

#include <QObject>
#include <QDropEvent>

#include <cstdint>
#include <functional>
#include <string>
#include <vector>
#include <memory>

class QHBoxLayout;

namespace editor {

class ComponentWidget;
class EngineObserver;

struct EditorBlockLayoutData {
    std::string name;
    std::string value;
    std::function<void(const std::string&)> changeHandler;
    std::function<std::string()> updateHandler;
    bool acceptDrag = false;
};

class DropFilter : public QObject
{
public:
    explicit DropFilter(QObject* parent = nullptr);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
};

ComponentWidget* createLabelLineEditorWidget(const std::string& label, const std::string& value,
    const std::function<void(const std::string&)>& changeHandler, const std::function<std::string()>& updateHandler, const std::shared_ptr<EngineObserver>& observer, bool acceptDrag);

ComponentWidget* createButtonLineWidget(const std::vector<std::string>& names, const std::vector<std::function<void()>>& handlers);

QHBoxLayout* createEditorBlockLayout(const std::string& title, const std::vector<EditorBlockLayoutData>& data, const std::shared_ptr<EngineObserver>& observer);
void setupEditorBlockLayout(QHBoxLayout* layout, const std::string& title, const std::vector<EditorBlockLayoutData>& data, const std::shared_ptr<EngineObserver>& observer);

std::string formatFloat(float value, uint32_t precision = 2);
bool parseFloat(const std::string& value, float& out);

std::string formatUint32(uint32_t value);
bool parseUint32(const std::string& value, uint32_t& out);

}
