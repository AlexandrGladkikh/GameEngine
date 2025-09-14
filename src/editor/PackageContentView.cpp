#include "PackageContentView.h"

#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QMimeData>
#include <QFileInfo>

namespace editor {

PackageContentView::PackageContentView(QWidget* parent) :
    QListView(parent)
{
    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);
    setDragEnabled(true);
    setDefaultDropAction(Qt::CopyAction);
    setDragDropMode(QAbstractItemView::DragDrop);
}

void PackageContentView::dragEnterEvent(QDragEnterEvent* event)
{
    event->acceptProposedAction();
    auto mime_data = event->mimeData();
    if (mime_data->hasUrls()) {
        bool has_files = false;
        const auto urls = mime_data->urls();
        for (const QUrl& url: urls) {
            if (url.isLocalFile()) {
                has_files = true;
                break;
            }
        }

        if (has_files) {
            event->acceptProposedAction();

            setStyleSheet(styleSheet() + R"(
                QListView {
                    border: 3px dashed #0078d4;
                    background-color: rgba(0, 120, 212, 10)
                }
            )");
        }
    }
}

void PackageContentView::dragMoveEvent(QDragMoveEvent* event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void PackageContentView::dropEvent(QDropEvent* event)
{
    setStyleSheet("");

    auto mime_data = event->mimeData();

    if (mime_data->hasUrls()) {
        QStringList file_paths;

        const auto urls = mime_data->urls();
        for (const QUrl& url: urls) {
            if (url.isLocalFile()) {
                QString path = url.toLocalFile();
                QFileInfo file_info(path);

                if (file_info.exists() && file_info.isFile()) {
                    file_paths.append(path);
                }
            }
        }

        if (!file_paths.isEmpty()) {
            emit filesDropped(file_paths);
            event->acceptProposedAction();
        }
    }

    QListView::dropEvent(event);
}

void PackageContentView::dragLeaveEvent(QDragLeaveEvent* event)
{
    Q_UNUSED(event);

    setStyleSheet("");
}

}
