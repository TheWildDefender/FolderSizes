#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QMap>

#include "dirsizecalculator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void curDirChanged(const QString& path);

private slots:
    void onBrowseButtonClick(bool checked);
    void onPathSelected(const QString& path);
    void onPathEditReturn();
    void onUpButtonClicked(bool checked);
    void onCurDirChanged(const QString& curDirPath);

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    Ui::MainWindow *ui;

    QString curDirPath = QDir::homePath();
    QMap<QString, int> fileSizes;
    DirSizeCalculator dirSizeCalculator;
};
#endif // MAINWINDOW_H
