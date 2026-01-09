#include "Utils.h"

#include "ComponentWidget.h"
#include "EngineObserver.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>
#include <QPushButton>
#include <QMimeData>
#include <QComboBox>

#include <iomanip>

namespace editor {

DropFilter::DropFilter(QObject* parent) :
    QObject(parent)
{

}

bool DropFilter::eventFilter(QObject* obj, QEvent* event)
{
    if (event->type() == QEvent::DragEnter) {
        auto dragEnterEvent = static_cast<QDragEnterEvent*>(event);
        if (dragEnterEvent->mimeData()->hasText()) {
            dragEnterEvent->acceptProposedAction();
            return true;
        }
    }

    if (event->type() == QEvent::Drop) {
        auto dropEvent = static_cast<QDropEvent*>(event);
        if (dropEvent->mimeData()->hasText()) {
            auto line = qobject_cast<QLineEdit*>(obj);
            if (line) {
                line->setText(dropEvent->mimeData()->text());
                dropEvent->acceptProposedAction();
                return true;
            }
        }
    }

    return QObject::eventFilter(obj, event);
}

ComponentWidget* createLabelLineEditorWidget(const EditorBlockLayoutData& data, const std::shared_ptr<EngineObserver>& observer)
{
    ComponentWidget* widget = new ComponentWidget;

    QHBoxLayout* layout = new QHBoxLayout;
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(new QLabel(data.name.c_str()));

    if (data.showComboBox) {
        QComboBox* combo = new QComboBox;
        combo->setEditable(true);
        combo->setInsertPolicy(QComboBox::NoInsert);

        for (const auto& value : data.comboBoxValues) {
            combo->addItem(value.c_str());
        }

        combo->setCurrentText(data.value.c_str());
        if (data.acceptDrag) {
            if (auto* lineEditor = combo->lineEdit()) {
                lineEditor->setAcceptDrops(true);
                DropFilter* filter = new DropFilter(lineEditor);
                lineEditor->installEventFilter(filter);
            }
        }

        QObject::connect(combo, &QComboBox::currentTextChanged, [data](const QString& value) {
            if (data.changeHandler) data.changeHandler(value.toStdString());
        });
        QObject::connect(combo->lineEdit(), &QLineEdit::textChanged, [data](const QString& value) {
            if (data.changeHandler) data.changeHandler(value.toStdString());
        });
        std::uintptr_t id = reinterpret_cast<uintptr_t>(combo);
        QObject::connect(combo, &QObject::destroyed, [observer, id]() {
            observer->removeHandler(id);
        });

        layout->addWidget(combo);
    } else {
        auto* lineEditor = new QLineEdit(data.value.c_str());
        if (data.acceptDrag) {
            lineEditor->setAcceptDrops(true);
            DropFilter* filter = new DropFilter(lineEditor);
            lineEditor->installEventFilter(filter);
        } else {
            lineEditor->setAcceptDrops(false);
        }

        if (data.updateHandler) {
            std::uintptr_t id = reinterpret_cast<std::uintptr_t>(lineEditor);
            observer->addHandler(id, [lineEditor, data]() {
                if (lineEditor->hasFocus()) {
                    return;
                }
                auto new_text = lineEditor->text();
                auto current_text = data.updateHandler();
                if (!current_text.empty() && new_text != current_text) {
                    lineEditor->setText(current_text.c_str());
                }
            });

            QObject::connect(lineEditor, &QObject::destroyed, [observer, id]() {
                observer->removeHandler(id);
            });
        }
        if (data.changeHandler) {
            QObject::connect(lineEditor, &QLineEdit::textChanged, [data](const QString& value) {
                data.changeHandler(value.toStdString());
            });
        }

        layout->addWidget(lineEditor);
    }

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
        layout->addWidget(createLabelLineEditorWidget(item, observer));
    }

    layout->addStretch();
}

ComponentWidget* createComboBoxWidget(const std::string& title, const std::vector<std::string>& names, const std::function<void(const std::string&)>& handler)
{
    ComponentWidget* widget = new ComponentWidget;
    QHBoxLayout* layout = new QHBoxLayout;
    layout->setSpacing(1);
    layout->setContentsMargins(0, 0, 0, 0);

    auto label = new QLabel(title.c_str());
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    layout->addWidget(label);

    QComboBox* combo = new QComboBox;
    layout->addWidget(combo);

    for (const auto& name : names) {
        combo->addItem(name.c_str());
    }

    QObject::connect(combo, &QComboBox::currentIndexChanged, [combo, handler](int index) {
        handler(combo->currentText().toStdString());
    });

    widget->setLayout(layout);

    return widget;
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
