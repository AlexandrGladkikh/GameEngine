#include "Utils.h"

#include "ComponentWidget.h"
#include "EngineObserver.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>
#include <QPushButton>

namespace editor {

ComponentWidget* createLabelLineEditorWidget(const std::string& label, const std::string& value,
    const std::function<void(const std::string&)>& changeHandler, const std::function<std::string()>& updateHandler, const std::shared_ptr<EngineObserver>& observer)
{
    ComponentWidget* widget = new ComponentWidget;

    QHBoxLayout* layout = new QHBoxLayout;
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(new QLabel(label.c_str()));
    auto* lineEditor = new QLineEdit(value.c_str());

    if (updateHandler) {
        std::uintptr_t id = reinterpret_cast<std::uintptr_t>(lineEditor);
        observer->addHandler(id, [lineEditor, updateHandler]() {
            if (lineEditor->hasFocus()) {
                return;
            }
            auto new_text = lineEditor->text();
            auto current_text = updateHandler();
            if (!current_text.empty() && new_text != current_text) {
                lineEditor->setText(current_text.c_str());
            }
        });

        QObject::connect(lineEditor, &QObject::destroyed, [observer, id]() {
            observer->removeHandler(id);
        });
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

ComponentWidget *createButtonLineWidget(const std::vector<std::string>& names, const std::vector<std::function<void()>>& handlers)
{
    ComponentWidget* widget = new ComponentWidget;

    QHBoxLayout* layout = new QHBoxLayout;
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);
    for (size_t i = 0; i < names.size(); ++i) {
        auto* button = new QPushButton(names[i].c_str());
        QObject::connect(button, &QPushButton::clicked, handlers[i]);
        layout->addWidget(button);
    }

    widget->setLayout(layout);

    return widget;
}

QHBoxLayout* createEditorBlockLayout(const std::string& title, const std::vector<EditorBlockLayoutData>& data, const std::shared_ptr<EngineObserver>& observer)
{
    QHBoxLayout* layout = new QHBoxLayout();
    setupEditorBlockLayout(layout, title, data, observer);

    return layout;
}

void setupEditorBlockLayout(QHBoxLayout* layout, const std::string& title, const std::vector<EditorBlockLayoutData>& data, const std::shared_ptr<EngineObserver>& observer)
{
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);
    if (data.size() > 1) {
        auto label = new QLabel(title.c_str());
        label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        layout->addWidget(label);
    }

    for (const auto& item : data) {
        layout->addWidget(createLabelLineEditorWidget(item.name, item.value, item.changeHandler, item.updateHandler, observer));
    }

    layout->addStretch();
}

std::string formatFloat(float value, uint32_t precision)
{
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}

bool parseFloat(const std::string& value, float& out)
{
    try {
        out = std::stof(value);
    } catch (std::exception&) {
        return false;
    }

    return true;
}


std::string formatUint32(uint32_t value)
{
    std::ostringstream ss;
    ss << std::fixed << value;
    return ss.str();
}

bool parseUint32(const std::string& value, uint32_t& out)
{
    try {
        out = std::stoul(value);
    } catch (std::exception&) {
        return false;
    }

    return true;
}

}
