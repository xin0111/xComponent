#ifndef ABSTRACTTASK_H
#define ABSTRACTTASK_H

#include "postjobaction.h"

#include <QProcess>
#include <QModelIndex>
#include <QList>
#include <QTime>

class QAction;
class QStandardItem;

class AbstractTask : public QProcess
{
    Q_OBJECT
public:
    explicit AbstractTask(const QString& name);
    virtual ~AbstractTask() {}

    void setStandardItem(QStandardItem* item);
    QStandardItem* standardItem();
    bool ran() const;
    bool stopped() const;
    void appendToLog(const QString&);
    QString log() const;
    QString label() const { return m_label; }
    void setLabel(const QString& label);
    QList<QAction*> standardActions() const { return m_standardActions; }
    QList<QAction*> successActions() const { return m_successActions; }
    QTime estimateRemaining(int percent);
    QTime time() const { return m_totalTime; }
    void setPostJobAction(PostJobAction* action);

public slots:
    virtual void start();
    virtual void stop();

signals:
    void progressUpdated(QStandardItem* item, int percent);
    void finished(AbstractTask* job, bool isSuccess, QString failureTime = QString());

protected:
    QList<QAction*> m_standardActions;
    QList<QAction*> m_successActions;
    QStandardItem*  m_item;

protected slots:
    virtual void onFinished(int exitCode, QProcess::ExitStatus exitStatus = QProcess::NormalExit);
    virtual void onReadyRead();
    virtual void onStarted();

private slots:
    void onProgressUpdated(QStandardItem*, int percent);

private:
    bool m_ran;
    bool m_killed;
    QString m_log;
    QString m_label;
    QTime m_estimateTime;
    int m_startingPercent;
    QTime m_totalTime;
    QScopedPointer<PostJobAction> m_postJobAction;
};

#endif // ABSTRACTJOB_H
