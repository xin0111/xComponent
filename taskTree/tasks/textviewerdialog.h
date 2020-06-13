#ifndef TEXTVIEWERDIALOG_H
#define TEXTVIEWERDIALOG_H

#include <QDialog>

namespace Ui {
    class TextViewerDialog;
}

class TextViewerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TextViewerDialog(QWidget *parent = 0);
    ~TextViewerDialog();
    void setText(const QString& s);

private slots:
    void on_buttonBox_accepted();

public:
	static bool warnIfNotWritable(const QString& filePath, QWidget* parent, const QString& caption, bool remove = false);
private:
    Ui::TextViewerDialog *ui;
};

#endif // TEXTVIEWERDIALOG_H
