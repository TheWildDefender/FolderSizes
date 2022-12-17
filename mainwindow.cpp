#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QCloseEvent>

#include "entrynametableitem.h"

const QString MainWindow::CALCULATING_SIZE_STR = "Calculating";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    dirSizeCalculator(this),
    getNumTasksTimer(this)
{
    ui->setupUi(this);

    // UI initialization
    ui->tableWidget->setColumnWidth(0, FILESIZE_COLUMN_WIDTH);
    ui->tableWidget->verticalHeader()->hide();
    ui->pathEdit->setText(this->curDirPath);

    // Signal-slot connections

    QObject::connect(ui->browseButton, &QAbstractButton::clicked, this, &MainWindow::onBrowseButtonClick);
    QObject::connect(ui->pathEdit, &QLineEdit::returnPressed, this, &MainWindow::onPathEditReturn);
    QObject::connect(ui->upButton, &QAbstractButton::clicked, this, &MainWindow::onUpButtonClicked);

    QHeaderView* colHeaders = ui->tableWidget->horizontalHeader();
    QObject::connect(colHeaders, &QHeaderView::sectionClicked, this, &MainWindow::onColHeaderClicked);

    QObject::connect(this, &MainWindow::curDirChanged, this, &MainWindow::onCurDirChanged);

    QObject::connect(&dirSizeCalculator, &DirSizeCalculator::dirSizeCalculated, this, &MainWindow::displayDirSize);

    QObject::connect(&getNumTasksTimer, &QTimer::timeout, this, &MainWindow::displayNumTasks);

    // Display subdirs and files and start calculating their sizes on start
    emit curDirChanged(curDirPath);

    // Start timer
    getNumTasksTimer.setInterval(GET_NUM_TASKS_TIMER_INTERVAL);
    getNumTasksTimer.start();
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
    QStringList subdirNames = curDir.entryList(
                QDir::Dirs | QDir::Hidden | QDir::System | QDir::NoSymLinks | QDir::NoDotAndDotDot
    );
    QStringList fileNames = curDir.entryList(
                QDir::Files | QDir::Hidden | QDir::System | QDir::NoSymLinks
    );

    // Stop any previous dir size calculations
    dirSizeCalculator.cancelTasks();

    // Clear table widget and entry size items list
    // Call removeRow repeatedly instead of using clear to preserve column headers
    for (int i = ui->tableWidget->rowCount() - 1; i >= 0; --i)
        ui->tableWidget->removeRow(i);
    sizeItems.clear();

    // Add a row for each subdir and file
    for (const QString& subdirName : subdirNames)
        addNewEntryRow(subdirName, true);
    for (const QString& fileName : fileNames)
        addNewEntryRow(fileName, false);

    // Start calculating subdir and file sizes
    for (const QString& subdirName : subdirNames) {
        QString subdirPath = curDir.absoluteFilePath(subdirName);
        dirSizeCalculator.startTask(subdirPath);
    }
    for (const QString& fileName : fileNames) {
        QString filePath = curDir.absoluteFilePath(fileName);
        QFileInfo fileInfo(filePath);
        sizeItems[fileName]->setEntrySize(fileInfo.size());
    }
}

void MainWindow::displayDirSize(const QString& dirPath, const long long size)
{
    QFileInfo dirInfo(dirPath);
    sizeItems[dirInfo.fileName()]->setEntrySize(size);
}

void MainWindow::displayNumTasks()
{
    long numTasks = dirSizeCalculator.getNumTasks();
    ui->statusBar->showMessage("Tasks: " + QString::number(numTasks));
}

void MainWindow::onColHeaderClicked(int sectionIndex)
{
    ui->tableWidget->sortItems(sectionIndex);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    dirSizeCalculator.stopWorkers();
}

void MainWindow::addNewEntryRow(const QString& entryName, bool isDir)
{
    int rowIdx = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(rowIdx);
    ui->tableWidget->setRowHeight(rowIdx, ROW_HEIGHT);

    EntryNameTableItem* entryNameItem = new EntryNameTableItem(entryName, isDir);
    EntrySizeTableItem* entrySizeItem = new EntrySizeTableItem(entryName);
    entryNameItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    entrySizeItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui->tableWidget->setItem(rowIdx, 0, entryNameItem);
    ui->tableWidget->setItem(rowIdx, 1, entrySizeItem);

    sizeItems[entryName] = entrySizeItem;
}
