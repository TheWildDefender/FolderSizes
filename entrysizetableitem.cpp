#include "entrysizetableitem.h"

#include "mainwindow.h"
#include "suffixedfilesize.h"

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
