#ifndef XVALIDATOR_H__
#define XVALIDATOR_H__

#include <QValidator>

class QLineEdit;
class xValidator :public QValidator
{
	Q_OBJECT
public:
	enum NumFlag
	{
		DOUBLE_FLAG,
		INT_FLAG
	};
	xValidator(int bottom, int top, QLineEdit *lineEdit);
	xValidator(int bottom, int top, bool bBottomClose, bool bTopClose, QLineEdit *lineEdit);
	xValidator(double bottom, double top, int decimals, QLineEdit *lineEdit);
	xValidator(double bottom, double top, bool bBottomClose, bool bTopClose, int decimals, QLineEdit *lineEdit);
	~xValidator();
	 QValidator::State validate(QString &, int &) const Q_DECL_OVERRIDE;
	 QValidator::State invalidTip()  const;
private:
	Q_DISABLE_COPY(xValidator)
	QVariant m_bottom;
	QVariant m_top;
	bool m_bBottomClose;
	bool m_bTopClose;
	int m_dec;
	NumFlag m_flag;
};

#endif // XVALIDATOR_H__
