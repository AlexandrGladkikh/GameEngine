#pragma once

#include <QMainWindow>

namespace editor {

class ResourcePackagesEditor : public QMainWindow {
    Q_OBJECT

public:
    explicit ResourcePackagesEditor(QWidget* parent = nullptr);
};

}
