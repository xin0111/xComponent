#pragma once
#include <QString>
#include <QJsonObject>
#include <QBoxLayout>

class QComboBox;
class QAbstractButton;


class xtHelpTools
{
public:
	static 	void adjustComboBoxViewWidth(QComboBox* combox);

	static QJsonObject getJsonObjectFromString(const QString& jsonString);

	static  QString getStringFromJsonObject(const QJsonObject& jsonObject);

	static QColor buttonBackgroundColor(QAbstractButton *button);

	static void hideComboBoxRow(QComboBox *combox, int nRow);

	static void boxlayoutVisible(QBoxLayout *boxLayout, bool bVisible);

	static void gridLayoutRowEnabled(QGridLayout *gridLayout, int row, bool bEnable);
};

