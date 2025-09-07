#pragma once

#include <QStandardItemModel>

namespace editor {

class ResourcePackagesItemModel : public QStandardItemModel {
public:
    explicit ResourcePackagesItemModel(QObject* parent = nullptr);

    QStringList mimeTypes() const override;

    QMimeData* mimeData(const QModelIndexList& indexes) const override;
};

}
