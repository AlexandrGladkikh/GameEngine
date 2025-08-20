#include "Utils.h"

#include "ComponentWidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>

namespace editor {

ComponentWidget* createLabelLineEditorWidget(const std::string& label, const std::string& value,
    const std::function<void(const std::string&)>& changeHandler, const std::function<std::string()>& updateHandler)
{
    ComponentWidget* widget = new ComponentWidget;

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
            if (lineEditor->hasFocus()) {
                return;
            }
            auto new_text = lineEditor->text();
            auto current_text = updateHandler();
            if (!current_text.empty() && new_text != current_text) {
                lineEditor->setText(current_text.c_str());
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

bool parseFloat(const std::string& value, float& out)
{
    try {
        out = std::stof(value);
    } catch (std::exception&) {
        return false;
    }

    return true;
}

}