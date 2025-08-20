#pragma once

#include <functional>
#include <string>

namespace editor {

class ComponentWidget;

ComponentWidget* createLabelLineEditorWidget(const std::string& label, const std::string& value,
    const std::function<void(const std::string&)>& changeHandler, const std::function<std::string()>& updateHandler);

std::string formatFloat(float value);
bool parseFloat(const std::string& value, float& out);

}