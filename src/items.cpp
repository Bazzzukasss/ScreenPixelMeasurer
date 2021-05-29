#include <QtMath>
#include <QFontMetrics>
#include <QGraphicsScene>
#include <QGraphicsView>

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
        addToGroup(m_text);
    }
}

void MeasureLineItem::setLine(const QLineF& line)
{
    auto visible = (line.dx() > 0) || (line.dy() > 0);

    setVisible(visible);

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

void MeasureLineItem::applyBgColor(const QColor& color)
{
    updateText();
}

void MeasureLineItem::updateText()
{
    auto line = m_line->line();
    QString format = QString("<div style='background:%1; color:%2;'>%3</div>");
    m_text->setHtml(format.arg(m_bgColor.name()).arg(m_pen.color().name()).arg(line.length() + 1));

    auto view = scene()->views().empty() ? nullptr : scene()->views()[0];
    auto textWidth = m_text->boundingRect().width() / (view ? view->transform().m11() : 1 );
    auto textHeight = m_text->boundingRect().height() / (view ? view->transform().m22() : 1 );

    m_text->setPos({line.center().x() - textWidth / 2, line.center().y() - textHeight / 2});
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

    addToGroup(m_rect);
    addToGroup(m_hText);
    addToGroup(m_wText);
}

void MeasureRectItem::setRect(const QRectF& rect)
{
    bool isVisible = (rect.height() != 0) && (rect.width() != 0);

    setVisible(isVisible);
    m_rect->setVisible(isVisible);
    m_hText->setVisible(isVisible);
    m_wText->setVisible(isVisible);

    if (isVisible)
    {
        m_rect->setRect(rect);        
        updateText();
    }
}

void MeasureRectItem::applyFont(const QFont& fnt)
{
    m_hText->setFont(fnt);
    m_wText->setFont(fnt);
    updateText();
}

void MeasureRectItem::applyBgColor(const QColor& color)
{
    updateText();
}

void MeasureRectItem::updateText()
{
    auto rect = m_rect->rect();
    auto w = rect.width();
    auto h = rect.height();
    QFontMetrics fm(m_hText->font());

    QString format = QString("<div style='background:%1; color:%2;'>%3</div>");
    m_wText->setHtml(format.arg(m_bgColor.name()).arg(m_pen.color().name()).arg(w + 1));
    m_hText->setHtml(format.arg(m_bgColor.name()).arg(m_pen.color().name()).arg(h + 1));

    auto view = scene()->views().empty() ? nullptr : scene()->views()[0];
    auto wTextWidth = m_wText->boundingRect().width() / (view ? view->transform().m11() : 1 );
    auto hTextWidth = m_hText->boundingRect().width() / (view ? view->transform().m11() : 1 );
    auto textHeight = m_hText->boundingRect().height() / (view ? view->transform().m22() : 1 );

    m_wText->setPos({rect.center().x() - wTextWidth / 2, rect.top() - textHeight});
    m_hText->setPos({rect.right(), rect.center().y() - textHeight / 2});
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