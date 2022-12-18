#ifndef ENTRYSIZETABLEITEM_H
#define ENTRYSIZETABLEITEM_H

#include <QTableWidgetItem>

class EntrySizeTableItem : public QTableWidgetItem
{
public:
    EntrySizeTableItem(const QString& entryName);

    void setEntrySize(const long long entrySize);

    bool operator<(const QTableWidgetItem& other) const override;

private:
    long long entrySize = -1; // -1 means size for the corresponding entry is being calculated
    const QString entryName;
};

#endif // ENTRYSIZETABLEITEM_H
