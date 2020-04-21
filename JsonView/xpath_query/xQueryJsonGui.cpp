#include "xQueryJsonGui.h"
#include "ui_xQueryJsonGui.h"
#include <QFileDialog>
#include "xjsonviewgui.h"

Q_DECLARE_METATYPE(pugi::xml_attribute)

xQueryJsonGui::xQueryJsonGui(QWidget *parent)
	: QWidget(parent)
{
	ui = new Ui::xQueryJsonGui();
	ui->setupUi(this);

	new QListWidgetItem(QStringLiteral("TROOPINFO//EQUIP//@seria"),ui->listWidget);
	new QListWidgetItem(QStringLiteral("TROOPINFO//DEVICE//@seria"), ui->listWidget);

	connect(ui->toolButton, SIGNAL(clicked()), this, SLOT(onLoadFile()));
	connect(ui->pushButton_refresh, SIGNAL(clicked()), this, SLOT(onRefresh()));
	connect(ui->pushButton_save, SIGNAL(clicked()), this, SLOT(onSaveJson()));
}

xQueryJsonGui::~xQueryJsonGui()
{
	delete ui;
}

void xQueryJsonGui::onLoadFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
		"",
		tr("(*)"));
	
	updateJsonTab(fileName);
}

void xQueryJsonGui::showJsonItem(QListWidgetItem *pItem)
{
	xJsonViewGui pView;
	QJsonModel* pJson = pView.getJsonModel();
	pJson->loadJson(pItem->text());
	
	if(pView.exec() == QDialog::Accepted)
	{
		QString strJson = pJson->sjson();
		if (strJson != pItem->text())
		{
			auto kk = pItem->data(Qt::UserRole + 1);
			pugi::xml_attribute attr = pItem->data(Qt::UserRole + 1).value<pugi::xml_attribute>();
		
			attr.set_value(strJson.toStdString().c_str());
			pItem->setText(strJson);
		}
	}
}

void xQueryJsonGui::onRefresh()
{
	QString sPath = ui->label->text();
	if (sPath.isEmpty()) return;
	updateJsonTab(sPath);
}

void xQueryJsonGui::onSaveJson()
{
	if (ui->tabWidget->count() <= 0) return;

	QString sPath = ui->label->text();
	if (sPath.isEmpty()) return;
	mdoc.save_file(sPath.toStdString().c_str());
}

void xQueryJsonGui::addJsonItems(const pugi::xml_document& doc, const QString &sxPath)
{
	pugi::xpath_node_set ns = doc.select_nodes(sxPath.toStdString().c_str());
	if (ns.empty()) return;
	QListWidget *pListWgt = new QListWidget();
	
	connect(pListWgt, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(showJsonItem(QListWidgetItem *)));

	for (size_t i = 0; i < ns.size(); i++)
	{
		const std::string& tx = ns[i].attribute().value();

		
		QString str = QString::fromStdString(tx);
		
		QListWidgetItem*pItem = new QListWidgetItem(str, pListWgt);
		QVariant var;
		var.setValue(((ns[i].attribute())));
		pItem->setData(Qt::UserRole + 1, var);
	}

	ui->tabWidget->addTab(pListWgt, QString());
	ui->tabWidget->setTabText(ui->tabWidget->indexOf(pListWgt), "Page" + QString::number(ui->tabWidget->count()));
}

void xQueryJsonGui::updateJsonTab(const QString& fileName)
{
	//init
	ui->tabWidget->clear();
	mdoc.remove_children();
	//
	if (!fileName.isEmpty())
	{
		if (mdoc.load_file(fileName.toStdString().c_str()))
		{

			for (int i = 0; i < ui->listWidget->count(); i++)
			{
				auto xpathItem = ui->listWidget->item(i);

				if (xpathItem)
				{
					addJsonItems(mdoc, xpathItem->text());
				}
			}
		}
		ui->label->setText(fileName);
	}
}
