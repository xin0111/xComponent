#include "qjsonmodel.h"
#include <QFile>
#include <QDebug>
#include <QFont>
#include "QHeaderView"

QJsonTreeItem::QJsonTreeItem(QJsonTreeItem *parent)
{
    mParent = parent;
}

QJsonTreeItem::~QJsonTreeItem()
{
    qDeleteAll(mChilds);
}

void QJsonTreeItem::appendChild(QJsonTreeItem *item)
{
    mChilds.append(item);
}

QJsonTreeItem *QJsonTreeItem::child(int row)
{
    return mChilds.value(row);
}

QJsonTreeItem *QJsonTreeItem::parent()
{
    return mParent;
}

int QJsonTreeItem::childCount() const
{
    return mChilds.count();
}

int QJsonTreeItem::row() const
{
    if (mParent)
        return mParent->mChilds.indexOf(const_cast<QJsonTreeItem*>(this));

    return 0;
}

void QJsonTreeItem::setKey(const QString &key)
{
    mKey = key;
}

void QJsonTreeItem::setValue(const QString &value)
{
    mValue = value;
}

void QJsonTreeItem::setType(const QJsonValue::Type &type)
{
    mType = type;
}

QString QJsonTreeItem::key() const
{
    return mKey;
}

QString QJsonTreeItem::value() const
{
	return mValue;
}

QJsonValue::Type QJsonTreeItem::type() const
{
    return mType;
}

QJsonTreeItem* QJsonTreeItem::load(const QJsonValue& value, QJsonTreeItem* parent)
{
    QJsonTreeItem * rootItem = new QJsonTreeItem(parent);
    rootItem->setKey("root");

    if ( value.isObject())
    {

        //Get all QJsonValue childs
        for (QString key : value.toObject().keys()){
            QJsonValue v = value.toObject().value(key);
            QJsonTreeItem * child = load(v,rootItem);
            child->setKey(key);
            child->setType(v.type());
            rootItem->appendChild(child);

        }

    }

    else if ( value.isArray())
    {
        //Get all QJsonValue childs
        int index = 0;
        for (QJsonValue v : value.toArray()){

            QJsonTreeItem * child = load(v,rootItem);
            child->setKey(QString::number(index));			
			child->setType(v.type());
			rootItem->appendChild(child);
            ++index;
        }
    }
    else
    {
        rootItem->setValue(value.toVariant().toString());
        rootItem->setType(value.type());
    }

    return rootItem;
}

QJsonModel::QJsonModel(QObject *parent)
: QAbstractItemModel(parent)
    , mRootItem{new QJsonTreeItem}
{
    mHeaders.append("key");
    mHeaders.append("value");
}

QJsonModel::~QJsonModel()
{
    delete mRootItem;
}

bool QJsonModel::loadJson(const QString &json)
{
	auto const& jdoc = QJsonDocument::fromJson(json.toStdString().c_str());

    if (!jdoc.isNull())
    {
        beginResetModel();
        delete mRootItem;
        if (jdoc.isArray()) {
            mRootItem = QJsonTreeItem::load(QJsonValue(jdoc.array()));
            mRootItem->setType(QJsonValue::Array);

        } else {
            mRootItem = QJsonTreeItem::load(QJsonValue(jdoc.object()));
            mRootItem->setType(QJsonValue::Object);
        }
        endResetModel();
        return true;
    }

    qDebug()<<Q_FUNC_INFO<<"cannot load json";
    return false;
}


QVariant QJsonModel::data(const QModelIndex &index, int role) const
{

    if (!index.isValid())
        return QVariant();


    QJsonTreeItem *item = static_cast<QJsonTreeItem*>(index.internalPointer());


    if (role == Qt::DisplayRole) {

        if (index.column() == 0)
            return QString("%1").arg(item->key());

        if (index.column() == 1)
            return QString("%1").arg(item->value());
    } else if (Qt::EditRole == role) {
        if (index.column() == 1) {
            return QString("%1").arg(item->value());
        }
    }



    return QVariant();

}

bool QJsonModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int col = index.column();
    if (Qt::EditRole == role) {
        if (col == 1) {
            QJsonTreeItem *item = static_cast<QJsonTreeItem*>(index.internalPointer());
                item->setValue(value.toString());
				QVector<int> roles;
				roles.push_back(Qt::EditRole);
				emit dataChanged(index, index, roles);
                return true;
        }
    }

    return false;
}

QVariant QJsonModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {

        return mHeaders.value(section);
    }
    else
        return QVariant();
}

