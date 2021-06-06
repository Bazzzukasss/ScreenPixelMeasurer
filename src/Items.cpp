#include <QtMath>
#include <QFontMetrics>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include "items.h"

MeasureGraphicsItem::MeasureGraphicsItem(QGraphicsItem* parent)
    : QGraphicsItemGroup(parent)
{
    m_pen.setCapStyle(Qt::PenCapStyle::FlatCap);
    m_pen.setJoinStyle(Qt::PenJoinStyle::MiterJoin);
}

void MeasureGraphicsItem::setPenStyle(Qt::PenStyle style)
{
    m_pen.setStyle(style);
    applyPen(m_pen);
}

void MeasureGraphicsItem::setFontSize(const float size)
{
    m_font.setPointSizeF(size);
    applyFont(m_font);
}

void MeasureGraphicsItem::setColor(const QColor& color)
{
    m_pen.setColor(color);
    applyPen(m_pen);
}

void MeasureGraphicsItem::setBgColor(const QColor& color)
{
    m_bgColor = color;
    applyBgColor(m_bgColor);
}

void MeasureGraphicsItem::changePosition(const QPointF &pos)
{
    setPos(mapToScene(pos - m_anchorPoint));
}

void MeasureGraphicsItem:: setTextValue(
        QGraphicsTextItem* item,
        float value,
        const QPointF& point,
        bool isHeightValue)
{
    QString format = QString("<div style='background:%1; color:%2;'>%3</div>");
    item->setHtml(format.arg(m_bgColor.name()).arg(m_pen.color().name()).arg(value));

    auto view = item->scene()->views().empty() ? nullptr : item->scene()->views()[0];
    auto th = item->boundingRect().height() / (view ? view->transform().m22() : 1 );
    auto tw = item->boundingRect().width() / (view ? view->transform().m11() : 1 );
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
    item->setPos({x, y});
}

void MeasureGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    m_anchorPoint = mapToScene(event->pos());
}

void MeasureGraphicsItem::mouseReleaseEvent(QGraphicsSceneMouseEvent*)
{
    setPos(0, 0);
}

void MeasureGraphicsItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (flags() & QGraphicsItem::ItemIsMovable)
    {
        changePosition(event->pos());        
        emit positionChanged(pos());
        setPos(0, 0);
    }
}

MeasureLineItem::MeasureLineItem(QGraphicsItem* parent)
    : MeasureLineItem(true, true, parent)
{
}

MeasureLineItem::MeasureLineItem(bool isTextPresent, bool isTicksPresent, QGraphicsItem* parent)
    : MeasureGraphicsItem(parent)
    , m_isTextPresent(isTextPresent)
    , m_isTicksPresent(isTicksPresent)
{
}

void MeasureLineItem::initialize()
{
    m_line = new QGraphicsLineItem(this);
    m_line->setZValue(0);
    m_line->setOpacity(0.5);

    addToGroup(m_line);

    if (m_isTicksPresent)
    {
        for (auto& tick : m_ticks)
        {
            tick = new QGraphicsLineItem(this);
            tick->setZValue(0);
            tick->setOpacity(0.5);
            addToGroup(tick);
        }
    }

    if (m_isTextPresent)
    {
        m_text = new QGraphicsTextItem(this);
        m_text->setFlag(GraphicsItemFlag::ItemIgnoresTransformations);
        m_text->setZValue(1);
        m_text->setAcceptHoverEvents(false);
        addToGroup(m_text);
    }
}

void MeasureLineItem::setItemsVisible(bool isVisible)
{
    setVisible(isVisible);
    m_line->setVisible(isVisible);
    if (m_isTicksPresent)
    {
        for (auto& tick : m_ticks)
        {
            tick->setVisible(isVisible);
        }
    }

    if (m_isTextPresent)
    {
        m_text->setVisible(isVisible);
    }
}

void MeasureLineItem::setItemsFlags(QGraphicsItem::GraphicsItemFlags flags)
{
    setFlags(flags);
    m_line->setFlags(flags);
}

void MeasureLineItem::setItemsAcceptHoverEvents(bool isAccept)
{
    m_line->setAcceptHoverEvents(isAccept);
}

void MeasureLineItem::setLine(const QLineF& line)
{
    auto visible = (line.dx() > 0) || (line.dy() > 0);

    m_line->setVisible(visible);
    m_line->setLine(line);

    if (m_isTicksPresent)
    {
        for (auto tick : m_ticks)
        {
            tick->setVisible(visible);
        }

        if (line.dy() != 0)
        {
            m_ticks[0]->setLine(line.p1().x() - 2, line.p1().y(),
                                line.p1().x() + 2, line.p1().y());

            m_ticks[1]->setLine(line.p2().x() - 2, line.p2().y(),
                                line.p2().x() + 2, line.p2().y());
        }
        else
        {
            m_ticks[0]->setLine(line.p1().x(), line.p1().y() - 2,
                                line.p1().x(), line.p1().y() + 2);

            m_ticks[1]->setLine(line.p2().x(), line.p2().y() - 2,
                                line.p2().x(), line.p2().y() + 2);
        }
    }

    if (m_isTextPresent)
    {
        m_text->setVisible(visible);
        updateText();
    }
}

