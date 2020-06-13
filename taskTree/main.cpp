#include <QApplication>
#include <QProcess>
#include <QMainWindow>
#include "tasks\TasksTree.h"
#include "tasks\TaskQueue.h"
#include "tasks\qimagetask.h"


#define EXIT_RESTART (42)

class QtGuiClass : public QMainWindow
{
public:
	QtGuiClass(QWidget *parent = Q_NULLPTR);
	~QtGuiClass();
protected:
	void closeEvent(QCloseEvent* event);
};

void QtGuiClass::closeEvent(QCloseEvent* event)
{
	//QApplication::exit(EXIT_RESTART);
}


int main(int argc, char* argv[])
{
	QApplication a(argc,argv);

	TasksTree tree;
	tree.show();

	AbstractTask* task = new QImageTask("E:/test/a.png",
		"E:/a.png", 100);
	JOBS.add(task);

	int result = a.exec();

	if (EXIT_RESTART == result)
	{

#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
		::qputenv("LIBGL_ALWAYS_SOFTWARE",
			Settings.drawMethod() == Qt::AA_UseSoftwareOpenGL && !Settings.playerGPU()
			? "1" : "0");
#endif
		QProcess* restart = new QProcess;
		QStringList args = a.arguments();
		if (!args.isEmpty())
			args.removeFirst();
		restart->start(a.applicationFilePath(), args, QIODevice::NotOpen);
		result = EXIT_SUCCESS;
	}
	return 0;
}