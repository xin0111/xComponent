#ifndef TASKSTREE_H__
#define TASKSTREE_H__

#include <QTreeView>

class AbstractTask;
class QStandardItem;


class TasksTree :public QTreeView
{
	Q_OBJECT
public:
	explicit TasksTree(QWidget *parent = 0);
	~TasksTree();

	AbstractTask* currentJob() const;
public slots:
	void onJobAdded();
	void onProgressUpdated(QStandardItem* item, int percent);
protected:
	void resizeEvent(QResizeEvent *event);
private slots:
	void on_customContextMenuRequested(const QPoint &pos);
	void on_actionStopJob_triggered();
	void on_actionViewLog_triggered();
	void on_actionRun_triggered();
	void on_doubleClicked(const QModelIndex &index);
	void on_actionRemove_triggered();
	void on_actionRemoveFinished_triggered();
private:
	QAction* actionRun;
	QAction* actionStopJob;
	QAction* actionRemove;
	QAction* actionViewLog;
	QAction* actionRemoveFinished;
};
#endif // TASKSTREE_H__

