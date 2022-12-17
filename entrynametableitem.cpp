#include "entrynametableitem.h"

#include <QIcon>

EntryNameTableItem::EntryNameTableItem(const QString& entryName, const bool isDir) :
    QTableWidgetItem(entryName),
    entryName(entryName),
    isDir(isDir)
{
    if (isDir)
        setIcon(QIcon(":icons/folder"));
    else
        setIcon(QIcon(":icons/file"));
}

bool EntryNameTableItem::operator<(const QTableWidgetItem& other) const
{
    const EntryNameTableItem& o = dynamic_cast<const EntryNameTableItem&>(other);

    if ( (this->isDir && o.isDir) || (!this->isDir && !o.isDir) )
        return this->entryName < o.entryName;
    else if (this->isDir)
        return true;
    else // other is dir
        return false;
}
