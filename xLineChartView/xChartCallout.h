#ifndef CALLOUT_H
#define CALLOUT_H

#include <QtWidgets/QGraphicsItem>
#include <QtGui/QFont>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
QT_END_NAMESPACE

class xChartCallout : public QGraphicsItem
{
public:
    xChartCallout(QGraphicsItem * parent = 0);

    void setText(const QString &text);
    void setAnchor(QPointF point);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private:
    QString m_text;
    QRectF m_textRect;
    QRectF m_rect;
    QPointF m_anchor;
    QFont m_font;
};

#endif // CALLOUT_H
