#include <QtMath>
#include <QDebug>
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
    addToGroup(m_line);

    if (m_isTextPresent)
    {
        m_text = new QGraphicsSimpleTextItem(this);
        addToGroup(m_text);
    }

    if (m_isTicksPresent)
    {
        for (auto& tick : m_ticks)
        {
            tick = new QGraphicsLineItem(this);
            addToGroup(tick);
        }
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
            m_ticks[0]->setLine(line.p1().x() - 1, line.p1().y(),
                                line.p1().x() + 1, line.p1().y());

            m_ticks[1]->setLine(line.p2().x() - 1, line.p2().y(),
                                line.p2().x() + 1, line.p2().y());
        }
        else
        {
            m_ticks[0]->setLine(line.p1().x(), line.p1().y() - 1,
                                line.p1().x(), line.p1().y() + 1);

            m_ticks[1]->setLine(line.p2().x(), line.p2().y() - 1,
                                line.p2().x(), line.p2().y() + 1);

        }
    }

    if (m_isTextPresent)
    {
        m_text->setVisible(visible);
        m_text->setText(QString::number(line.length() + 1));
        m_text->setPos(line.center());
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
        m_text->setBrush(pen.color());
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
    if (m_isTextPresent)
    {
        //m_text->setBrush(color);
    }
}

MeasureRectItem::MeasureRectItem(QGraphicsItem* parent)
    : MeasureGraphicsItem(parent)
{
}

void MeasureRectItem::initialize()
{
    m_rect = new QGraphicsRectItem(this);
    m_hText = new QGraphicsSimpleTextItem(this);
    m_wText = new QGraphicsSimpleTextItem(this);

    addToGroup(m_rect);
    addToGroup(m_hText);
    addToGroup(m_wText);
}

void MeasureRectItem::setRect(const QRectF& rect)
{
    auto w = rect.width();
    auto h = rect.height();
    bool isVisible = (w != 0) && (h != 0);

    setVisible(isVisible);
    m_rect->setVisible(isVisible);
    m_hText->setVisible(isVisible);
    m_wText->setVisible(isVisible);

    if (isVisible)
    {
        m_rect->setRect(rect);

        m_hText->setText(QString::number(h + 1));
        m_wText->setText(QString::number(w + 1));

        m_hText->setPos({rect.right(), rect.center().y()});
        m_wText->setPos({rect.center().x(), rect.top()});
    }
}

void MeasureRectItem::applyFont(const QFont& fnt)
{
    m_hText->setFont(fnt);
    m_wText->setFont(fnt);
}

void MeasureRectItem::applyBgColor(const QColor& color)
{
    //m_hText->setBrush(color);
    //m_wText->setBrush(color);
}

void MeasureRectItem::applyPen(const QPen& pen)
{
    m_rect->setPen(pen);
    m_hText->setBrush(pen.color());
    m_wText->setBrush(pen.color());
}

MeasureSimpleLineItem::MeasureSimpleLineItem(QGraphicsItem* parent)
    : MeasureLineItem(false, false, parent)
{
}
