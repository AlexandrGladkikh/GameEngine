#include "ResourcePackagesEditor.h"

#include <engine/Engine.h>
#include <engine/Context.h>
#include <engine/ResourcePackageStore.h>
#include <engine/ResourcePackage.h>

#include <QListView>
#include <QVBoxLayout>
#include <QSplitter>
#include <QListWidget>
#include <QStandardItemModel>
#include <QIcon>

namespace editor {

ResourcePackagesEditor::ResourcePackagesEditor(engine::Engine* engine, QWidget* parent) :
    QMainWindow(parent),
    m_item_model(new QStandardItemModel(this)),
    m_engine(engine)
{
    setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    setWindowTitle("Resource explorer");
    resize(1000, 400);

    QWidget* central_widget = new QWidget(this);
    setCentralWidget(central_widget);

    QVBoxLayout* main_layout = new QVBoxLayout(central_widget);

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

    m_package_content = new QListView;
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

    right_layout->addWidget(m_package_content);

    m_content_splitter->addWidget(left_panel);
    m_content_splitter->addWidget(right_panel);
    m_content_splitter->setStretchFactor(0, 1);
    m_content_splitter->setStretchFactor(1, 3);
    m_content_splitter->setSizes({200, 600});

    main_layout->addWidget(m_content_splitter);

    loadContent();
}

void ResourcePackagesEditor::loadContent()
{
    m_item_model->clear();

    auto context = m_engine->context();

    auto& resource_packages = context->resourcePackageStore->getResourcePackages();

    if (resource_packages.empty()) {
        return;
    }

    for (const auto& [id, package]: resource_packages) {
        QListWidgetItem* item = new QListWidgetItem(package->name.c_str());
        m_packages_list->addItem(item);
    }

    const auto& first_package = resource_packages.begin()->second;

    for (auto& texture_info: first_package->textures) {
        auto texture_name = texture_info.path.stem();
        QIcon texture_icon(QString(texture_info.path.c_str()));
        QStandardItem* item = new QStandardItem(QString(texture_name.c_str()));
        item->setIcon(texture_icon);
        m_item_model->appendRow(item);
    }
}

}
