#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QMap>
#include <QTimer>
#include <QTableWidgetItem>

#include "dirsizecalculator.h"
#include "entrysizetableitem.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    constexpr static int FILESIZE_COLUMN_WIDTH = 300;
    constexpr static int ROW_HEIGHT = 15;
    constexpr static int GET_NUM_TASKS_TIMER_INTERVAL = 500;
    const static QString CALCULATING_SIZE_STR;

signals:
    void curDirChanged(const QString& path);

private slots:
    void onBrowseButtonClick(bool checked);
    void onPathSelected(const QString& path);
    void onPathEditReturn();
    void onUpButtonClicked(bool checked);
    void onCurDirChanged(const QString& curDirPath);
    void displayDirSize(const QString& dirPath, const long long size);
    void displayNumTasks();
    void onColHeaderClicked(int sectionIndex);

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    Ui::MainWindow *ui;

    QString curDirPath = QDir::homePath();
    // keyed by entry name, not full paths
    QMap<QString, EntrySizeTableItem*> sizeItems;
    DirSizeCalculator dirSizeCalculator;
    QTimer getNumTasksTimer;

    void addNewEntryRow(const QString& entryName, bool isDir);
};
#endif // MAINWINDOW_H
