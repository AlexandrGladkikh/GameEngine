#include "SceneNodeTree.h"
#include "ComponentWidget.h"
#include "EngineController.h"
#include "NodeTreeWidgetBuilder.h"
#include "NodeWidget.h"
#include "ResourcePackagesEditor.h"
#include "TreeWidgetBuilderHelper.h"
#include "UserTreeWidgetBuilder.h"

#include "engine/Engine.h"
#include "engine/Scene.h"
#include "engine/Node.h"
#include "engine/Component.h"
#include "engine/ComponentBuilder.h"
#include "engine/UserComponentsBuilder.h"
#include "engine/Window.h"
#include "engine/SceneConfig.h"
#include "engine/EngineSettings.h"
#include "engine/Context.h"
#include "engine/ResourcePackage.h"
#include "engine/ResourcePackageStore.h"

#include <QTreeWidget>
#include <QAction>
#include <QInputDialog>
#include <QHeaderView>
#include <QMenuBar>
#include <QClipboard>
#include <QApplication>
#include <QWindow>

#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QVBoxLayout>

#include <limits>

namespace {
[[maybe_unused]] void _editorIncludeSanity(engine::SceneConfig*, engine::EngineSettings*) {}
}

namespace editor {

SceneNodeTree::SceneNodeTree(engine::Engine* engine, QWidget* parent) :
    QMainWindow(parent),
    m_engine(engine),
    m_scene_tree(new QTreeWidget(this))
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle("Scene node tree");
    resize(500, 800);

    m_resource_package_editor = new ResourcePackagesEditor(m_engine, this);

    m_engine_controller.reset(new EngineController(m_engine, this));
    m_engine_controller->start();

    m_scene_tree_widget_builder = std::make_unique<TreeWidgetBuilder>(this, m_engine_controller);

    initMenuBar();

    setCentralWidget(m_scene_tree);

    m_scene_tree->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_scene_tree, &QTreeWidget::customContextMenuRequested, this, &SceneNodeTree::onCustomContextMenu);

    auto header = m_scene_tree->header();
    header->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(header, &QHeaderView::customContextMenuRequested, this, &SceneNodeTree::onHeaderContextMenu);

    initContextMenu();
    initHeaderContextMenu();

    m_resource_package_editor->show();
}

SceneNodeTree::~SceneNodeTree()
{
    m_engine_controller->stop();
    auto ctx = m_engine->context();
    ctx->engineAccessor->stop();
}

void SceneNodeTree::setUserTreeWidgetBuilder(std::unique_ptr<UserTreeWidgetBuilder> builder)
{
    m_user_tree_widget_builder = std::move(builder);
    m_user_tree_widget_builder->setSceneNodeTree(this);
    m_user_tree_widget_builder->setEngineObserver(m_engine_controller);
    m_user_tree_widget_builder->setTreeWidgetBuilderHelper(std::make_shared<TreeWidgetBuilderHelper>(m_engine_controller));
}

void SceneNodeTree::onHeaderContextMenu(const QPoint& pos)
{
    int logical_index = m_scene_tree->header()->logicalIndexAt(pos);
    if (logical_index < 0) {
        return;
    }

    m_header_context_menu->popup(m_scene_tree->viewport()->mapToGlobal(pos));
}

void SceneNodeTree::onAddRootNode()
{
    bool ok = false;
    QString root_node_name = QInputDialog::getText(this, "Add root node",
        "Node name", QLineEdit::Normal, "New root node", &ok);

    if (!ok || root_node_name.isEmpty()) {
        return;
    }

    auto ctx = m_engine->context();
    auto active_scene_id = m_engine->getActiveSceneId();

    auto active_scene = ctx->sceneStore->get(active_scene_id);
    if (!active_scene.has_value()) {
        return;
    }

    auto new_root_node = active_scene.value()->createRootNode(root_node_name.toStdString());

    if (!new_root_node.has_value()) {
        return;
    }

    auto new_root_node_value = new_root_node.value();

    QTreeWidgetItem* nodeItem = new QTreeWidgetItem(m_scene_tree);

    auto new_node_widget = m_scene_tree_widget_builder->buildWidgetForNode(new_root_node_value);
    if (!new_node_widget.has_value()) {
        return;
    }

    new_node_widget.value()->setNode(new_root_node_value);
    m_scene_tree->setItemWidget(nodeItem, 0, new_node_widget.value());
}

void SceneNodeTree::onCustomContextMenu(const QPoint& pos)
{
    QTreeWidgetItem* item = m_scene_tree->itemAt(pos);
    if (item == nullptr) {
        return;
    }

    m_selected_item = item;

    auto widget = m_scene_tree->itemWidget(item, 0);

    bool is_node_widget = dynamic_cast<NodeWidget*>(widget) != nullptr;
    bool is_component_widget = dynamic_cast<ComponentWidget*>(widget) != nullptr;

    if (is_node_widget) {
        m_add_child_node_action->setEnabled(true);
        m_remove_node_action->setEnabled(true);
        m_add_component_menu->setEnabled(true);
        m_remove_component_action->setEnabled(false);
        m_paste->setEnabled(m_to_copy_item != nullptr);
        m_rename->setEnabled(true);
    }

    if (is_component_widget) {
        m_add_child_node_action->setEnabled(false);
        m_remove_node_action->setEnabled(false);
        m_add_component_menu->setEnabled(false);
        m_remove_component_action->setEnabled(true);
        m_paste->setEnabled(false);
        m_rename->setEnabled(false);
    }

    m_context_menu->popup(m_scene_tree->viewport()->mapToGlobal(pos));
}

