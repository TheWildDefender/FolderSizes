#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    dirSizeCalculator(this)
{
    ui->setupUi(this);

    // UI initialization
    ui->tableWidget->setColumnWidth(0, 300);
    ui->pathEdit->setText(this->curDirPath);

    // Signal-slot connections
    QObject::connect(ui->browseButton, &QAbstractButton::clicked, this, &MainWindow::onBrowseButtonClick);
    QObject::connect(ui->pathEdit, &QLineEdit::returnPressed, this, &MainWindow::onPathEditReturn);
    QObject::connect(ui->upButton, &QAbstractButton::clicked, this, &MainWindow::onUpButtonClicked);
    QObject::connect(this, &MainWindow::curDirChanged, this, &MainWindow::onCurDirChanged);

    emit curDirChanged(curDirPath);
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*!
 * \brief Slot for the browse button being clicked. Open a folder selection dialog to choose a new directory.
 * \param checked
 */
void MainWindow::onBrowseButtonClick(bool checked)
{
    QFileDialog fileDialog(this);
    fileDialog.setFileMode(QFileDialog::Directory);
    fileDialog.setDirectory(curDirPath);
    QObject::connect(&fileDialog, &QFileDialog::fileSelected, this, &MainWindow::onPathSelected);
    fileDialog.exec();
}

void MainWindow::onPathSelected(const QString& path)
{
    curDirPath = path;
    ui->pathEdit->setText(curDirPath);
    emit curDirChanged(curDirPath);
}

/*!
 * \brief Slot for the return key being pressed in the path edit. Check the path and update the current dir path.
 */
void MainWindow::onPathEditReturn()
{
    const QString& path = ui->pathEdit->text();

    if (!QFileInfo::exists(path))
        ui->pathEdit->setText(curDirPath);

    curDirPath = path;
    emit curDirChanged(curDirPath);
}

void MainWindow::onUpButtonClicked(bool checked)
{
    QDir curDir(curDirPath);
    if (!curDir.cdUp())
        return;

    this->curDirPath = curDir.canonicalPath();
    ui->pathEdit->setText(curDirPath);
    emit curDirChanged(curDirPath);
}

void MainWindow::onCurDirChanged(const QString& curDirPath)
{
    QDir curDir(curDirPath);
    for (const QString& entryPath : curDir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot))
        dirSizeCalculator.startTask(entryPath);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    dirSizeCalculator.stopWorkers();
}
