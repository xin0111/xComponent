#ifndef XJSONVIEWGUI_H
#define XJSONVIEWGUI_H

#include <QDialog>
#include "ui_xjsonviewgui.h"

class xJsonViewGui : public QDialog
{
	Q_OBJECT

public:
	xJsonViewGui(QWidget *parent = 0);
	~xJsonViewGui();
	QJsonModel* getJsonModel();
protected slots:
	void setSearchFilter(const QString& str);
	void setExpandCollapse();
	void on_lineEdit_search_returnPressed();
private:
	Ui::xJsonViewGui ui;
};

#endif // XJSONVIEWGUI_H
