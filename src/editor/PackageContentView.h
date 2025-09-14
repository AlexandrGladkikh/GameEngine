#pragma once

#include <QListView>

namespace editor {

class PackageContentView : public QListView {
    Q_OBJECT

public:
    explicit PackageContentView(QWidget* parent = nullptr);

signals:
    void filesDropped(const QStringList& files);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
};

}
