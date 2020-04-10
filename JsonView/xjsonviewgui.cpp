#include "xjsonviewgui.h"

xJsonViewGui::xJsonViewGui(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	QObject::connect(ui.lineEdit_search, SIGNAL(textChanged(QString)), this,
		SLOT(setSearchFilter(QString)));
	QObject::connect(ui.pushButton_display, SIGNAL(clicked()), this,
		SLOT(setExpandCollapse()));
}

xJsonViewGui::~xJsonViewGui()
{

}

QJsonModel* xJsonViewGui::getJsonModel()
{
	return ui.jsonView->getJsonModel();
}

void xJsonViewGui::setSearchFilter(const QString& str)
{
	ui.jsonView->selectionModel()->clear();
	ui.jsonView->setSearchFilter(str);
}

void xJsonViewGui::setExpandCollapse()
{
	QPushButton *btn = qobject_cast<QPushButton*>(sender());
	if (btn)
	{
		if (btn->text() == "ExpandAll")
		{
			btn->setText("CollapseAll");
			ui.jsonView->expandAll();
		}
		else
		{
			btn->setText("ExpandAll");
			ui.jsonView->collapseAll();
		}
	}
}
