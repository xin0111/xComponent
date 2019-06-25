#include <QComboBox>
#include <QFontMetrics>
#include <QAbstractItemView>
#include <QJsonDocument>
#include <QDebug>
#include <QAbstractButton>
#include <QStandardItemModel>
#include <QScrollBar>

#include "xHelpTools.h"

void xtHelpTools::adjustComboBoxViewWidth(QComboBox* combox)
{
	if (nullptr == combox) return;

	int nCol = combox->count();
	int nMaxLen = 0, nItemWidth = 0;
	QFontMetrics fm(combox->view()->font());
	for (int idx = 0; idx < nCol; ++idx)
	{
		nItemWidth = fm.width(combox->itemText(idx));

		nMaxLen = std::max(nMaxLen, nItemWidth);
	}
	nMaxLen += combox->view()->verticalScrollBar()->depth();
	combox->view()->setMinimumWidth(nMaxLen);
}


// QString >> QJson  
QJsonObject xtHelpTools::getJsonObjectFromString(const QString& jsonString){
	QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toLocal8Bit().data());
	if (jsonDocument.isNull()){
		qDebug() << "===> please check the string " << jsonString.toLocal8Bit().data();
	}
	QJsonObject jsonObject = jsonDocument.object();
	return jsonObject;
}

// QJson >> QString  
QString xtHelpTools::getStringFromJsonObject(const QJsonObject& jsonObject){
	return QString(QJsonDocument(jsonObject).toJson());
}

QColor xtHelpTools::buttonBackgroundColor(QAbstractButton *button)
{
	QColor bgColor(0,0,0);
	if (button)
		bgColor = button->palette().color(QPalette::Background);
	return bgColor;
}

void xtHelpTools::hideComboBoxRow(QComboBox *combox,int nRow)
{
	if (NULL == combox) return;
	if (QStandardItemModel *m = qobject_cast<QStandardItemModel*>(combox->model())) {
		QStandardItem *hiddenRoot = m->invisibleRootItem();
		//设置不显示
		hiddenRoot->child(nRow, 0)->setSizeHint(QSize(0, 0));
		hiddenRoot->child(nRow, 0)->setEnabled(false);
		//切换当前显示
		combox->setCurrentIndex(-1);
	}
}


void xtHelpTools::boxlayoutVisible(QBoxLayout *boxLayout, bool bVisible)
{
	if (NULL == boxLayout) return;

	int nColum = boxLayout->count();
	for (int i = 0; i < nColum; i++)
	{
		auto item = boxLayout->itemAt(i);
		if (item && item->widget())
		{
			item->widget()->setVisible(bVisible);
		}
	}
}

void xtHelpTools::gridLayoutRowEnabled(QGridLayout *gridLayout,
	int row, bool bEnable)
{
	if (NULL == gridLayout) return;

	int columnCount = gridLayout->columnCount();
	for (int k = 0; k < columnCount; ++k)
	{
		QLayoutItem * item = gridLayout->itemAtPosition(row, k);
		if (item)
		{
			item->widget()->setEnabled(bEnable);
			//item->widget()->setVisible(bEnable);
		}
	}
}