void MeasureLineItem::applyPen(const QPen& pen)
{
    m_line->setPen(pen);

    if (m_isTicksPresent)
    {
        for (auto tick : m_ticks)
        {
            auto tickPen = pen;
            tickPen.setStyle(Qt::PenStyle::SolidLine);
            tick->setPen(tickPen);
        }
    }

    if (m_isTextPresent)
    {
        updateText();
    }
}

void MeasureLineItem::applyFont(const QFont& fnt)
{
    if (m_isTextPresent)
    {
        m_text->setFont(fnt);
    }
}

void MeasureLineItem::applyBgColor(const QColor& )
{
    updateText();
}

QRectF MeasureLineItem::boundingRect() const
{
    return m_line->boundingRect();
}

void MeasureLineItem::updateText()
{
    if (m_isTextPresent)
    {
        auto line = m_line->line();
        setTextValue(m_text, line.length() + 1, {line.center().x(), line.center().y()}, line.dy() != 0);
    }
}

MeasureRectItem::MeasureRectItem(QGraphicsItem* parent)
    : MeasureGraphicsItem(parent)
{
}

void MeasureRectItem::initialize()
{
    m_rect = new QGraphicsRectItem(this);
    m_hText = new QGraphicsTextItem(this);
    m_wText = new QGraphicsTextItem(this);

    m_hText->setFlag(GraphicsItemFlag::ItemIgnoresTransformations);
    m_wText->setFlag(GraphicsItemFlag::ItemIgnoresTransformations);

    m_hText->setZValue(1);
    m_wText->setZValue(1);
    m_rect->setZValue(0);
    m_rect->setOpacity(0.8);
    m_hText->setAcceptHoverEvents(false);
    m_wText->setAcceptHoverEvents(false);

    addToGroup(m_rect);
    addToGroup(m_hText);
    addToGroup(m_wText);
}

void MeasureRectItem::setItemsVisible(bool isVisible)
{
    setVisible(isVisible);
    m_rect->setVisible(isVisible);
    m_hText->setVisible(isVisible);
    m_wText->setVisible(isVisible);
}

void MeasureRectItem::setItemsFlags(QGraphicsItem::GraphicsItemFlags flags)
{
    setFlags(flags);
    m_rect->setFlags(flags);
}

void MeasureRectItem::setItemsAcceptHoverEvents(bool isAccept)
{
    m_rect->setAcceptHoverEvents(isAccept);
}

void MeasureRectItem::setRect(const QRectF& rect)
{
    m_rect->setRect(rect);
    updateText();
}

void MeasureRectItem::applyFont(const QFont& fnt)
{
    m_hText->setFont(fnt);
    m_wText->setFont(fnt);
    updateText();
}

void MeasureRectItem::applyBgColor(const QColor& )
{
    updateText();
}

QRectF MeasureRectItem::boundingRect() const
{
    return m_rect->rect();
}

void MeasureRectItem::updateText()
{
    auto rect = m_rect->rect();
    setTextValue(m_wText, rect.width() + 1, {rect.center().x(), rect.top()}, true);
    setTextValue(m_hText, rect.height() + 1, {rect.right(), rect.center().y()}, false);
}

void MeasureRectItem::applyPen(const QPen& pen)
{
    m_rect->setPen(pen);
    updateText();
}

MeasureSimpleLineItem::MeasureSimpleLineItem(QGraphicsItem* parent)
    : MeasureLineItem(false, false, parent)
{
}

MeasureSimpleHorLineItem::MeasureSimpleHorLineItem(QGraphicsItem* parent)
    : MeasureSimpleLineItem(parent)
{
    setCursor(Qt::SizeVerCursor);
}

void MeasureSimpleHorLineItem::changePosition(const QPointF& pos)
{
    auto x = this->pos().x();
    setPos(mapToScene(x, pos.y() - m_anchorPoint.y()));
}

QRectF MeasureSimpleHorLineItem::boundingRect() const
{
    return m_line->boundingRect().adjusted(0, -1, 0, 1);
}

MeasureSimpleVertLineItem::MeasureSimpleVertLineItem(QGraphicsItem *parent)
    : MeasureSimpleLineItem(parent)
{
    setCursor(Qt::SizeHorCursor);
}

void MeasureSimpleVertLineItem::changePosition(const QPointF& pos)
{
    auto y = this->pos().y();
    setPos(mapToScene(pos.x() - m_anchorPoint.x(), y));
}

QRectF MeasureSimpleVertLineItem::boundingRect() const
{
    return m_line->boundingRect().adjusted(-1, 0, 1, 0);
}
