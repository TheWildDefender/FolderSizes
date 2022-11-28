#ifndef DIRSIZECALCULATOR_H
#define DIRSIZECALCULATOR_H

#include <QObject>
#include <QQueue>
#include <QThread>
#include <QMutex>

class DirSizeTask : public QObject
{
    Q_OBJECT
public:
    QString dirPath;
    long size = 0;
    QQueue<QString> subdirsToSearch;
    QMutex taskMutex;

    DirSizeTask(QObject* parent, const QString& dirPath);

    // debug
//    ~DirSizeTask();
};

class DirSizeTaskWorker;

class DirSizeCalculator : public QObject
{
    Q_OBJECT
public:
    explicit DirSizeCalculator(QObject *parent = nullptr);

    void startTask(const QString& path);
    void stopWorkers();

signals:
    void dirSizeCalculated(const QString& path, const long size);

private slots:
    void dirSizeTaskFinished(const QString& dirPath, const long size);

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
    void stop();

signals:
    void dirSizeTaskFinished(const QString& dirPath, const long size);

protected:
    void run() override;

private:
    bool active = true;
    QQueue<DirSizeTask*>& tasks;
    QMutex& tasksMutex;
};

#endif // DIRSIZECALCULATOR_H
