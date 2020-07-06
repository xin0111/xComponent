#include "xjsonviewgui.h"

xJsonViewGui::xJsonViewGui(QWidget *parent)
: QDialog(parent)
{
	ui.setupUi(this);
	QObject::connect(ui.lineEdit_search, SIGNAL(textChanged(QString)), this,
		SLOT(setSearchFilter(QString)));
	QObject::connect(ui.pushButton_display, SIGNAL(clicked()), this,
		SLOT(setExpandCollapse()));

	connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(ui.buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
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
	ui.pushButton_display->setText("ExpandAll");
	ui.jsonView->resizeColumnToContents(0);
}
#include <QDebug>
void xJsonViewGui::setExpandCollapse()
{
	QPushButton *btn = qobject_cast<QPushButton*>(sender());
	if (btn)
	{
		if (btn->text() == "ExpandAll")
		{
			btn->setText("CollapseAll");
			ui.jsonView->expandAllEx();
		}
		else
		{
			btn->setText("ExpandAll");
			ui.jsonView->collapseAll();
		}
	}
}

void xJsonViewGui::on_lineEdit_search_returnPressed()
{
	ui.pushButton_display->setFocus();
	setExpandCollapse();
}