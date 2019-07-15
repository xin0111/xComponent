#ifndef XTABLEWIDGET_H__
#define XTABLEWIDGET_H__

#include <QTableWidget>

class xTableWidget :public QTableWidget
{
	Q_OBJECT
public:
	xTableWidget();
	~xTableWidget();
	//选中单行
	void setSelectSingleRow();
	//列宽 自动调整
	void setAutoResizeHeader();
	//删除所有行
	void clearAllRows();
	//背景颜色
	void setBackgroundColor(QColor color);
};

#endif // XTABLEWIDGET_H__
