#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include "abstracttask.h"
#include <QStandardItemModel>
#include <QMutex>

class TaskQueue : public QStandardItemModel
{
    Q_OBJECT
protected:
    TaskQueue(QObject *parent);
    void startNextJob();

public:
    enum ColumnRole {
        COLUMN_ICON,
        COLUMN_OUTPUT,
        COLUMN_STATUS,
        COLUMN_COUNT
    };

    static TaskQueue& singleton(QObject* parent = 0);
    void cleanup();
    AbstractTask* add(AbstractTask *job);
    AbstractTask* jobFromIndex(const QModelIndex& index) const;
    void pause();
    void resume();
    bool isPaused() const;
    bool hasIncomplete() const;
    void remove(const QModelIndex& index);
    void removeFinished();
    QList<AbstractTask*> jobs() const { return m_jobs; }

signals:
    void jobAdded();

public slots:
    void onProgressUpdated(QStandardItem* standardItem, int percent);
    void onFinished(AbstractTask* job, bool isSuccess, QString time);

private:
    QList<AbstractTask*> m_jobs;
    QMutex m_mutex; // protects m_jobs
    bool m_paused;
};

#define JOBS TaskQueue::singleton()

#endif // JOBQUEUE_H