void SceneNodeTree::onAddChildNode()
{
    bool ok = false;
    QString new_node_name = QInputDialog::getText(this, "Add child node",
        "Node name", QLineEdit::Normal, "New node", &ok);

    if (!ok || new_node_name.isEmpty()) {
        return;
    }

    auto widget = m_scene_tree->itemWidget(m_selected_item, 0);
    NodeWidget* node_widget = dynamic_cast<NodeWidget*>(widget);
    auto new_node = node_widget->node()->addChild(new_node_name.toStdString());

    QTreeWidgetItem* nodeItem = new QTreeWidgetItem(m_selected_item);

    auto new_node_widget = m_scene_tree_widget_builder->buildWidgetForNode(new_node);
    if (!new_node_widget.has_value()) {
        return;
    }

    new_node_widget.value()->setNode(new_node);
    m_scene_tree->setItemWidget(nodeItem, 0, new_node_widget.value());

    m_selected_item = nullptr;
}

void SceneNodeTree::onRemoveNode()
{
    auto widget = m_scene_tree->itemWidget(m_selected_item, 0);
    NodeWidget* node_widget = dynamic_cast<NodeWidget*>(widget);

    node_widget->node()->getScene().value()->removeNode(node_widget->node()->id());
    delete m_selected_item;

    m_selected_item = nullptr;
}

void SceneNodeTree::onAddComponent()
{
    auto action = qobject_cast<QAction*>(sender());
    if (action == nullptr) {
        return;
    }

    bool ok = false;
    QString new_component_name = QInputDialog::getText(this, "Add component",
        "Component name", QLineEdit::Normal, action->text(), &ok);

    if (!ok || new_component_name.isEmpty()) {
        return;
    }

    auto widget = m_scene_tree->itemWidget(m_selected_item, 0);
    auto node = dynamic_cast<NodeWidget*>(widget)->node();
    auto component_type = action->text().toStdString();

    auto component = node->addComponent(component_type, new_component_name.toStdString());

    if (!component.has_value()) {
        return;
    }

    auto component_value = component.value();

    int insertIndex = m_selected_item->childCount();
    for (int i = 0; i < m_selected_item->childCount(); ++i) {
        auto* childWidget = m_scene_tree->itemWidget(m_selected_item->child(i), 0);
        if (dynamic_cast<NodeWidget*>(childWidget) != nullptr) {
            insertIndex = i;
            break;
        }
    }
    
    auto* component_item = new QTreeWidgetItem();
    m_selected_item->insertChild(insertIndex, component_item);

    auto component_widget = m_scene_tree_widget_builder->buildWidgetForComponent(component_value, component_item);
    if (!component_widget.has_value()) {
        component_widget = m_user_tree_widget_builder->buildWidgetForComponent(component_value);
    }
    if (!component_widget.has_value()) {
        return;
    }

    component_widget.value()->setComponent(component_value);

    m_scene_tree->setItemWidget(component_item, 0, component_widget.value());

    m_selected_item = nullptr;
}

void SceneNodeTree::onRemoveComponent()
{
    auto widget = m_scene_tree->itemWidget(m_selected_item, 0);
    auto* component_widget = dynamic_cast<ComponentWidget*>(widget);
    auto node = component_widget->component()->getNode();
    if (!node.has_value()) {
        return;
    }
    node.value()->removeComponent(component_widget->component()->id());
    delete m_selected_item;

    m_selected_item = nullptr;
}

void SceneNodeTree::onCopyId()
{
    auto widget = m_scene_tree->itemWidget(m_selected_item, 0);
    auto* component_widget = dynamic_cast<ComponentWidget*>(widget);
    if (component_widget != nullptr) {
        auto component = component_widget->component();
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText(QString::fromStdString(std::to_string(component->id())));
    }

    auto* node_widget = dynamic_cast<NodeWidget*>(widget);
    if (node_widget != nullptr) {
        auto node = node_widget->node();
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText(QString::fromStdString(std::to_string(node->id())));
    }

    m_selected_item = nullptr;
}

void SceneNodeTree::onCopyName()
{
    auto widget = m_scene_tree->itemWidget(m_selected_item, 0);
    auto* component_widget = dynamic_cast<ComponentWidget*>(widget);
    if (component_widget != nullptr) {
        auto component = component_widget->component();
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText(QString::fromStdString(component->name()));
    }

    auto* node_widget = dynamic_cast<NodeWidget*>(widget);
    if (node_widget != nullptr) {
        auto node = node_widget->node();
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText(QString::fromStdString(node->name()));
    }

    m_selected_item = nullptr;
}

void SceneNodeTree::onCopy()
{
    m_to_copy_item = m_selected_item;

    m_selected_item = nullptr;
}

