#include "dirsizecalculator.h"

#include <QDebug>

DirSizeTask::DirSizeTask(QObject* parent, const QString& dirPath) :
    QObject(parent),
    dirPath(dirPath)
{}

//DirSizeTask::~DirSizeTask()
//{
//    qInfo() << "task destructed";
//}

DirSizeCalculator::DirSizeCalculator(QObject *parent) : QObject{parent}
{
    for (int i = 0; i < QThread::idealThreadCount(); ++i) {
        DirSizeTaskWorker* w = new DirSizeTaskWorker(this, tasks, tasksMutex);
        workers.push_back(w);
        QObject::connect(w, &DirSizeTaskWorker::dirSizeTaskFinished,
                         this, &DirSizeCalculator::dirSizeTaskFinished);
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
        w->stop();
    for (DirSizeTaskWorker* w : workers)
        w->wait();
}

void DirSizeCalculator::dirSizeTaskFinished(const QString& dirPath, const long size)
{
    emit dirSizeCalculated(dirPath, size);
}

DirSizeTaskWorker::DirSizeTaskWorker(QObject* parent,
        QQueue<DirSizeTask*>& tasks, QMutex& tasksMutex) :
    QThread{parent},
    tasks(tasks), tasksMutex(tasksMutex)
{}

void DirSizeTaskWorker::stop()
{
    active = false;
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

        // debug
        qInfo() << task->dirPath;

        // Check if task is finished
        task->taskMutex.lock();
        if (task->subdirsToSearch.isEmpty()) {
            task->taskMutex.unlock();
            emit dirSizeTaskFinished(task->dirPath, task->size);
            task->deleteLater();
            continue;
        }
        task->taskMutex.unlock();
    }
}
