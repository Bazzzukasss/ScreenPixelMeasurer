#include <QtMath>
#include <QFontMetrics>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include "items.h"


GraphicsLineItem::GraphicsLineItem(QGraphicsItem* parent)
    : QGraphicsLineItem(parent)
{
}

void GraphicsLineItem::setPenStyle(Qt::PenStyle style)
{
    auto p = pen();
    p.setStyle(style);
    setPen(p);
}

void GraphicsLineItem::setPenColor(const QColor& color)
{
    auto p = pen();
    p.setColor(color);
    setPen(p);
}

GraphicsHorLineItemExt::GraphicsHorLineItemExt(QGraphicsItem* parent)
    : GraphicsLineItem(parent)
{
    setCursor(Qt::SizeVerCursor);
}

QVariant GraphicsHorLineItemExt::itemChange(QGraphicsItem::GraphicsItemChange change,
                                            const QVariant& value)
{
    if (change == ItemPositionChange && scene())
    {
        auto newPos = QPointF{pos().x(), value.toPointF().y()};
        emit positionChanged(newPos);
        return QPointF{0, 0};
    }

    return QGraphicsItem::itemChange(change, value);
}

GraphicsVertLineItemExt::GraphicsVertLineItemExt(QGraphicsItem* parent)
    : GraphicsLineItem(parent)
{
    setCursor(Qt::SizeHorCursor);
}

QVariant GraphicsVertLineItemExt::itemChange(QGraphicsItem::GraphicsItemChange change,
                                             const QVariant& value)
{
    if (change == ItemPositionChange && scene())
    {
        auto newPos = QPointF{value.toPointF().x(), pos().y()};
        emit positionChanged(newPos);
        return QPointF{0, 0};
    }

    return QGraphicsItem::itemChange(change, value);
}

GraphicsTextItem::GraphicsTextItem(QGraphicsItem* parent)
    : QGraphicsTextItem(parent)
{
    setFlag(GraphicsItemFlag::ItemIgnoresTransformations);
}

void GraphicsTextItem::setText(const QString& value, const QPointF& point, bool isHeightValue)
{
    QString format = QString("<div style='background:%1; color:%2;'>%3</div>");
    setHtml(format.arg(m_bgColor.name()).arg(defaultTextColor().name()).arg(value));

    auto view = scene()->views().empty() ? nullptr : scene()->views()[0];
    auto th = boundingRect().height() / (view ? view->transform().m22() : 1 );
    auto tw = boundingRect().width() / (view ? view->transform().m11() : 1 );
    auto x = isHeightValue ? point.x() - tw / 2 : point.x();
    auto y = isHeightValue ? point.y() - th : point.y() - th / 2;

    if (view)
    {
        auto tl = view->mapToScene(view->viewport()->rect().topLeft());
        auto br = view->mapToScene(view->viewport()->rect().bottomRight());

        if (x < tl.x()) x = tl.x();
        if (x > br.x() - tw) x = br.x() - tw;
        if (y < tl.y()) y = tl.y();
        if (y > br.y() - th) y = br.y() - th;
    }
    setPos({x, y});
}

void GraphicsTextItem::setBgColor(const QColor& color)
{
    m_bgColor = color;
}

void GraphicsTextItem::setData(const QLineF& line)
{
    setText(QString::number(line.length() + 1),
            {line.center().x(), line.center().y()}, line.dy() != 0);
}

void GraphicsTextItem::setData(const QRectF& rect, bool isHeightValue)
{
    if (!isHeightValue)
    {
        setText(QString::number(rect.height() + 1),
                {rect.right(), rect.center().y()}, isHeightValue);
    }
    else
    {
        setText(QString::number(rect.width() + 1),
                {rect.center().x(), rect.top()}, isHeightValue);
    }
}

GraphicsRectItem::GraphicsRectItem(QGraphicsItem* parent)
    : QGraphicsRectItem(parent)
{
    QPen p = pen();
    p.setCapStyle(Qt::PenCapStyle::FlatCap);
    p.setJoinStyle(Qt::PenJoinStyle::MiterJoin);
    setPen(p);
}

void GraphicsRectItem::setPenColor(const QColor& color)
{
    auto p = pen();
    p.setColor(color);
    setPen(p);
}
