#pragma once

#include <QMainWindow>

namespace engine {
class Engine;
}

class QSplitter;
class QListWidget;

namespace editor {

class ResourcePackagesItemModel;
class PackageContentView;

class ResourcePackagesEditor : public QMainWindow {
    Q_OBJECT

public:
    explicit ResourcePackagesEditor(engine::Engine* engine, QWidget* parent = nullptr);

private slots:
    void onDropNewPackageItems(const QStringList& files);

private:
    void loadContent();

    engine::Engine* m_engine;

    QSplitter* m_content_splitter;

    PackageContentView* m_package_content;
    ResourcePackagesItemModel* m_item_model;

    QListWidget* m_packages_list;

    uint32_t m_selected_package_id = -1;
};

}
