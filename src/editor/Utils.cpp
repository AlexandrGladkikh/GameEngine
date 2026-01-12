#include "Utils.h"

#include "ComponentWidget.h"
#include "EngineController.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMimeData>
#include <QPushButton>
#include <QTimer>
#include <QMouseEvent>
#include <QtGlobal>

#include <algorithm>
#include <iomanip>

namespace editor {

namespace {

class LineEditDragValueFilter final : public QObject
{
public:
    LineEditDragValueFilter(QLineEdit* lineEdit, float minValue, float maxValue)
        : QObject(lineEdit)
        , m_lineEdit(lineEdit)
        , m_minValue(minValue)
        , m_maxValue(maxValue)
    {
    }

protected:
    bool eventFilter(QObject* obj, QEvent* event) override
    {
        if (obj != m_lineEdit) {
            return QObject::eventFilter(obj, event);
        }

        switch (event->type()) {
        case QEvent::MouseButtonPress: {
            auto* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() != Qt::LeftButton) {
                return QObject::eventFilter(obj, event);
            }

            float parsedValue = 0.0f;
            if (!parseFloat(m_lineEdit->text().toStdString(), parsedValue)) {
                parsedValue = 0.0f;
            }

            m_startGlobalX = mouseEvent->globalPosition().x();
            m_startValue = std::clamp(parsedValue, m_minValue, m_maxValue);
            m_dragging = true;

            m_lineEdit->setFocus();
            m_lineEdit->selectAll();
            m_lineEdit->grabMouse();
            m_lineEdit->setCursor(Qt::SizeHorCursor);

            return true;
        }
        case QEvent::MouseMove: {
            if (!m_dragging) {
                return QObject::eventFilter(obj, event);
            }
            auto* mouseEvent = static_cast<QMouseEvent*>(event);

            const float currentGlobalX = mouseEvent->globalPosition().x();
            const float dx = currentGlobalX - m_startGlobalX;

            float range = m_maxValue - m_minValue;
            if (range <= 0.000001f) {
                range = 1.0f;
            }

            float stepPerPixel = range / 300.0f;
            const auto mods = mouseEvent->modifiers();
            if (mods.testFlag(Qt::ShiftModifier)) {
                stepPerPixel *= 0.1f;
            }
            if (mods.testFlag(Qt::ControlModifier)) {
                stepPerPixel *= 0.01f;
            }
            if (mods.testFlag(Qt::AltModifier)) {
                stepPerPixel *= 10.0f;
            }

            const float newValue = std::clamp(m_startValue + dx * stepPerPixel, m_minValue, m_maxValue);
            m_lineEdit->setText(QString::fromStdString(formatFloat(newValue)));
            return true;
        }
        case QEvent::MouseButtonRelease: {
            auto* mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() != Qt::LeftButton) {
                return QObject::eventFilter(obj, event);
            }
            if (!m_dragging) {
                return QObject::eventFilter(obj, event);
            }

            m_dragging = false;
            m_lineEdit->releaseMouse();
            m_lineEdit->unsetCursor();
            return true;
        }
        default:
            break;
        }

        return QObject::eventFilter(obj, event);
    }

private:
    QLineEdit* m_lineEdit = nullptr;
    float m_minValue = -1000.0f;
    float m_maxValue = 1000.0f;

    bool m_dragging = false;
    float m_startGlobalX = 0.0f;
    float m_startValue = 0.0f;
};

}

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

ComponentWidget* createLabelLineEditorWidget(const EditorBlockLayoutData& data, const std::shared_ptr<EngineController>& observer)
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

        if (data.enableSlider) {
            auto* filter = new LineEditDragValueFilter(lineEditor, data.sliderMin, data.sliderMax);
            lineEditor->installEventFilter(filter);
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

QHBoxLayout* createEditorBlockLayout(const std::string& title, const std::vector<EditorBlockLayoutData>& data, const std::shared_ptr<EngineController>& observer)
{
    QHBoxLayout* layout = new QHBoxLayout();
    setupEditorBlockLayout(layout, title, data, observer);

    return layout;
}

void setupEditorBlockLayout(QHBoxLayout* layout, const std::string& title, const std::vector<EditorBlockLayoutData>& data, const std::shared_ptr<EngineController>& observer)
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
