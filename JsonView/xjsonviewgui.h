#ifndef XJSONVIEWGUI_H
#define XJSONVIEWGUI_H

#include <QWidget>
#include "ui_xjsonviewgui.h"

class xJsonViewGui : public QWidget
{
	Q_OBJECT

public:
	xJsonViewGui(QWidget *parent = 0);
	~xJsonViewGui();
	QJsonModel* getJsonModel();
protected slots:
	void setSearchFilter(const QString& str);
	void setExpandCollapse();
private:
	Ui::xJsonViewGui ui;
};

#endif // XJSONVIEWGUI_H
