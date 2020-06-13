#include "TaskQueue.h"
#include <QtWidgets>

TaskQueue::TaskQueue(QObject *parent) :
    QStandardItemModel(0, COLUMN_COUNT, parent),
    m_paused(false)
{
}

TaskQueue& TaskQueue::singleton(QObject* parent)
{
    static TaskQueue* instance = 0;
    if (!instance)
        instance = new TaskQueue(parent);
    return *instance;
}

void TaskQueue::cleanup()
{
    QMutexLocker locker(&m_mutex);
    foreach (AbstractTask* job, m_jobs) {
        if (job->state() == QProcess::Running) {
            job->stop();
            break;
        }
    }
    qDeleteAll(m_jobs);
}

AbstractTask* TaskQueue::add(AbstractTask* job)
{
    QList<QStandardItem*> items;
	QStyle* style = QApplication::style();
	QIcon icon = style->standardIcon(QStyle::SP_TitleBarMenuButton);
    items << new QStandardItem(icon, "");
    QStandardItem* item = new QStandardItem(job->label());
    items << item;
    item = new QStandardItem(tr("pending"));
#ifdef Q_OS_MAC
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    font.setPointSize(QGuiApplication::font().pointSize());
    item->setFont(font);
#endif
    item->setToolTip(tr("Estimated Hours:Minutes:Seconds"));
    items << item;
    appendRow(items);
    job->setParent(this);
    job->setStandardItem(item);
    connect(job, SIGNAL(progressUpdated(QStandardItem*, int)), SLOT(onProgressUpdated(QStandardItem*, int)));
    connect(job, SIGNAL(finished(AbstractTask*, bool, QString)), SLOT(onFinished(AbstractTask*, bool, QString)));
    m_mutex.lock();
    m_jobs.append(job);
    m_mutex.unlock();
    emit jobAdded();
    startNextJob();

    return job;
}

void TaskQueue::onProgressUpdated(QStandardItem* standardItem, int percent)
{
    if (standardItem) {
        AbstractTask* job = m_jobs.at(standardItem->row());
        if (job) {
            QString remaining = "--:--:--";
            if (percent > 2)
                remaining = job->estimateRemaining(percent).toString();
            standardItem->setText(QString("%1% (%2)").arg(percent).arg(remaining));
        }
    }
}

void TaskQueue::onFinished(AbstractTask* job, bool isSuccess, QString time)
{
    QStandardItem* item = job->standardItem();
    if (item) {
		QStyle* style = QApplication::style();
		QIcon icon = style->standardIcon(QStyle::SP_TitleBarMenuButton);
        if (isSuccess) {
            const QTime& time = QTime::fromMSecsSinceStartOfDay(job->time().elapsed());
            item->setText(time.toString());
            item->setToolTip(tr("Elapsed Hours:Minutes:Seconds"));
        } else if (job->stopped()) {
            item->setText(tr("stopped"));

        } else {
            item->setText(tr("failed").append(' ').append(time));
        }

        // Remove any touched or incomplete pending proxy files
        if (job->stopped() || !isSuccess)
        if (job->objectName().contains("proxies") && job->objectName().contains(".pending")) {
            QFile::remove(job->objectName());
        }

        item = JOBS.item(item->row(), TaskQueue::COLUMN_ICON);
        if (item)
            item->setIcon(icon);
    }
    startNextJob();
}

void TaskQueue::startNextJob()
{
    if (m_paused) return;
    QMutexLocker locker(&m_mutex);
    if (!m_jobs.isEmpty()) {
        foreach(AbstractTask* job, m_jobs) {
            // if there is already a job started or running, then exit
            if (job->ran() && job->state() != QProcess::NotRunning)
                break;
            // otherwise, start first non-started job and exit
            if (!job->ran()) {
                job->start();
                break;
            }
        }
    }
}

AbstractTask* TaskQueue::jobFromIndex(const QModelIndex& index) const
{
    return m_jobs.at(index.row());
}

void TaskQueue::pause()
{
    m_paused = true;
}

void TaskQueue::resume()
{
    m_paused = false;
    startNextJob();
}

bool TaskQueue::isPaused() const
{
    return m_paused;
}

bool TaskQueue::hasIncomplete() const
{
    foreach (AbstractTask* job, m_jobs) {
        if (!job->ran() || job->state() == QProcess::Running)
            return true;
    }
    return false;
}

void TaskQueue::remove(const QModelIndex& index)
{
    int row = index.row();
    removeRow(index.row());
    m_mutex.lock();

    AbstractTask* job = m_jobs.at(row);
    m_jobs.removeOne(job);
    delete job;

    m_mutex.unlock();
}

void TaskQueue::removeFinished()
{
    QMutexLocker locker(&m_mutex);
    auto row = 0;
    foreach (AbstractTask* job, m_jobs) {
        if (job->ran() && job->state() != QProcess::Running) {
            removeRow(row);
            m_jobs.removeOne(job);
            delete job;
        } else {
            ++row;
        }
    }
}
