#include "textviewerdialog.h"
#include "ui_textviewerdialog.h"

#include <QMessageBox>
#include <QFileDialog>

TextViewerDialog::TextViewerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextViewerDialog)
{
    ui->setupUi(this);
}

TextViewerDialog::~TextViewerDialog()
{
    delete ui;
}

void TextViewerDialog::setText(const QString &s)
{
    ui->plainTextEdit->setPlainText(s);
}

void TextViewerDialog::on_buttonBox_accepted()
{
    QString path = ".";
    QString caption = tr("Save Text");
    QString nameFilter = tr("Text Documents (*.txt);;All Files (*)");
    QString filename = QFileDialog::getSaveFileName(this, caption, path, nameFilter);
    if (!filename.isEmpty()) {
        if (warnIfNotWritable(filename, this, caption))
            return;
        QFile f(filename);
        f.open(QIODevice::WriteOnly | QIODevice::Text);
        f.write(ui->plainTextEdit->toPlainText().toUtf8());
        f.close();
    }
}

bool TextViewerDialog::warnIfNotWritable(const QString& filePath, QWidget* parent, const QString& caption, bool remove)
{
	// Returns true if not writable.
	if (!filePath.isEmpty() && !filePath.contains("://")) {
		// Do a hard check by writing to the file.
		QFile file(filePath);
		file.open(QIODevice::WriteOnly | QIODevice::Append);
		if (file.write("") < 0) {
			QFileInfo fi(filePath);
			QMessageBox::warning(parent, caption,
				QObject::tr("Unable to write file %1\n"
				"Perhaps you do not have permission.\n"
				"Try again with a different folder.")
				.arg(fi.fileName()));
			return true;
		}
		else if (remove) {
			file.remove();
		}
	}
	return false;
}