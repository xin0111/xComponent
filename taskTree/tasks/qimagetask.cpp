#include "qimagetask.h"

#include <QImage>
#include <QImageReader>
#include <QRunnable>
#include <QtConcurrent/QtConcurrent>

QImageTask::QImageTask(const QString& destFilePath, const QString& srcFilePath, const int height)
    : AbstractTask(srcFilePath)
    , m_srcFilePath(srcFilePath)
    , m_destFilePath(destFilePath)
    , m_height(height)
{
    setLabel(tr("Make proxy for %1").arg(baseName(srcFilePath)));
}

QImageTask::~QImageTask()
{
    if (m_destFilePath.contains("proxies") && m_destFilePath.contains(".pending.")){
        QFile::remove(m_destFilePath);
    }
}

void QImageTask::start()
{
    AbstractTask::start();
    QtConcurrent::run([=]() {
        appendToLog(QString("Reading source image \"%1\"\n").arg(m_srcFilePath));
        QImageReader reader;
        reader.setAutoTransform(true);
        reader.setDecideFormatFromContent(true);
        reader.setFileName(m_srcFilePath);
        QImage image(reader.read());
        if (!image.isNull()) {
            image = image.scaledToHeight(m_height, Qt::SmoothTransformation);
            if (image.save(m_destFilePath)) {
                appendToLog(QString("Successfully saved image as \"%1\"\n").arg(m_destFilePath));
                QMetaObject::invokeMethod(this, "onFinished", Qt::QueuedConnection, Q_ARG(int, 0));
            } else {
                appendToLog(QString("Failed to save image as \"%1\"\n").arg(m_destFilePath));
                QMetaObject::invokeMethod(this, "onFinished", Qt::QueuedConnection, Q_ARG(int, 1));
            }
        } else {
            appendToLog(QString("Failed to read source image \"%1\"\n").arg(m_srcFilePath));
            QMetaObject::invokeMethod(this, "onFinished", Qt::QueuedConnection, Q_ARG(int, 1));
        }
    });
}

QString QImageTask::baseName(const QString &filePath)
{
	QString s = filePath;
	// Only if absolute path and not a URI.
	if (s.startsWith('/') || s.midRef(1, 2) == ":/" || s.midRef(1, 2) == ":\\")
		s = QFileInfo(s).fileName();
	return s;
}