#ifndef XTABLEWIDGET_H__
#define XTABLEWIDGET_H__

#include <QTableWidget>

class xTableWidget :public QTableWidget
{
	Q_OBJECT
public:
	xTableWidget();
	~xTableWidget();
	//ѡ�е���
	void setSelectSingleRow();
	//�п� �Զ�����
	void setAutoResizeHeader();
	//ɾ��������
	void clearAllRows();
	//������ɫ
	void setBackgroundColor(QColor color);
};

#endif // XTABLEWIDGET_H__
