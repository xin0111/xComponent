#include "TasksTree.h"
#include "TaskQueue.h"
#include "textviewerdialog.h"
#include <QtWidgets>

TasksTree::TasksTree(QWidget *parent)
	:QTreeView(parent)
{
	actionStopJob = new QAction(this);
	actionStopJob->setObjectName(QStringLiteral("actionStopJob"));
	actionStopJob->setText(QStringLiteral("停止"));
	actionViewLog = new QAction(this);
	actionViewLog->setObjectName(QStringLiteral("actionViewLog"));
	actionViewLog->setText(QStringLiteral("查看日志"));
	actionRun = new QAction(this);
	actionRun->setObjectName(QStringLiteral("actionRun"));
	actionRun->setText(QStringLiteral("运行"));
	actionRemove = new QAction(this);
	actionRemove->setObjectName(QStringLiteral("actionRemove"));
	actionRemove->setText(QStringLiteral("移除"));
	actionRemoveFinished = new QAction(this);
	actionRemoveFinished->setObjectName(QStringLiteral("actionRemoveFinished"));
	actionRemoveFinished->setText(QStringLiteral("移除已完成"));
	//
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	this->setEditTriggers(QAbstractItemView::NoEditTriggers);
	this->setAlternatingRowColors(true);
	this->setTextElideMode(Qt::ElideMiddle);
	this->setRootIsDecorated(false);
	this->setUniformRowHeights(true);
	this->header()->setVisible(false);
	//
	this->setModel(&JOBS);
	QHeaderView* header = this->header();
	header->setStretchLastSection(false);
	header->setSectionResizeMode(TaskQueue::COLUMN_ICON, QHeaderView::Fixed);
	this->setColumnWidth(TaskQueue::COLUMN_ICON, 24);
	header->setSectionResizeMode(TaskQueue::COLUMN_OUTPUT, QHeaderView::Stretch);
	header->setSectionResizeMode(TaskQueue::COLUMN_STATUS, QHeaderView::ResizeToContents);
	//
	connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(on_customContextMenuRequested(const QPoint &)));
	connect(this, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(on_doubleClicked(const QModelIndex &)));
	QMetaObject::connectSlotsByName(this);
}


TasksTree::~TasksTree()
{
}

AbstractTask* TasksTree::currentJob() const
{
	QModelIndex index = this->currentIndex();
	if (!index.isValid()) return 0;
	return JOBS.jobFromIndex(index);
}

void TasksTree::onJobAdded()
{
	QModelIndex index = JOBS.index(JOBS.rowCount() - 1, TaskQueue::COLUMN_OUTPUT);
	QProgressBar* progressBar = new QProgressBar;
	progressBar->setMinimum(0);
	progressBar->setMaximum(100);
	progressBar->setAutoFillBackground(true);
	progressBar->setTextVisible(false);
	QHBoxLayout* layout = new QHBoxLayout(progressBar);
	QLabel* label = new QLabel;
	layout->addWidget(label);
	layout->setContentsMargins(0, 0, 0, 0);
	this->setIndexWidget(index, progressBar);
	this->resizeColumnToContents(TaskQueue::COLUMN_STATUS);
	label->setToolTip(JOBS.data(index).toString());
	label->setText(label->fontMetrics().elidedText(
		JOBS.data(index).toString(), Qt::ElideMiddle, this->columnWidth(TaskQueue::COLUMN_OUTPUT)));
	connect(JOBS.jobFromIndex(index), SIGNAL(progressUpdated(QStandardItem*, int)), SLOT(onProgressUpdated(QStandardItem*, int)));
	show();
	raise();
}

void TasksTree::onProgressUpdated(QStandardItem* item, int percent)
{
	if (item) {
		QModelIndex index = JOBS.index(item->row(), TaskQueue::COLUMN_OUTPUT);
		QProgressBar* progressBar = qobject_cast<QProgressBar*>(this->indexWidget(index));
		if (progressBar)
			progressBar->setValue(percent);
	}
}

void TasksTree::resizeEvent(QResizeEvent *event)
{
	QTreeView::resizeEvent(event);
	foreach(QLabel* label, this->findChildren<QLabel*>()) {
		label->setText(label->fontMetrics().elidedText(
			label->toolTip(), Qt::ElideMiddle, this->columnWidth(TaskQueue::COLUMN_OUTPUT)));
	}

}

void TasksTree::on_customContextMenuRequested(const QPoint &pos)
{
	QModelIndex index = this->currentIndex();
	QMenu menu(this);
	AbstractTask* job = index.isValid() ? JOBS.jobFromIndex(index) : nullptr;
	if (job) {
		if (job->ran() && job->state() == QProcess::NotRunning && job->exitStatus() == QProcess::NormalExit) {
			menu.addActions(job->successActions());
		}
		if (job->stopped() || (JOBS.isPaused() && !job->ran()))
			menu.addAction(actionRun);
		if (job->state() == QProcess::Running)
			menu.addAction(actionStopJob);
		else
			menu.addAction(actionRemove);
		if (job->ran())
			menu.addAction(actionViewLog);
		menu.addActions(job->standardActions());
	}
	for (auto job : JOBS.jobs()) {
		if (job->ran() && job->state() != QProcess::Running) {
			menu.addAction(actionRemoveFinished);
			break;
		}
	}
	menu.exec(mapToGlobal(pos));
}

void TasksTree::on_actionStopJob_triggered()
{
	QModelIndex index = this->currentIndex();
	if (!index.isValid()) return;
	AbstractTask* job = JOBS.jobFromIndex(index);
	if (job) job->stop();
}

void TasksTree::on_actionViewLog_triggered()
{
	QModelIndex index = this->currentIndex();
	if (!index.isValid()) return;
	AbstractTask* job = JOBS.jobFromIndex(index);
	if (job) {
		TextViewerDialog dialog(this);
		dialog.setWindowTitle(tr("Job Log"));
		dialog.setText(job->log());
		dialog.exec();
	}
}

void TasksTree::on_actionRun_triggered()
{
	QModelIndex index = this->currentIndex();
	if (!index.isValid()) return;
	AbstractTask* job = JOBS.jobFromIndex(index);
	if (job) job->start();
}

void TasksTree::on_doubleClicked(const QModelIndex &index)
{
	AbstractTask* job = JOBS.jobFromIndex(index);
	if (job && job->ran() && job->state() == QProcess::NotRunning && job->exitStatus() == QProcess::NormalExit) {
		foreach(QAction* action, job->successActions()) {
			if (action->text() == "Open") {
				action->trigger();
				break;
			}
		}
	}
}

void TasksTree::on_actionRemove_triggered()
{
	QModelIndex index = this->currentIndex();
	if (!index.isValid()) return;
	JOBS.remove(index);
}

void TasksTree::on_actionRemoveFinished_triggered()
{
	JOBS.removeFinished();
}

