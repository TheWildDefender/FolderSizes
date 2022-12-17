#include "entrysizetableitem.h"

#include "mainwindow.h"

EntrySizeTableItem::EntrySizeTableItem(const QString& entryName) :
    entryName(entryName)
{
    setText(MainWindow::CALCULATING_SIZE_STR);
}

void EntrySizeTableItem::setEntrySize(const long long entrySize)
{
    this->entrySize = entrySize;
    setText(suffixedFileSize(entrySize));
}

bool EntrySizeTableItem::operator<(const QTableWidgetItem& other) const
{
    const EntrySizeTableItem& o = dynamic_cast<const EntrySizeTableItem&>(other);

    if (this->entrySize == -1 && o.entrySize == -1)
        return this->entryName < o.entryName;
    else if (this->entrySize == -1)
        return true;
    else if (o.entrySize == -1)
        return false;
    else
        return this->entrySize < o.entrySize;
}

QString EntrySizeTableItem::suffixedFileSize(long long size)
{
    float suffixedSize = size;
    int suffixIndex = 0;
    QStringList suffixes {"B", "kB", "MB", "GB"};

    while (suffixIndex < suffixes.size() - 1 && suffixedSize >= 1024) {
        suffixedSize /= 1024;
        ++suffixIndex;
    }

    return QString("%1 %2").arg(suffixedSize, 0, 'f', 2).arg(suffixes[suffixIndex]);
}
