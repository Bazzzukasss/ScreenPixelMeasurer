#include <QtMath>
#include <QFontMetrics>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include "items.h"
#include <QDebug>
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

void GraphicsLineItem::setData(const QLineF& line)
{
    setLine(line);
}

GraphicsMeasureLineItem::GraphicsMeasureLineItem(QGraphicsItem* parent)
    : GraphicsLineItem(parent)
{
    m_ticks.push_back(new GraphicsLineItem(this));
    m_ticks.push_back(new GraphicsLineItem(this));
    m_label = new GraphicsTextItem(this);
    m_label->setFlag(ItemIgnoresParentOpacity);
}

void GraphicsMeasureLineItem::setPenColor(const QColor& color)
{
    GraphicsLineItem::setPenColor(color);

    auto p = pen();
    p.setStyle(Qt::PenStyle::SolidLine);

    m_label->setPenColor(color);

    for (auto& tick : m_ticks)
    {
        tick->setPen(p);
    }
}

void GraphicsMeasureLineItem::setData(const QLineF& line)
{
    GraphicsLineItem::setData(line);

    m_label->setData(line);

    if (line.dy() == 0)
    {
        m_ticks[0]->setLine(line.p1().x(), line.p1().y() - kTickSize,
                            line.p1().x(), line.p1().y() + kTickSize);

        m_ticks[1]->setLine(line.p2().x(), line.p2().y() - kTickSize,
                            line.p2().x(), line.p2().y() + kTickSize);
    }
    else if(line.dx() == 0)
    {
        m_ticks[0]->setLine(line.p1().x() - kTickSize, line.p1().y(),
                            line.p1().x() + kTickSize, line.p1().y());

        m_ticks[1]->setLine(line.p2().x() - kTickSize, line.p2().y(),
                            line.p2().x() + kTickSize, line.p2().y());
    }
}

void GraphicsMeasureLineItem::setBgColor(const QColor& color)
{
    GraphicsLineItem::setBgColor(color);
    m_label->setBgColor(color);
}

QVariant GraphicsMeasureLineItem::itemChange(QGraphicsItem::GraphicsItemChange change,
                                             const QVariant& value)
{
    if (change == ItemVisibleHasChanged && scene())
    {
        auto visible = value.toBool();
        for (auto& tick : m_ticks)
        {
            tick->setVisible(visible);
        }

        if (m_label)
        {
            m_label->setVisible(visible);
        }
    }

    return QGraphicsItem::itemChange(change, value);
}

GraphicsFixedHorLineItem::GraphicsFixedHorLineItem(QGraphicsItem* parent)
    : GraphicsLineItem(parent)
{
    setCursor(Qt::SizeVerCursor);
}

QRectF GraphicsFixedHorLineItem::boundingRect() const
{
    return GraphicsLineItem::boundingRect().adjusted(0, -kBoundingGap, 0, kBoundingGap);
}

QVariant GraphicsFixedHorLineItem::itemChange(QGraphicsItem::GraphicsItemChange change,
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

GraphicsFixedVertLineItem::GraphicsFixedVertLineItem(QGraphicsItem* parent)
    : GraphicsLineItem(parent)
{
    setCursor(Qt::SizeHorCursor);
}

QRectF GraphicsFixedVertLineItem::boundingRect() const
{
    return GraphicsLineItem::boundingRect().adjusted(-kBoundingGap, 0, kBoundingGap, 0);
}

QVariant GraphicsFixedVertLineItem::itemChange(QGraphicsItem::GraphicsItemChange change,
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

void GraphicsTextItem::setText(const QString& value, const QPointF& point,
                               TextPosCorrection posCorrection)
{
    m_value = value;
    m_point = point;
    m_posCorrection = posCorrection;
    applyText();
}

void GraphicsTextItem::setData(const QLineF& line)
{
    setText(QString::number(line.length() + 1),
            {line.center().x(), line.center().y()}, GraphicsTextItem::TextPosCorrection::None);
}

void GraphicsTextItem::setData(const QRectF& rect, bool isHeightValue)
{
    if (!isHeightValue)
    {
        setText(QString::number(rect.height() + 1),
                {rect.right(), rect.center().y()}, GraphicsTextItem::TextPosCorrection::ByY);
    }
    else
    {
        setText(QString::number(rect.width() + 1),
                {rect.center().x(), rect.top()}, GraphicsTextItem::TextPosCorrection::ByX);
    }
}

void GraphicsTextItem::setPenColor(const QColor& color)
{
    setDefaultTextColor(color);
    applyText();
}

void GraphicsTextItem::setBgColor(const QColor& color)
{
    IGraphicsItem::setBgColor(color);
    applyText();
}

void GraphicsTextItem::applyText()
{
    QString format = QString("<div style='background:%1; color:%2;'>%3</div>");
    setHtml(format.arg(m_bgColor.name()).arg(defaultTextColor().name()).arg(m_value));

    auto view = scene()->views().empty() ? nullptr : scene()->views()[0];
    auto th = boundingRect().height() / (view ? view->transform().m22() : 1 );
    auto tw = boundingRect().width() / (view ? view->transform().m11() : 1 );
    double x, y;

    if (m_posCorrection == TextPosCorrection::None)
    {
        x = m_point.x() - tw / 2;
        y = m_point.y() - th / 2;
    }
    else
    {
        x = m_posCorrection == TextPosCorrection::ByX ? m_point.x() - tw / 2 : m_point.x();
        y = m_posCorrection == TextPosCorrection::ByX ? m_point.y() - th : m_point.y() - th / 2;
    }

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

GraphicsMeasureRectItem::GraphicsMeasureRectItem(QGraphicsItem* parent)
    : QGraphicsRectItem(parent)
{
    QPen p = pen();
    p.setCapStyle(Qt::PenCapStyle::FlatCap);
    p.setJoinStyle(Qt::PenJoinStyle::MiterJoin);
    setPen(p);

    m_labels.push_back(new GraphicsTextItem(this));
    m_labels.push_back(new GraphicsTextItem(this));

    for(auto label : m_labels)
    {
        label->setFlag(ItemIgnoresParentOpacity);
    }
}

void GraphicsMeasureRectItem::setData(const QRectF& rect)
{
    setRect(rect);

    m_labels[0]->setData(rect, true);
    m_labels[1]->setData(rect, false);
}

void GraphicsMeasureRectItem::setPenStyle(Qt::PenStyle style)
{
    auto p = pen();
    p.setStyle(style);
    setPen(p);
}

void GraphicsMeasureRectItem::setPenColor(const QColor& color)
{
    auto p = pen();
    p.setColor(color);
    setPen(p);

    for (auto label : m_labels)
    {
        label->setPenColor(color);
    }
}

void GraphicsMeasureRectItem::setBgColor(const QColor& color)
{
    for (auto& label : m_labels)
    {
        label->setBgColor(color);
    }
}

QVariant GraphicsMeasureRectItem::itemChange(QGraphicsItem::GraphicsItemChange change,
                                      const QVariant& value)
{
    if (change == ItemVisibleHasChanged && scene())
    {
        auto visible = value.toBool();
        for (auto& label : m_labels)
        {
            label->setVisible(visible);
        }
    }

    return QGraphicsItem::itemChange(change, value);
}
