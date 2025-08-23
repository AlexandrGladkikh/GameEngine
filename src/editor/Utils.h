#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

class QHBoxLayout;

namespace editor {

class ComponentWidget;

struct EditorBlockLayoutData {
    std::string name;
    std::string value;
    std::function<void(const std::string&)> changeHandler;
    std::function<std::string()> updateHandler;
};

ComponentWidget* createLabelLineEditorWidget(const std::string& label, const std::string& value,
    const std::function<void(const std::string&)>& changeHandler, const std::function<std::string()>& updateHandler);

ComponentWidget* createButtonLineWidget(const std::vector<std::string>& names, const std::vector<std::function<void()>>& handlers);

QHBoxLayout* createEditorBlockLayout(const std::string& title, const std::vector<EditorBlockLayoutData>& data);
void setupEditorBlockLayout(QHBoxLayout* layout, const std::string& title, const std::vector<EditorBlockLayoutData>& data);

std::string formatFloat(float value);
bool parseFloat(const std::string& value, float& out);

std::string formatUint32(uint32_t value);
bool parseUint32(const std::string& value, uint32_t& out);

}