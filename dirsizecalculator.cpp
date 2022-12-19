#include "dirsizecalculator.h"

#include <QDebug>
#include <QDir>

DirSizeTask::DirSizeTask(QObject* parent, const QString& dirPath) :
    QObject(parent),
    dirPath(dirPath)
{
    subdirsToSearch.enqueue(dirPath);
}

DirSizeCalculator::DirSizeCalculator(QObject *parent) : QObject{parent}
{
    for (int i = 0; i < QThread::idealThreadCount(); ++i) {
        DirSizeTaskWorker* w = new DirSizeTaskWorker(this, tasks, tasksMutex);
        workers.push_back(w);
        // Use a blocking queued connection to ensure that when the main thread is finished with waiting for the
        // calculator to cancel all tasks, the calculator will not emit any more dirSizeCalculated signals. The
        // connection should be queued, because reporting a dir size requires updating UI elements.
        QObject::connect(w, &DirSizeTaskWorker::dirSizeTaskFinished,
                         this, &DirSizeCalculator::dirSizeTaskFinished,
                         Qt::BlockingQueuedConnection);
        w->start();
    }
}

void DirSizeCalculator::startTask(const QString& path)
{
    tasksMutex.lock();
    tasks.enqueue(new DirSizeTask(this, path));
    tasksMutex.unlock();
}

void DirSizeCalculator::stopWorkers()
{
    for (DirSizeTaskWorker* w : workers)
        w->setActive(false);
    for (DirSizeTaskWorker* w : workers)
        w->wait();
}

void DirSizeCalculator::cancelTasks()
{
    for (DirSizeTaskWorker* w : workers)
        w->setActive(false);
    for (DirSizeTaskWorker* w : workers)
        w->wait();

    tasks.clear();

    for (DirSizeTaskWorker* w : workers) {
        w->setActive(true);
        w->start();
    }
}

long DirSizeCalculator::getNumTasks()
{
    tasksMutex.lock();
    long numTasks = tasks.size();
    tasksMutex.unlock();
    return numTasks;
}

void DirSizeCalculator::dirSizeTaskFinished(const QString& dirPath, const long long size)
{
    emit dirSizeCalculated(dirPath, size);
}

DirSizeTaskWorker::DirSizeTaskWorker(QObject* parent,
        QQueue<DirSizeTask*>& tasks, QMutex& tasksMutex) :
    QThread{parent},
    tasks(tasks), tasksMutex(tasksMutex)
{}

void DirSizeTaskWorker::setActive(bool active)
{
    this->active = active;
}

void DirSizeTaskWorker::run()
{
    while (active) {
        // Wait for a task if no tasks
        tasksMutex.lock();
        if (tasks.isEmpty()) {
            tasksMutex.unlock();
            QThread::msleep(1);
            continue;
        }

        // Get a task
        DirSizeTask* task = tasks.dequeue();
        tasksMutex.unlock();

        // Get subdir to work on and increment worker count
        task->taskMutex.lock();
        QString curDirPath = task->subdirsToSearch.dequeue();
        ++task->numActiveWorkers;
        task->taskMutex.unlock();

        QDir curDir(curDirPath);
        QStringList fileNames = curDir.entryList(
                    QDir::Files | QDir::Hidden | QDir::System | QDir::NoSymLinks
        );
        QStringList subdirNames = curDir.entryList(
                    QDir::Dirs | QDir::Hidden | QDir::System | QDir::NoSymLinks | QDir::NoDotAndDotDot
        );
        for (const QString& fileName : fileNames) {
            QString filePath = curDir.absoluteFilePath(fileName);
            QFileInfo fileInfo(filePath);
            task->size.fetchAndAddRelaxed(fileInfo.size());
        }
        for (const QString& subdirName : subdirNames) {
            QString subdirPath = curDir.absoluteFilePath(subdirName);

            task->taskMutex.lock();
            task->subdirsToSearch.enqueue(subdirPath);
            task->taskMutex.unlock();

            tasksMutex.lock();
            tasks.enqueue(task);
            tasksMutex.unlock();
        }

        // Decrement worker count and check if task is finished
        task->taskMutex.lock();
        --task->numActiveWorkers;
        if (task->subdirsToSearch.isEmpty() && task->numActiveWorkers == 0) {
            task->taskMutex.unlock();
            emit dirSizeTaskFinished(task->dirPath, task->size);
            task->deleteLater();
            continue;
        }
        task->taskMutex.unlock();
    }
}
