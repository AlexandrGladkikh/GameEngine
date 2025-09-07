#pragma once

#include <QMainWindow>

namespace engine {
class Engine;
}

class QListView;
class QSplitter;
class QListWidget;

namespace editor {

class ResourcePackagesItemModel;

class ResourcePackagesEditor : public QMainWindow {
    Q_OBJECT

public:
    explicit ResourcePackagesEditor(engine::Engine* engine, QWidget* parent = nullptr);

private:
    void loadContent();

    engine::Engine* m_engine;

    QSplitter* m_content_splitter;

    QListView* m_package_content;
    ResourcePackagesItemModel* m_item_model;

    QListWidget* m_packages_list;
};

}