void SceneNodeTree::onPaste()
{
    auto widget = m_scene_tree->itemWidget(m_to_copy_item, 0);

    auto* selected_widget = m_scene_tree->itemWidget(m_selected_item, 0);

    auto* component_widget = dynamic_cast<ComponentWidget*>(widget);
    auto* selected_component_widget = dynamic_cast<NodeWidget*>(selected_widget);
    if (component_widget != nullptr && selected_component_widget != nullptr) {
        auto parent_node = selected_component_widget->node();
        auto component = component_widget->component();
        auto node = component->getNode();
        if (node.has_value()) {
            std::shared_ptr<engine::Component> clone = component->clone(parent_node->id());
            parent_node->addComponent(clone->id());
            auto scene = node.value()->getScene();
            if (scene.has_value()) {
                scene.value()->addComponent(clone->id(), clone);

                int insertIndex = m_selected_item->childCount();
                for (int i = 0; i < m_selected_item->childCount(); ++i) {
                    auto* childWidget = m_scene_tree->itemWidget(m_selected_item->child(i), 0);
                    if (dynamic_cast<NodeWidget*>(childWidget) != nullptr) {
                        insertIndex = i;
                        break;
                    }
                }

                auto* clone_item = new QTreeWidgetItem();
                m_selected_item->insertChild(insertIndex, clone_item);
                createComponentWidget(clone_item, clone);
            }
        }
    }

    {
        auto* node_widget = dynamic_cast<NodeWidget*>(widget);
        auto* selected_node_widget = dynamic_cast<NodeWidget*>(selected_widget);
        if (node_widget != nullptr && selected_node_widget != nullptr) {
            auto parent_node = selected_node_widget->node();
            auto node = node_widget->node();
            auto clone = node->clone(parent_node->id());
            if (clone.has_value()) {
                std::shared_ptr<engine::Node> clone_value = std::move(clone.value());
                auto scene = node->getScene();
                if (scene.has_value()) {
                    scene.value()->addNode(clone_value->id(), clone_value);

                    createNodeWidget(clone_value, m_selected_item);
                }
            }
        }
    }

    m_to_copy_item = nullptr;
    m_selected_item = nullptr;
}

void SceneNodeTree::onRename()
{
    auto* selected_widget = m_scene_tree->itemWidget(m_selected_item, 0);
    auto node_widget = dynamic_cast<NodeWidget*>(selected_widget);
    if (node_widget != nullptr) {
        auto node = node_widget->node();
        bool ok = false;
        QString new_node_name = QInputDialog::getText(this, "Rename node",
            "Node name", QLineEdit::Normal, node->name().c_str(), &ok);
        if (ok && !new_node_name.isEmpty()) {
            node->setName(new_node_name.toStdString());

            auto node_widget = m_scene_tree_widget_builder->buildWidgetForNode(node);
            if (!node_widget.has_value()) {
                return;
            }

            node_widget.value()->setNode(node);
            m_scene_tree->setItemWidget(m_selected_item, 0, node_widget.value());
        }
    }
}

void SceneNodeTree::onDuplicate()
{
    m_to_copy_item = m_selected_item;
    m_selected_item = m_selected_item->parent();

    onPaste();
}

void SceneNodeTree::onEnable()
{
    changeActiveWidget(true);
}

void SceneNodeTree::onDisable()
{
    changeActiveWidget(false);
}

void SceneNodeTree::onSaveScene()
{
    m_engine->saveScene(m_engine->getActiveSceneId());
}

void SceneNodeTree::onSelectScene()
{
    auto scenes_info = m_engine->getScenesInfo();
    QStringList scene_names;

    uint32_t index = 0;
    for (const auto& scene : scenes_info) {
        scene_names.append(QString::fromStdString(scene.second->name()));
        if (scene.second->id() == m_engine->getActiveSceneId()) {
            index = scene_names.size() - 1;
        }
    }

    bool ok = false;
    QString selected_scene = QInputDialog::getItem(this, "Select scene",
        "Scene:", scene_names, index, false, &ok);

    if (!ok || selected_scene.isEmpty()) {
        return;
    }

    uint32_t selected_scene_id = -1;
    for (const auto& scene : scenes_info) {
        if (scene.second->name() == selected_scene.toStdString()) {
            selected_scene_id = scene.second->id();
            break;
        }
    }

    if (selected_scene_id == -1 || selected_scene_id == m_engine->getActiveSceneId()) {
        return;
    }

    m_engine->changeActiveScene(selected_scene_id);
    auto ctx = m_engine->context();
    auto selected = ctx->sceneStore->get(selected_scene_id);
    if (!selected.has_value()) {
        return;
    }
    build(selected);

    m_resource_package_editor->updateContent();
}

void SceneNodeTree::onAddScene()
{
    bool ok = false;
    QString scene_name = QInputDialog::getText(this, "Add scene",
        "Scene name", QLineEdit::Normal, "New scene", &ok);

    if (!ok || scene_name.isEmpty()) {
        return;
    }

    uint32_t id = m_engine->addEmptyScene(scene_name.toStdString(), "../configs/scenes/" + scene_name.toStdString() + ".json");

    m_engine->changeActiveScene(id);

    auto ctx = m_engine->context();
    auto new_scene = ctx->sceneStore->get(scene_name.toStdString());
    if (!new_scene.has_value()) {
        return;
    }

    auto engine_settings = m_engine->getEngineSettings();
    if (!engine_settings) {
        QMessageBox::warning(this, "Engine settings", "EngineSettings is not initialized.");
        return;
    }

    auto& doc = const_cast<rapidjson::Document&>(engine_settings->settings());
    if (!doc.IsObject()) {
        QMessageBox::warning(this, "Engine settings", "Engine config root must be an object.");
        return;
    }

    auto scenes_it = doc.FindMember("scenes");
    if (scenes_it == doc.MemberEnd() || !scenes_it->value.IsArray()) {
        QMessageBox::warning(this, "Engine settings", "Scenes array not found in engine config.");
        return;
    }

    auto scenes_arr = scenes_it->value.GetArray();
    for (const auto& v : scenes_arr) {
        if (!v.IsObject()) {
            continue;
        }
        int id = v["id"].GetInt();
        if (id == new_scene.value()->id()) {
            return;
        }
    }

    auto& alloc = doc.GetAllocator();

    rapidjson::Value o(rapidjson::kObjectType);
    o.AddMember("id", new_scene.value()->id(), alloc);
    rapidjson::Value path_val;
    std::string path = "../configs/scenes/" + scene_name.toStdString() + ".json";
    path_val.SetString(path.c_str(), static_cast<rapidjson::SizeType>(path.size()), alloc);
    o.AddMember("path", path_val, alloc);
    scenes_arr.PushBack(o, alloc);

    engine_settings->save();
    engine_settings->load();

    build(new_scene.value());

    m_resource_package_editor->updateContent();
}

