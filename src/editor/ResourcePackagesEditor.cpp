#include "ResourcePackagesEditor.h"

#include "ResourcePackagesItemModel.h"
#include "PackageContentView.h"

#include <engine/Engine.h>
#include <engine/Context.h>
#include <engine/ResourcePackageStore.h>
#include <engine/ResourcePackage.h>
#include <engine/Utils.h>
#include <engine/SceneStore.h>
#include <engine/Scene.h>

#include <QListView>
#include <QVBoxLayout>
#include <QSplitter>
#include <QListWidget>
#include <QStandardItemModel>
#include <QIcon>

namespace editor {

ResourcePackagesEditor::ResourcePackagesEditor(engine::Engine* engine, QWidget* parent) :
    QMainWindow(parent),
    m_item_model(new ResourcePackagesItemModel(this)),
    m_engine(engine)
{
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    setWindowTitle("Resource explorer");
    resize(1000, 400);

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* main_layout = new QVBoxLayout(centralWidget);

    m_content_splitter = new QSplitter(Qt::Horizontal, this);

    QWidget* left_panel = new QWidget;
    QVBoxLayout* left_layout = new QVBoxLayout(left_panel);
    left_layout->setContentsMargins(5, 5, 5, 5);

    m_packages_list = new QListWidget;
    m_packages_list->setFrameStyle(QFrame::StyledPanel);
    m_packages_list->setStyleSheet(R"(
        QListWidget {
            background-color: #f5f5f5;
            border: 1px solid #d0d0d0;
            border-radius: 3px;
        }
        QListWidget::item {
            padding: 8px;
            border-bottom: 1px solid #e0e0e0;
        }
        QListWidget::item::selected {
            background-color: #0078d4;
            color: white;
        }
        QListWidget::item::hover {
            background-color: #e5f3ff;
            color: black;
        }
    )");

    left_layout->addWidget(m_packages_list);

    QWidget* right_panel = new QWidget;
    QVBoxLayout* right_layout = new QVBoxLayout(right_panel);
    right_layout->setContentsMargins(5, 5, 5, 5);

    m_package_content = new PackageContentView;
    m_package_content->setModel(m_item_model);
    m_package_content->setViewMode(QListView::IconMode);
    m_package_content->setIconSize(QSize(64, 64));
    m_package_content->setGridSize(QSize(100, 100));
    m_package_content->setSpacing(10);
    m_package_content->setResizeMode(QListView::Adjust);
    m_package_content->setMovement(QListView::Static);
    m_package_content->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_package_content->setWordWrap(true);
    m_package_content->setTextElideMode(Qt::ElideMiddle);
    m_package_content->setSelectionMode(QAbstractItemView::SingleSelection);

    m_package_content->setStyleSheet(R"(
        QListView {
            background-color: white;
            border: 1px solid #d0d0d0;
            border-radius: 3px;
        }
        QListView::item {
            padding: 5px;
            border-radius: 3px;
        }
        QListView::item::selected {
            background-color: #cce8ff;
            color: black;
        }
        QListView::item::hover {
            background-color: #e5f3ff;
        }
    )");

    m_package_content->setAcceptDrops(true);
    m_package_content->viewport()->setAcceptDrops(true);

    m_package_content->setDragEnabled(true);
    m_package_content->setDefaultDropAction(Qt::CopyAction);
    m_package_content->setDragDropMode(QAbstractItemView::DragDrop);

    connect(m_package_content, &PackageContentView::filesDropped, this, &ResourcePackagesEditor::onDropNewPackageItems);

    right_layout->addWidget(m_package_content);

    m_content_splitter->addWidget(left_panel);
    m_content_splitter->addWidget(right_panel);
    m_content_splitter->setStretchFactor(0, 1);
    m_content_splitter->setStretchFactor(1, 3);
    m_content_splitter->setSizes({200, 600});

    main_layout->addWidget(m_content_splitter);

    loadContent();
}

void ResourcePackagesEditor::updateContent()
{
    loadContent();
}

void ResourcePackagesEditor::onDropNewPackageItems(const QStringList& files)
{
    auto context = m_engine->context();
    auto resource_package = context->resourcePackageStore->get(m_selected_package_id);

    if (!resource_package.has_value()) {
        return;
    }

    for (const auto& file: files) {
        std::filesystem::path texture_path(file.toStdString());

        engine::ResourceInfo resource_info;
        resource_info.id = engine::generateUniqueId();
        resource_info.path = texture_path;
        resource_package.value()->textures.push_back(resource_info);

        auto texture_name = QString(texture_path.stem().c_str());
        QIcon texture_icon(QString(texture_path.c_str()));
        QStandardItem* item = new QStandardItem(texture_name);
        item->setIcon(texture_icon);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt:: ItemIsDragEnabled);
        item->setData(texture_name, Qt::DisplayRole);
        item->setData(texture_name, Qt::UserRole);

        m_item_model->appendRow(item);
    }

    m_engine->needReloadResourcePackage(m_selected_package_id);

    auto path = context->resourcePackageStore->getResourcePackageInformation(m_selected_package_id);
    if (path.has_value()) {
        engine::saveResourcePackage(resource_package.value(), m_selected_package_id, path.value());
    }

    m_selected_package_id = -1;
}

void ResourcePackagesEditor::loadContent()
{
    m_item_model->clear();
    m_packages_list->clear();

    auto active_scene = m_engine->context()->sceneStore->get(m_engine->getActiveSceneId());
    if (!active_scene.has_value()) {
        return;
    }

    auto& scene_value = active_scene.value();

    auto context = m_engine->context();

    bool selected_package = false;
    for (auto id : scene_value->getResources()) {
        auto package = context->resourcePackageStore->get(id);
        if (!package.has_value()) {
            continue;
        }

        QListWidgetItem* item = new QListWidgetItem(package.value()->name.c_str());
        m_packages_list->addItem(item);

        if (!selected_package) {
            m_selected_package_id = id;
            selected_package = true;
        }
    }

    auto package = context->resourcePackageStore->get(m_selected_package_id);
    if (package.has_value()) {
        for (auto& texture_info: package.value()->textures) {
            auto texture_name = QString(texture_info.path.stem().c_str());
            QIcon texture_icon(QString(texture_info.path.c_str()));
            QStandardItem* item = new QStandardItem(texture_name);
            item->setIcon(texture_icon);
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt:: ItemIsDragEnabled);
            item->setData(texture_name, Qt::DisplayRole);
            item->setData(texture_name, Qt::UserRole);
            m_item_model->appendRow(item);
        }
    }

}

}
