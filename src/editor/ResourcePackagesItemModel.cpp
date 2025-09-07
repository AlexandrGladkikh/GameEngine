#include "ResourcePackagesItemModel.h"

#include <QMimeData>

namespace editor {

ResourcePackagesItemModel::ResourcePackagesItemModel(QObject* parent) :
    QStandardItemModel(parent)
{
}

QStringList ResourcePackagesItemModel::mimeTypes() const
{
    QStringList types = QStandardItemModel::mimeTypes();
    types << "text/plain";
    return types;
}

QMimeData* ResourcePackagesItemModel::mimeData(const QModelIndexList& indexes) const
{
    QMimeData* mime_data = QStandardItemModel::mimeData(indexes);

    QStringList texts;
    for (const QModelIndex& index: indexes) {
        if (index.isValid()) {
            texts << data(index, Qt::DisplayRole).toString();
        }
    }

    if (!texts.isEmpty()) {
        mime_data->setText(texts.join("\n"));
    }

    return mime_data;
}

}