void SceneNodeTree::onEngineSettings()
{
    auto engine_settings = m_engine->getEngineSettings();
    if (!engine_settings) {
        QMessageBox::warning(this, "Engine settings", "EngineSettings is not initialized.");
        return;
    }

    if (!engine_settings->load()) {
        QMessageBox::warning(this, "Engine settings", "Failed to load engine config (see logs).");
        return;
    }

    auto& doc = const_cast<rapidjson::Document&>(engine_settings->settings());
    if (!doc.IsObject()) {
        QMessageBox::warning(this, "Engine settings", "Engine config root must be an object.");
        return;
    }

    int window_width = 800;
    int window_height = 600;
    QString window_title = "Game Engine";
    int gl_major = 4;
    int gl_minor = 1;
    int main_scene_id = 1;
    QString resource_packages = "../configs/packages_info.json";

    struct SceneRow {
        int id = 0;
        QString path;
    };
    std::vector<SceneRow> scenes;

    auto readInt = [&](const rapidjson::Value& obj, const char* key, int def) -> int {
        if (!obj.IsObject()) return def;
        auto it = obj.FindMember(key);
        if (it == obj.MemberEnd()) return def;
        if (it->value.IsInt()) return it->value.GetInt();
        if (it->value.IsUint()) return static_cast<int>(it->value.GetUint());
        if (it->value.IsInt64()) return static_cast<int>(it->value.GetInt64());
        if (it->value.IsUint64()) return static_cast<int>(it->value.GetUint64());
        return def;
    };

    auto readString = [&](const rapidjson::Value& obj, const char* key, const QString& def) -> QString {
        if (!obj.IsObject()) return def;
        auto it = obj.FindMember(key);
        if (it == obj.MemberEnd() || !it->value.IsString()) return def;
        return QString::fromUtf8(it->value.GetString());
    };

    if (auto win_it = doc.FindMember("window"); win_it != doc.MemberEnd() && win_it->value.IsObject()) {
        const rapidjson::Value& win = win_it->value;
        window_width = readInt(win, "width", window_width);
        window_height = readInt(win, "height", window_height);
        window_title = readString(win, "title", window_title);

        if (auto gl_it = win.FindMember("gl_version"); gl_it != win.MemberEnd() && gl_it->value.IsObject()) {
            const rapidjson::Value& gl = gl_it->value;
            gl_major = readInt(gl, "major", gl_major);
            gl_minor = readInt(gl, "minor", gl_minor);
        }
    }

    main_scene_id = readInt(doc, "main_scene_id", main_scene_id);
    resource_packages = readString(doc, "resource_packages", resource_packages);

    if (auto scenes_it = doc.FindMember("scenes"); scenes_it != doc.MemberEnd() && scenes_it->value.IsArray()) {
        for (const auto& v : scenes_it->value.GetArray()) {
            if (!v.IsObject()) {
                continue;
            }
            SceneRow r;
            r.id = readInt(v, "id", 0);
            r.path = readString(v, "path", "");
            scenes.push_back(r);
        }
    }

    QDialog dialog(this);
    dialog.setWindowTitle("Engine settings");
    dialog.setModal(true);
    dialog.resize(700, 500);

    auto* root_layout = new QVBoxLayout(&dialog);

    auto* window_group = new QGroupBox("Window", &dialog);
    auto* window_form = new QFormLayout(window_group);
    auto* width_spin = new QSpinBox(window_group);
    width_spin->setRange(1, 16384);
    width_spin->setValue(window_width);
    auto* height_spin = new QSpinBox(window_group);
    height_spin->setRange(1, 16384);
    height_spin->setValue(window_height);
    auto* title_edit = new QLineEdit(window_title, window_group);

    auto* gl_major_spin = new QSpinBox(window_group);
    gl_major_spin->setRange(1, 10);
    gl_major_spin->setValue(gl_major);
    auto* gl_minor_spin = new QSpinBox(window_group);
    gl_minor_spin->setRange(0, 10);
    gl_minor_spin->setValue(gl_minor);

    window_form->addRow("Width", width_spin);
    window_form->addRow("Height", height_spin);
    window_form->addRow("Title", title_edit);
    window_form->addRow("GL major", gl_major_spin);
    window_form->addRow("GL minor", gl_minor_spin);
    root_layout->addWidget(window_group);

    auto* engine_group = new QGroupBox("Engine", &dialog);
    auto* engine_form = new QFormLayout(engine_group);
    auto* main_scene_spin = new QSpinBox(engine_group);
    main_scene_spin->setRange(0, std::numeric_limits<int>::max());
    main_scene_spin->setValue(main_scene_id);
    auto* packages_edit = new QLineEdit(resource_packages, engine_group);
    engine_form->addRow("Main scene id", main_scene_spin);
    engine_form->addRow("Resource packages", packages_edit);
    root_layout->addWidget(engine_group);

    auto* scenes_group = new QGroupBox("Scenes", &dialog);
    auto* scenes_layout = new QVBoxLayout(scenes_group);

    auto* scenes_table = new QTableWidget(scenes_group);
    scenes_table->setColumnCount(2);
    scenes_table->setHorizontalHeaderLabels(QStringList() << "id" << "path");
    scenes_table->horizontalHeader()->setStretchLastSection(true);
    scenes_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    scenes_table->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed | QAbstractItemView::SelectedClicked);

    bool suppress_table_changes = true;
    scenes_table->setRowCount(static_cast<int>(scenes.size()));
    for (int row = 0; row < scenes_table->rowCount(); ++row) {
        auto* id_item = new QTableWidgetItem(QString::number(scenes[static_cast<size_t>(row)].id));
        auto* path_item = new QTableWidgetItem(scenes[static_cast<size_t>(row)].path);
        scenes_table->setItem(row, 0, id_item);
        scenes_table->setItem(row, 1, path_item);
    }
    suppress_table_changes = false;

    auto* buttons_row = new QHBoxLayout();
    auto* add_scene_btn = new QPushButton("Add", scenes_group);
    auto* remove_scene_btn = new QPushButton("Remove", scenes_group);
    buttons_row->addWidget(add_scene_btn);
    buttons_row->addWidget(remove_scene_btn);
    buttons_row->addStretch();

    scenes_layout->addWidget(scenes_table);
    scenes_layout->addLayout(buttons_row);
    root_layout->addWidget(scenes_group, 1);

    auto* button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    root_layout->addWidget(button_box);

    // Signals -> update конкретных значений (переменных конфига).
    QObject::connect(width_spin, QOverload<int>::of(&QSpinBox::valueChanged), [&window_width](int v) { window_width = v; });
    QObject::connect(height_spin, QOverload<int>::of(&QSpinBox::valueChanged), [&window_height](int v) { window_height = v; });
    QObject::connect(title_edit, &QLineEdit::textChanged, [&window_title](const QString& v) { window_title = v; });
    QObject::connect(gl_major_spin, QOverload<int>::of(&QSpinBox::valueChanged), [&gl_major](int v) { gl_major = v; });
    QObject::connect(gl_minor_spin, QOverload<int>::of(&QSpinBox::valueChanged), [&gl_minor](int v) { gl_minor = v; });
    QObject::connect(main_scene_spin, QOverload<int>::of(&QSpinBox::valueChanged), [&main_scene_id](int v) { main_scene_id = v; });
    QObject::connect(packages_edit, &QLineEdit::textChanged, [&resource_packages](const QString& v) { resource_packages = v; });

    QObject::connect(add_scene_btn, &QPushButton::clicked, [&]() {
        SceneRow r;
        int max_id = 0;
        for (const auto& s : scenes) {
            max_id = std::max(max_id, s.id);
        }
        r.id = max_id + 1;
        r.path = "../configs/main_scene.json";
        scenes.push_back(r);

        suppress_table_changes = true;
        const int row = scenes_table->rowCount();
        scenes_table->insertRow(row);
        scenes_table->setItem(row, 0, new QTableWidgetItem(QString::number(r.id)));
        scenes_table->setItem(row, 1, new QTableWidgetItem(r.path));
        suppress_table_changes = false;
    });

    QObject::connect(remove_scene_btn, &QPushButton::clicked, [&]() {
        const auto ranges = scenes_table->selectedRanges();
        if (ranges.isEmpty()) {
            return;
        }
        const int row = ranges.first().topRow();
        if (row < 0 || row >= scenes_table->rowCount()) {
            return;
        }
        suppress_table_changes = true;
        scenes_table->removeRow(row);
        suppress_table_changes = false;
        if (row >= 0 && row < static_cast<int>(scenes.size())) {
            scenes.erase(scenes.begin() + row);
        }
    });

    QObject::connect(scenes_table, &QTableWidget::itemChanged, [&](QTableWidgetItem* item) {
        if (suppress_table_changes) {
            return;
        }
        if (!item) {
            return;
        }
        const int row = item->row();
        const int col = item->column();
        if (row < 0 || row >= static_cast<int>(scenes.size())) {
            return;
        }
        if (col == 0) {
            bool ok = false;
            int v = item->text().toInt(&ok);
            if (ok) {
                scenes[static_cast<size_t>(row)].id = v;
            }
        } else if (col == 1) {
            scenes[static_cast<size_t>(row)].path = item->text();
        }
    });

    QObject::connect(button_box, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(button_box, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    auto& alloc = doc.GetAllocator();

    auto ensureObject = [&](rapidjson::Value& parent, const char* key) -> rapidjson::Value& {
        auto it = parent.FindMember(key);
        if (it == parent.MemberEnd() || !it->value.IsObject()) {
            if (it != parent.MemberEnd()) {
                parent.RemoveMember(it);
            }
            rapidjson::Value k;
            k.SetString(key, alloc);
            rapidjson::Value obj(rapidjson::kObjectType);
            parent.AddMember(k, obj, alloc);
            it = parent.FindMember(key);
        }
        return it->value;
    };

    auto setInt = [&](rapidjson::Value& parent, const char* key, int v) {
        auto it = parent.FindMember(key);
        if (it == parent.MemberEnd()) {
            rapidjson::Value k;
            k.SetString(key, alloc);
            parent.AddMember(k, v, alloc);
        } else {
            it->value.SetInt(v);
        }
    };

    auto setString = [&](rapidjson::Value& parent, const char* key, const QString& v) {
        const std::string s = v.toStdString();
        auto it = parent.FindMember(key);
        if (it == parent.MemberEnd()) {
            rapidjson::Value k;
            k.SetString(key, alloc);
            rapidjson::Value val;
            val.SetString(s.c_str(), static_cast<rapidjson::SizeType>(s.size()), alloc);
            parent.AddMember(k, val, alloc);
        } else {
            it->value.SetString(s.c_str(), static_cast<rapidjson::SizeType>(s.size()), alloc);
        }
    };

    rapidjson::Value& window = ensureObject(doc, "window");
    setInt(window, "width", window_width);
    setInt(window, "height", window_height);
    setString(window, "title", window_title);

    rapidjson::Value& gl = ensureObject(window, "gl_version");
    setInt(gl, "major", gl_major);
    setInt(gl, "minor", gl_minor);

    setInt(doc, "main_scene_id", main_scene_id);
    setString(doc, "resource_packages", resource_packages);

    rapidjson::Value scenes_arr(rapidjson::kArrayType);
    for (const auto& s : scenes) {
        rapidjson::Value o(rapidjson::kObjectType);
        o.AddMember("id", s.id, alloc);
        const std::string path = s.path.toStdString();
        rapidjson::Value path_val;
        path_val.SetString(path.c_str(), static_cast<rapidjson::SizeType>(path.size()), alloc);
        o.AddMember("path", path_val, alloc);
        scenes_arr.PushBack(o, alloc);
    }
    if (auto it = doc.FindMember("scenes"); it != doc.MemberEnd()) {
        it->value.Swap(scenes_arr);
    } else {
        rapidjson::Value k;
        k.SetString("scenes", alloc);
        doc.AddMember(k, scenes_arr, alloc);
    }

    engine_settings->save();
    engine_settings->load();
}

void SceneNodeTree::onSceneSettings()
{
    auto ctx = m_engine->context();
    if (!ctx || !ctx->sceneStore || !ctx->resourcePackageStore) {
        QMessageBox::warning(this, "Scene settings", "Engine context is not initialized.");
        return;
    }

    auto scene_opt = ctx->sceneStore->get(m_engine->getActiveSceneId());
    if (!scene_opt.has_value()) {
        QMessageBox::warning(this, "Scene settings", "Active scene is not loaded.");
        return;
    }

    auto scene = scene_opt.value();
    QString scene_name = QString::fromStdString(scene->name());

    const auto& available_packages_info = ctx->resourcePackageStore->getResourcePackagesInformation();

    const auto& current_resources_ref = scene->getResources();
    std::vector<uint32_t> selected_resources(current_resources_ref.begin(), current_resources_ref.end());

    QDialog dialog(this);
    dialog.setWindowTitle("Scene settings");
    dialog.setModal(true);
    dialog.resize(700, 500);

    auto* root_layout = new QVBoxLayout(&dialog);

    auto* scene_group = new QGroupBox("Scene", &dialog);
    auto* scene_form = new QFormLayout(scene_group);
    auto* name_edit = new QLineEdit(scene_name, scene_group);
    scene_form->addRow("Name", name_edit);
    root_layout->addWidget(scene_group);

    auto* resources_group = new QGroupBox("Resources (resource packages)", &dialog);
    auto* resources_layout = new QVBoxLayout(resources_group);

    auto* selected_list = new QListWidget(resources_group);
    selected_list->setSelectionMode(QAbstractItemView::SingleSelection);

    auto displayForId = [&](uint32_t id) -> QString {
        if (auto it = available_packages_info.find(id); it != available_packages_info.end()) {
            const auto& p = it->second;
            return QString("%1  (%2)").arg(QString::number(id), QString::fromStdString(p.filename().string()));
        }
        return QString::number(id);
    };

    auto repopulateSelected = [&]() {
        selected_list->clear();
        for (auto id : selected_resources) {
            auto* item = new QListWidgetItem(displayForId(id), selected_list);
            item->setData(Qt::UserRole, static_cast<qulonglong>(id));
            if (auto it = available_packages_info.find(id); it != available_packages_info.end()) {
                item->setToolTip(QString::fromStdString(it->second.string()));
            }
            selected_list->addItem(item);
        }
    };
    repopulateSelected();

    auto* controls_row = new QHBoxLayout();
    auto* available_combo = new QComboBox(resources_group);
    available_combo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    available_combo->setMinimumContentsLength(40);

    std::vector<std::pair<uint32_t, std::filesystem::path>> avail_sorted;
    avail_sorted.reserve(available_packages_info.size());
    for (const auto& [id, path] : available_packages_info) {
        avail_sorted.emplace_back(id, path);
    }
    std::ranges::sort(avail_sorted, [](const auto& a, const auto& b) { return a.second.filename() < b.second.filename(); });
    for (const auto& [id, path] : avail_sorted) {
        available_combo->addItem(QString("%1  (%2)").arg(QString::number(id), QString::fromStdString(path.filename().string())),
                                static_cast<qulonglong>(id));
    }

    auto* add_btn = new QPushButton("Add", resources_group);
    auto* remove_btn = new QPushButton("Remove selected", resources_group);

    controls_row->addWidget(new QLabel("Available:", resources_group));
    controls_row->addWidget(available_combo, 1);
    controls_row->addWidget(add_btn);
    controls_row->addWidget(remove_btn);

    resources_layout->addLayout(controls_row);
    resources_layout->addWidget(selected_list, 1);

    root_layout->addWidget(resources_group, 1);

    auto* button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    root_layout->addWidget(button_box);

    QObject::connect(name_edit, &QLineEdit::textChanged, [&scene_name](const QString& v) { scene_name = v; });

    QObject::connect(add_btn, &QPushButton::clicked, [&]() {
        if (available_combo->count() == 0) {
            return;
        }
        const auto id = static_cast<uint32_t>(available_combo->currentData().toULongLong());
        if (std::ranges::find(selected_resources, id) != selected_resources.end()) {
            return;
        }
        selected_resources.push_back(id);
        repopulateSelected();
    });

    QObject::connect(remove_btn, &QPushButton::clicked, [&]() {
        auto* item = selected_list->currentItem();
        if (!item) {
            return;
        }
        const auto id = static_cast<uint32_t>(item->data(Qt::UserRole).toULongLong());
        auto it = std::ranges::find(selected_resources, id);
        if (it != selected_resources.end()) {
            selected_resources.erase(it);
        }
        repopulateSelected();
    });

    QObject::connect(button_box, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(button_box, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    const auto old_name = scene->name();
    const auto old_resources = scene->getResources();

    scene->setName(scene_name.toStdString());
    scene->setResources(selected_resources);

    if (scene->name() != old_name) {
        m_scene_tree->setHeaderLabel(QString::fromStdString(scene->name()));
    }

    std::unordered_set<uint32_t> old_set(old_resources.begin(), old_resources.end());
    for (auto id : scene->getResources()) {
        if (old_set.contains(id)) {
            continue;
        }

        auto pkg = ctx->resourcePackageStore->get(id);
        if (!pkg.has_value()) {
            auto path = ctx->resourcePackageStore->getResourcePackageInformation(id);
            if (!path.has_value()) {
                continue;
            }
            auto built = engine::buildResourcePackage(path.value());
            if (!built.has_value()) {
                continue;
            }
            ctx->resourcePackageStore->add(id, built.value());
            pkg = built.value();
        }

        if (pkg.has_value()) {
            engine::loadResourcePackage(ctx, pkg.value());
        }
    }

    m_engine->saveScene(scene->id());
    
    build(scene);

    m_resource_package_editor->updateContent();
}

void SceneNodeTree::moveEvent(QMoveEvent* event)
{
    QMainWindow::moveEvent(event);

    positioningEngineWindow();
    positioningResourcePackageEditorWindow();
}

void SceneNodeTree::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);

    positioningEngineWindow();
    positioningResourcePackageEditorWindow();
}

void SceneNodeTree::build(std::optional<std::shared_ptr<engine::Scene>> scene)
{
    if (!scene.has_value()) {
        return;
    }

    auto& scene_value = scene.value();

    m_scene_tree->clear();

    m_scene_tree->setHeaderLabel(scene_value->name().c_str());

    auto root_node = scene_value->getRoot();

    if (!root_node.has_value()) {
        return;
    }

    createNodeWidget(root_node, nullptr);

    m_scene_tree->expandAll();
}

void SceneNodeTree::rebuildComponentWidget(QTreeWidgetItem* item)
{
    auto widget = m_scene_tree->itemWidget(item, 0);
    auto* component_widget = dynamic_cast<ComponentWidget*>(widget);
    if (component_widget != nullptr) {
        auto component = component_widget->component();
        createComponentWidget(item, component);
    }
}

void SceneNodeTree::initMenuBar()
{
    QMenu* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&Save Scene", this, &SceneNodeTree::onSaveScene);
    fileMenu->addAction("&Select Scene", this, &SceneNodeTree::onSelectScene);
    fileMenu->addAction("&Add Scene", this, &SceneNodeTree::onAddScene);
    fileMenu->addAction("&Engine settings", this, &SceneNodeTree::onEngineSettings);
    fileMenu->addAction("&Scene settings", this, &SceneNodeTree::onSceneSettings);
    fileMenu->addSeparator();
    fileMenu->addAction("&Exit", this, &SceneNodeTree::close);
}

void SceneNodeTree::initHeaderContextMenu()
{
    m_header_context_menu = new QMenu(m_scene_tree);

    m_add_root_node_action = m_header_context_menu->addAction("Add root node");
    m_add_root_node_action->setIcon(QIcon::fromTheme("list-add"));
    connect(m_add_root_node_action, &QAction::triggered, this, &SceneNodeTree::onAddRootNode);
}

void SceneNodeTree::initContextMenu()
{
    m_context_menu = new QMenu(m_scene_tree);

    m_add_child_node_action = m_context_menu->addAction("Add child node");
    m_add_child_node_action->setIcon(QIcon::fromTheme("list-add"));
    connect(m_add_child_node_action, &QAction::triggered, this, &SceneNodeTree::onAddChildNode);

    m_remove_node_action = m_context_menu->addAction("Remove node");
    m_remove_node_action->setIcon(QIcon::fromTheme("list-remove"));
    connect(m_remove_node_action, &QAction::triggered, this, &SceneNodeTree::onRemoveNode);

    auto ctx = m_engine->context();

    std::vector<std::string> component_types = ctx->userComponentsBuilder->componentTypes();

    component_types.insert(component_types.end(), engine::ComponentBuilder::componentTypes().begin(), engine::ComponentBuilder::componentTypes().end());

    m_add_component_menu = new QMenu("Add components", m_context_menu);

    for (const auto& component_type : component_types) {
        m_add_component_actions[component_type] = m_add_component_menu->addAction(QString::fromStdString(component_type));
        m_add_component_actions[component_type]->setIcon(QIcon::fromTheme("list-add"));
        connect(m_add_component_actions[component_type], &QAction::triggered, this, &SceneNodeTree::onAddComponent);
    }

    m_context_menu->addMenu(m_add_component_menu);

    m_remove_component_action = m_context_menu->addAction("Remove component");
    m_remove_component_action->setIcon(QIcon::fromTheme("list-remove"));
    connect(m_remove_component_action, &QAction::triggered, this, &SceneNodeTree::onRemoveComponent);

    m_copy_id = m_context_menu->addAction("Copy ID");
    m_copy_id->setIcon(QIcon::fromTheme("edit-copy"));
    connect(m_copy_id, &QAction::triggered, this, &SceneNodeTree::onCopyId);

    m_copy_name = m_context_menu->addAction("Copy name");
    m_copy_name->setIcon(QIcon::fromTheme("edit-copy"));
    connect(m_copy_name, &QAction::triggered, this, &SceneNodeTree::onCopyName);

    m_copy = m_context_menu->addAction("Copy");
    m_copy->setIcon(QIcon::fromTheme("edit-copy"));
    connect(m_copy, &QAction::triggered, this, &SceneNodeTree::onCopy);

    m_paste = m_context_menu->addAction("Paste");
    m_paste->setIcon(QIcon::fromTheme("edit-paste"));
    connect(m_paste, &QAction::triggered, this, &SceneNodeTree::onPaste);

    m_rename = m_context_menu->addAction("Rename");
    m_rename->setIcon(QIcon::fromTheme("edit-rename"));
    connect(m_rename, &QAction::triggered, this, &SceneNodeTree::onRename);

    m_duplicate = m_context_menu->addAction("Duplicate");
    m_duplicate->setIcon(QIcon::fromTheme("edit-duplicate"));
    connect(m_duplicate, &QAction::triggered, this, &SceneNodeTree::onDuplicate);

    m_enable = m_context_menu->addAction("Enable");
    m_enable->setIcon(QIcon::fromTheme("object-visibility"));
    connect(m_enable, &QAction::triggered, this, &SceneNodeTree::onEnable);

    m_disable = m_context_menu->addAction("Disable");
    m_disable->setIcon(QIcon::fromTheme("object-visibility"));
    connect(m_disable, &QAction::triggered, this, &SceneNodeTree::onDisable);
}

void SceneNodeTree::createComponentWidget(QTreeWidgetItem* item, const std::shared_ptr<engine::Component>& component)
{
    auto component_widget = m_scene_tree_widget_builder->buildWidgetForComponent(component, item);
    if (!component_widget.has_value()) {
        component_widget = m_user_tree_widget_builder->buildWidgetForComponent(component);
    }
    if (component_widget.has_value()) {
        component_widget.value()->setComponent(component);
        m_scene_tree->setItemWidget(item, 0, component_widget.value());
    }
}

void SceneNodeTree::createNodeWidget(std::optional<std::shared_ptr<engine::Node>> node, QTreeWidgetItem* parent)
{
    if (!node.has_value()) {
        return;
    }

    auto& node_value = node.value();

    QTreeWidgetItem* nodeItem = nullptr;

    if (parent == nullptr) {
        nodeItem = new QTreeWidgetItem(m_scene_tree);
    } else {
        nodeItem = new QTreeWidgetItem(parent);
    }

    auto node_widget = m_scene_tree_widget_builder->buildWidgetForNode(node_value);
    if (!node_widget.has_value()) {
        return;
    }

    node_widget.value()->setNode(node_value);
    m_scene_tree->setItemWidget(nodeItem, 0, node_widget.value());

    auto components = node_value->components();

    for (const auto component_id : components) {
        auto component = node_value->getComponent(component_id);

        if (!component.has_value()) {
            return;
        }

        auto& component_value = component.value();

        auto* componentItem = new QTreeWidgetItem(nodeItem);
        createComponentWidget(componentItem, component_value);
    }

    auto node_children = node_value->children();
    for (const auto child_id : node_children) {
        createNodeWidget(node_value->getChild(child_id), nodeItem);
    }
}

void SceneNodeTree::changeActiveWidget(bool active)
{
    auto widget = m_scene_tree->itemWidget(m_selected_item, 0);
    auto* component_widget = dynamic_cast<ComponentWidget*>(widget);
    if (component_widget != nullptr) {
        auto component = component_widget->component();
        component->setActive(active);
    }

    auto* node_widget = dynamic_cast<NodeWidget*>(widget);
    if (node_widget != nullptr) {
        auto node = node_widget->node();
        node->setActive(active);
    }

    m_selected_item = nullptr;
}

void SceneNodeTree::positioningEngineWindow()
{
    auto ctx = m_engine->context();
    qreal dpr = windowHandle()->devicePixelRatio();
    QRect frame = frameGeometry();
    int x = (frame.x() + frame.width() + 10) * dpr;
    int y = frame.y() * dpr;
    ctx->window->setWindowPosition(x, y);
}

void SceneNodeTree::positioningResourcePackageEditorWindow()
{
    qreal dpr = windowHandle()->devicePixelRatio();
    QRect frame = frameGeometry();
    int x = frame.x() * dpr;
    int y = (frame.y() + frame.height() + 10) * dpr;

    auto frame_height = m_resource_package_editor->height();

    m_resource_package_editor->resize(frame.width() * dpr, frame_height);
    m_resource_package_editor->move(x, y);
}

}
