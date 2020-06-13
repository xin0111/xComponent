#ifndef QIMAGETASK_H
#define QIMAGETASK_H

#include "abstracttask.h"
#include <QSize>

class QImageTask : public AbstractTask
{
    Q_OBJECT
public:
    QImageTask(const QString& destFilePath, const QString& srcFilePath, const int height);
    virtual ~QImageTask();
    void start();
	void execute();
private:
	QString baseName(const QString &filePath);
private:
    QString m_srcFilePath;
    QString m_destFilePath;
    int m_height;
};

#endif // QIMAGEJOB_H
