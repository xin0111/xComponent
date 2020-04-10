/***********************************************
    Copyright (C) 2014  Schutz Sacha
    This file is part of QJsonModel (https://github.com/dridk/QJsonmodel).

    QJsonModel is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QJsonModel is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QJsonModel.  If not, see <http://www.gnu.org/licenses/>.

**********************************************/

#include <QApplication>
#include <QTreeView>
#include <QFile>
#include <QDomDocument>
#include <string>
#include "qjsonmodel.h"
#include "xjsonviewgui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	QString sJson;
	QDomDocument doc;
	QFile file(("./troops.hlib"));
	if (file.open(QIODevice::ReadOnly))
	{
		if (doc.setContent(&file)){			
		}		
	}
	file.close();
	QDomNodeList equipList = doc.elementsByTagName("equip");
	for (int index = 0; index != equipList.size(); ++index){
		QDomNode node = equipList.at(index);
		if (!node.isElement()) continue;
		QDomElement equipEle = node.toElement();
	
		sJson = equipEle.attribute("seria");

		break;
	}

	//sJson = R"({
 //                      "address":
 //                      {
	//					    "type":
	//						{
 //                            "number": "212 555-1234"
 //                          }
 //                      }
 //                  })";
	xJsonViewGui gui;
	gui.getJsonModel()->loadJson(sJson);
	gui.show();

	return a.exec();
}
