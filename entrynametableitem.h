#ifndef ENTRYNAMETABLEITEM_H
#define ENTRYNAMETABLEITEM_H

#include <QTableWidgetItem>

class EntryNameTableItem : public QTableWidgetItem
{
public:
    EntryNameTableItem(const QString& entryName, const bool isDir);

    bool operator<(const QTableWidgetItem& other) const override;

private:
    const QString entryName;
    const bool isDir;
};

#endif // ENTRYNAMETABLEITEM_H
