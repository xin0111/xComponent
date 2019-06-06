#include "xvalidator.h"
#include <QMessageBox>
#include <QLineEdit>
#include <QToolTip>


xValidator::xValidator(int bottom, int top, QLineEdit *lineEdit)
: QValidator(lineEdit),
m_dec(0),
m_flag(INT_FLAG)
{
	m_bottom = bottom;
	m_top = top;
	if (lineEdit)
	{
		lineEdit->setToolTip(QString::fromLocal8Bit("[%1,%2],int")
			.arg(m_bottom.toInt())
			.arg(m_top.toInt()));
	}
}

xValidator::xValidator(double bottom, double top, int decimals, QLineEdit *lineEdit)
: QValidator(lineEdit),
m_flag(DOUBLE_FLAG)
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

QValidator::State xValidator::validate(QString & input, int &) const
{
	auto bottom = xValidator::INT_FLAG ?
		m_bottom.toInt() : m_bottom.toDouble();
	auto top = xValidator::INT_FLAG ?
		m_top.toInt() : m_top.toDouble();

	//符号
	if (input.isEmpty())
		return QValidator::Intermediate;

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
	// 数值大小
	//+
	if (top > 0 && (i > top || (bottom <0 && i<bottom)))
		return	invalidTip();
	//-
	if (top < 0 && i < bottom)
		return	invalidTip();

	int iInt = getNumberInt(i);
	int ni = QString::number(iInt).length();
	if (i < 0) ni -= 1;
	int nb = QString::number(getNumberInt(bottom)).length();
	if (bottom < 0) nb -= 1;
	int nt = QString::number(getNumberInt(top)).length();
	if (top < 0) nb -= 1;

	if ((iInt* pow(10, nb - ni) + (i - iInt)) >= bottom
		|| (iInt*pow(10, nt - ni) + (i - iInt)) <= top)
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


