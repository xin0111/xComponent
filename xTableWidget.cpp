#include "xTableWidget.h"

#include <QHeaderView>

xTableWidget::xTableWidget()
{

}


xTableWidget::~xTableWidget()
{
}

void xTableWidget::setSelectSingleRow()
{
	setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
	setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
}

void xTableWidget::setAutoResizeHeader()
{
	horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

void xTableWidget::clearAllRows()
{
	int nRow = rowCount();
	for (int i = 0; i < nRow; ++i)
	{
		removeRow(0);
	}
}

void xTableWidget::setBackgroundColor(QColor color)
{
	horizontalHeader()->setBackgroundRole(QPalette::ColorRole::Background);
	horizontalHeader()->setAutoFillBackground(true);
	horizontalHeader()->setPalette(QPalette(color));
}
