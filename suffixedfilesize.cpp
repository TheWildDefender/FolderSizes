#include "suffixedfilesize.h"

#include <QStringList>

QString suffixedFileSize(long long size)
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
