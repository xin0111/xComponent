#include "xvalidator.h"
#include <QMessageBox>
#include <QLineEdit>
#include <QToolTip>


xValidator::xValidator(int bottom, int top, QLineEdit *lineEdit)
: QValidator(lineEdit),
m_dec(0),
m_flag(INT_FLAG),
m_bBottomClose(true),
m_bTopClose(true)
{
	m_bottom = bottom;
	m_top = top;
	if (lineEdit)
	{
		lineEdit->setToolTip(QString::fromLocal8Bit("[%1,%2],int")
			.arg(m_bottom.toInt())
			.arg(m_top.toInt()));
	}
	init(lineEdit);
}
xValidator::xValidator(int bottom, int top, bool bBottomClose, bool bTopClose, QLineEdit *lineEdit)
{
	m_bottom = bottom;
	m_top = top;
	m_bBottomClose = bBottomClose;
	m_bTopClose = bTopClose;
	if (lineEdit)
	{
		lineEdit->setToolTip(QString::fromLocal8Bit("%1%2,%3%4,int")
			.arg(m_bBottomClose ? "[":"(")
			.arg(m_bottom.toInt())
			.arg(m_top.toInt())
			.arg(m_bTopClose ? "]" : ")"));
	}
	init(lineEdit);
}


xValidator::xValidator(double bottom, double top, int decimals, QLineEdit *lineEdit)
: QValidator(lineEdit),
m_flag(DOUBLE_FLAG),
m_bBottomClose(true),
m_bTopClose(true)
{
	m_bottom = bottom;
	m_top = top;
	m_dec = decimals;
	if (lineEdit)
	{
		lineEdit->setToolTip(QString::fromLocal8Bit("[%1,%2],double,%3")
			.arg(m_bottom.toDouble())
			.arg(m_top.toDouble())
			.arg(decimals));
	}
	init(lineEdit);
}

xValidator::xValidator(double bottom, double top, bool bBottomClose, bool bTopClose, int decimals, QLineEdit *lineEdit)
{
	m_bottom = bottom;
	m_top = top;
	m_bBottomClose = bBottomClose;
	m_bTopClose = bTopClose;
	m_dec = decimals;
	if (lineEdit)
	{
		lineEdit->setToolTip(QString::fromLocal8Bit("%1%2,%3%4,double,%5")
			.arg(m_bBottomClose ? "[" : "(")
			.arg(m_bottom.toDouble())
			.arg(m_top.toDouble())
			.arg(m_bTopClose ? "]" : ")")
			.arg(decimals));
	}
	init(lineEdit);
}

xValidator::~xValidator()
{
}

int getNumberInt(double d)
{
	if (d > 0)
		return floor(d);//返回小于或等于指定数字的最大整数

	return ceil(d); // 返回大于或等于指定数字的最小整数。 
}

QValidator::State xValidator::validate(QString & input, int &pos) const
{
	double bottom = m_bottom.toDouble();
	double top = m_top.toDouble();

	//符号
	if (input.isEmpty())
		return QValidator::Intermediate;

	if (pos>0 &&input.size()>=pos)
	{
		//空格处理
		QChar word = input.at(pos - 1);
		if (word.isSpace())
		{
			return QValidator::Invalid;
		}
	}
	if (bottom >= 0 && input.startsWith('-'))
		return	invalidTip();

	if (top < 0 && !input.startsWith('-'))
		return	invalidTip();

	if ((input.startsWith('-') || input.startsWith('+')) 
		&& input.length() == 1)
		return QValidator::Intermediate;

	//小数位数
	if (input.indexOf(".") != -1)
	{
		if (xValidator::INT_FLAG == m_flag)
			return	invalidTip();

		QStringList numList = input.split(".");
		if (numList.at(1).length() > m_dec)
			return	invalidTip();
	}

	//数字
	bool ok = false;
	double i = input.toDouble(&ok); 
	if (!ok)
		return	invalidTip();

	if (DOUBLE_FLAG == m_flag)
	{
		if (i == 0 && (top == 0 || bottom == 0))
		{// 0.000 处理
			QRegExp rx(QString("^[+|-]?0+.0{%1}$").arg(m_dec));
			if (rx.exactMatch(input))
			{
				return invalidTip();
			}
			return QValidator::Acceptable;
		}
	}

	if (!m_bBottomClose && m_bTopClose)
	{//左开右闭

		// 数值大小
		//+
		if (i > top)
			return	invalidTip();
		//-
		if (i == bottom)
			return	invalidTip();
	}
	else if (m_bBottomClose && !m_bTopClose)
	{//左闭右开

		// 数值大小
		//+
		if (i >= top)
			return	invalidTip();
	}
	else if (!m_bBottomClose && !m_bTopClose)
	{//左开右开

		// 数值大小
		//+
		if (i >= top)
			return	invalidTip();
		//-
		if (i == bottom)
			return	invalidTip();
	}
	else if (m_bBottomClose && m_bTopClose)
	{//左闭右闭

		// 数值大小
		//+
		if (i > top)
			return	invalidTip();
	}

	int iInt = getNumberInt(i);
	int ni = QString::number(iInt).length();
	if (i < 0) ni -= 1;
	int nb = QString::number(getNumberInt(bottom)).length();
	if (bottom < 0) nb -= 1;
	int nt = QString::number(getNumberInt(top)).length();
	if (top < 0) nb -= 1;

	if ((iInt* pow(10.0, nb - ni) + (i - iInt)) >= bottom
		|| (iInt*pow(10.0, nt - ni) + (i - iInt)) <= top)
		return	QValidator::Acceptable;

	return invalidTip();
}

QValidator::State xValidator::invalidTip() const
{
	QLineEdit *lineEdit = qobject_cast<QLineEdit*>(this->parent());
	QWidget* widget = (QWidget*)lineEdit->parent();
	if (lineEdit && widget)
		QToolTip::showText(widget->mapToGlobal(lineEdit->pos()),
		lineEdit->toolTip(), lineEdit);
	return QValidator::Invalid;
}

void xValidator::slotCheckMin()
{
	QLineEdit* lineEdit = qobject_cast<QLineEdit*>(sender());
	if (lineEdit != nullptr)
	{
		double val = lineEdit->text().toDouble();
		if (val < m_bottom.toDouble())
		{
			lineEdit->clear();
		}
	}
}

void xValidator::init(QLineEdit *lineEdit)
{
	if (lineEdit)
	{
		lineEdit->setPlaceholderText(lineEdit->toolTip());
		connect(lineEdit, SIGNAL(editingFinished()), this, SLOT(slotCheckMin()));
	}
}
