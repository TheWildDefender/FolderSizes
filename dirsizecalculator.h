#ifndef DIRSIZECALCULATOR_H
#define DIRSIZECALCULATOR_H

#include <QObject>
#include <QQueue>
#include <QThread>
#include <QMutex>
#include <QAtomicInteger>

class DirSizeTask : public QObject
{
    Q_OBJECT
public:
    QString dirPath;
    QAtomicInteger<long long> size = 0;
    QQueue<QString> subdirsToSearch;
    int numActiveWorkers = 0;
    QMutex taskMutex;

    DirSizeTask(QObject* parent, const QString& dirPath);
};

class DirSizeTaskWorker;

class DirSizeCalculator : public QObject
{
    Q_OBJECT
public:
    explicit DirSizeCalculator(QObject *parent = nullptr);

    void startTask(const QString& path);
    void stopWorkers();
    void cancelTasks();
    long getNumTasks();

signals:
    void dirSizeCalculated(const QString& path, const long long size);

private slots:
    void dirSizeTaskFinished(const QString& dirPath, const long long size);

private:
    QQueue<DirSizeTask*> tasks;
    QMutex tasksMutex;
    QList<DirSizeTaskWorker*> workers;
};

class DirSizeTaskWorker : public QThread
{
    Q_OBJECT
public:
    DirSizeTaskWorker(QObject* parent, QQueue<DirSizeTask*>& tasks, QMutex& tasksMutex);
    void setActive(bool active);

signals:
    void dirSizeTaskFinished(const QString& dirPath, const long long size);

protected:
    void run() override;

private:
    bool active = true;
    QQueue<DirSizeTask*>& tasks;
    QMutex& tasksMutex;
};

#endif // DIRSIZECALCULATOR_H
