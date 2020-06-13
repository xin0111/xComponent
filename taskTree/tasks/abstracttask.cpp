#include "abstracttask.h"
#include "postjobaction.h"
#include <QApplication>
#include <QTimer>
#ifdef Q_OS_WIN
#include <windows.h>
#endif

AbstractTask::AbstractTask(const QString& name)
    : QProcess(0)
    , m_item(0)
    , m_ran(false)
    , m_killed(false)
    , m_label(name)
    , m_startingPercent(0)
{
    setObjectName(name);
    connect(this, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onFinished(int, QProcess::ExitStatus)));
    connect(this, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(this, SIGNAL(started()), this, SLOT(onStarted()));
    connect(this, SIGNAL(progressUpdated(QStandardItem*, int)), SLOT(onProgressUpdated(QStandardItem*, int)));
}

void AbstractTask::start()
{
    m_killed = false;
    m_ran = true;
    m_estimateTime.start();
    m_totalTime.start();
    emit progressUpdated(m_item, 0);
}

void AbstractTask::setStandardItem(QStandardItem* item)
{
    m_item = item;
}

QStandardItem* AbstractTask::standardItem()
{
    return m_item;
}

bool AbstractTask::ran() const
{
    return m_ran;
}

bool AbstractTask::stopped() const
{
    return m_killed;
}

void AbstractTask::appendToLog(const QString& s)
{
    m_log.append(s);
}

QString AbstractTask::log() const
{
    return m_log;
}

void AbstractTask::setLabel(const QString &label)
{
    m_label = label;
}

QTime AbstractTask::estimateRemaining(int percent)
{
    QTime result;
    if (percent) {
        int averageMs = m_estimateTime.elapsed() / (percent - m_startingPercent);
        result = QTime::fromMSecsSinceStartOfDay(averageMs * (100 - percent));
    }
    return result;
}

void AbstractTask::setPostJobAction(PostJobAction* action)
{
    m_postJobAction.reset(action);
}

void AbstractTask::stop()
{
    closeWriteChannel();
    terminate();
    QTimer::singleShot(2000, this, SLOT(kill()));
    m_killed = true;
}

void AbstractTask::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    const QTime& time = QTime::fromMSecsSinceStartOfDay(m_totalTime.elapsed());
    if (isOpen()) {
        m_log.append(readAll());
    }
    if (exitStatus == QProcess::NormalExit && exitCode == 0 && !m_killed) {
        if (m_postJobAction) {
            m_postJobAction->doAction();
        }
        m_log.append(QString("Completed successfully in %1\n").arg(time.toString()));
        emit progressUpdated(m_item, 100);
        emit finished(this, true);
    } else if (m_killed) {
        m_log.append(QString("Stopped by user at %1\n").arg(time.toString()));
        emit finished(this, false);
    } else {
        m_log.append(QString("Failed with exit code %1\n").arg(exitCode));
        emit finished(this, false);
    }
}

void AbstractTask::onReadyRead()
{
    QString msg;
    do {
        msg = readLine();
        appendToLog(msg);
    } while (!msg.isEmpty());
}

void AbstractTask::onStarted()
{
#ifdef Q_OS_WIN
    qint64 processId = QProcess::processId();
    HANDLE processHandle = OpenProcess(PROCESS_SET_INFORMATION, FALSE, processId);
    if(processHandle)
    {
        SetPriorityClass(processHandle, BELOW_NORMAL_PRIORITY_CLASS);
        CloseHandle(processHandle);
    }
#endif
}

void AbstractTask::onProgressUpdated(QStandardItem*, int percent)
{
    // Start timer on first reported percentage > 0.
    if (percent == 1) {
        m_estimateTime.restart();
        m_startingPercent = percent;
    }
}
