#pragma once

#include <QWidget>
#include <QListWidgetItem>
#include "pugixml/pugixml.hpp"

namespace Ui { class xQueryJsonGui; };

class xQueryJsonGui : public QWidget
{
	Q_OBJECT

public:
	xQueryJsonGui(QWidget *parent = Q_NULLPTR);
	~xQueryJsonGui();
public slots:
void onLoadFile();
void showJsonItem(QListWidgetItem *pItem);
void onRefresh();
void onSaveJson();
private:
	void addJsonItems(const pugi::xml_document& doc,const QString &sxPath);
	void updateJsonTab(const QString& fileName);
private:
	Ui::xQueryJsonGui *ui;
	pugi::xml_document mdoc;
};