QModelIndex QJsonModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    QJsonTreeItem *parentItem;

    if (!parent.isValid())
        parentItem = mRootItem;
    else
        parentItem = static_cast<QJsonTreeItem*>(parent.internalPointer());

    QJsonTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex QJsonModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    QJsonTreeItem *childItem = static_cast<QJsonTreeItem*>(index.internalPointer());
    QJsonTreeItem *parentItem = childItem->parent();

    if (parentItem == mRootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int QJsonModel::rowCount(const QModelIndex &parent) const
{
    QJsonTreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = mRootItem;
    else
        parentItem = static_cast<QJsonTreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int QJsonModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 2;
}

Qt::ItemFlags QJsonModel::flags(const QModelIndex &index) const
{
    int col   = index.column();
    auto item = static_cast<QJsonTreeItem*>(index.internalPointer());

    auto isArray = QJsonValue::Array == item->type();
    auto isObject = QJsonValue::Object == item->type();

    if ((col == 1) && !(isArray || isObject)) {
        return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
    } else {
        return QAbstractItemModel::flags(index);
    }
}

QJsonDocument QJsonModel::json() const
{

    auto v = genJson(mRootItem);
    QJsonDocument doc;

    if (v.isObject()) {
        doc = QJsonDocument(v.toObject());
    } else {
        doc = QJsonDocument(v.toArray());
    }

    return doc;
}

QString QJsonModel::sjson() const
{
	QJsonDocument dom = json();
	//Ñ¹Ëõ
	return QString::fromStdString(dom.toJson(QJsonDocument::Compact).data());
}

QJsonValue  QJsonModel::genJson(QJsonTreeItem * item) const
{
    auto type   = item->type();
    int  nchild = item->childCount();

    if (QJsonValue::Object == type) {
        QJsonObject jo;
        for (int i = 0; i < nchild; ++i) {
            auto ch = item->child(i);
            auto key = ch->key();
            jo.insert(key, genJson(ch));
        }
        return  jo;
    } else if (QJsonValue::Array == type) {
        QJsonArray arr;
        for (int i = 0; i < nchild; ++i) {
            auto ch = item->child(i);
            arr.append(genJson(ch));
        }
        return arr;
    }else if (QJsonValue::Double == type) {
		QJsonValue va(item->value().toDouble());
        return va;
	} 
	else {
        QJsonValue va(item->value());
        return va;
    }

}
////////////////////////////////////////////////////////////////////////////
// filter for searches
class RuleSearchFilter : public QSortFilterProxyModel
{
public:
	RuleSearchFilter(QObject* o)
		: QSortFilterProxyModel(o)
	{
	}
	
protected:

	void allParents(const QAbstractItemModel* m, const QModelIndex& idx, bool &bDisplay) const
	{
		if (!idx.isValid()) return;

		QString strKey = m->data(idx).toString();
		if (strKey.contains(this->filterRegExp()))
		{
			bDisplay = true;
		}
		if (bDisplay) return;
		allParents(m, idx.parent(), bDisplay);
	}
	void allChildren(const QAbstractItemModel* m, const QModelIndex& idx, bool &bDisplay) const
	{
		QString strKey = m->data(idx).toString();
		if (strKey.contains(this->filterRegExp()))
		{
			bDisplay = true;		
		}
		if (bDisplay) return;
		if (m->hasChildren(idx)) 
		{//children
			int num = m->rowCount(idx);
			for (int i = 0; i < num; i++) {
				QModelIndex keyidx = m->index(i, 0, idx);
				allChildren(m, keyidx, bDisplay);
			}
		}
		else
		{//parents
			allParents(m, idx.parent(), bDisplay);
		}
	}

	 bool filterAcceptsRow(int row, const QModelIndex& p) const override
	{
		const QAbstractItemModel* m = this->sourceModel();
		QModelIndex idx = m->index(row, 0, p);
		bool bDisplay = false;
		allChildren(m, idx,bDisplay);
		return bDisplay;
	}

};

QJsonView::QJsonView(QWidget* p)
: QTreeView(p)
{
	this->mJsonModel = new QJsonModel(this);
	this->mSearchFilter = new RuleSearchFilter(this);
	this->mSearchFilter->setSourceModel(this->mJsonModel);
	this->mSearchFilter->setFilterCaseSensitivity(Qt::CaseInsensitive);
	this->mSearchFilter->setDynamicSortFilter(false);
	
	this->setModel(this->mSearchFilter);

	mbEpandState = true;
	connect(this, SIGNAL(expanded(const QModelIndex &)
		), this, SLOT(onExpandEx(const QModelIndex &)));
}


QJsonView::~QJsonView()
{
	delete mJsonModel;
	delete mSearchFilter;
}

void QJsonView::setSearchFilter(const QString&s)
{
	this->mSearchFilter->setFilterFixedString(s);
}

void QJsonView::expandAllEx()
{
	mbEpandState = false;
	__super::expandAll();
	this->resizeColumnToContents(0);
	mbEpandState = true;
}

void QJsonView::onExpandEx(const QModelIndex &index)
{
	if (mbEpandState){
		this->resizeColumnToContents(0);
	}
}


