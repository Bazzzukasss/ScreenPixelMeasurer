#ifndef MEASURELINEITEM_H
#define MEASURELINEITEM_H

#include <QGraphicsItemGroup>
#include <QPen>
#include <QFont>
#include <QBrush>

class MeasureGraphicsItem : public QGraphicsItemGroup
{
public:
    MeasureGraphicsItem(QGraphicsItem* parent = nullptr);
    virtual ~MeasureGraphicsItem() = default;

    virtual void initialize() = 0;

    void setPenStyle(Qt::PenStyle style);
    void setFontSize(const float size);
    void setColor(const QColor& color);
    void setBgColor(const QColor& color);

protected:
    QPen m_pen;
    QFont m_font;
    QColor m_bgColor;

protected:
    virtual void applyPen(const QPen& pen) = 0;
    virtual void applyFont(const QFont& fnt) = 0;
    virtual void applyBgColor(const QColor& color) = 0;
};

class MeasureLineItem : public MeasureGraphicsItem
{
public:
    MeasureLineItem(QGraphicsItem* parent = nullptr);
    MeasureLineItem(bool isTextPresent, bool isTicksPresent, QGraphicsItem* parent = nullptr);

    void setLine(const QLineF& line);
    void initialize() override;

protected:
    void applyPen(const QPen& pen) override;
    void applyFont(const QFont& fnt) override;
    void applyBgColor(const QColor& color) override;

private:
    QGraphicsLineItem* m_line;
    std::array<QGraphicsLineItem*, 2> m_ticks;
    QGraphicsSimpleTextItem* m_text;
    bool m_isTextPresent;
    bool m_isTicksPresent;
};

class MeasureSimpleLineItem : public MeasureLineItem
{
public:
    MeasureSimpleLineItem(QGraphicsItem* parent = nullptr);
};

class MeasureRectItem : public MeasureGraphicsItem
{
public:
    MeasureRectItem(QGraphicsItem* parent = nullptr);

    void setRect(const QRectF& rect);
    void initialize() override;

protected:
    void applyPen(const QPen& pen) override;
    void applyFont(const QFont& fnt) override;
    void applyBgColor(const QColor& color) override;

private:
    QGraphicsRectItem* m_rect;
    QGraphicsSimpleTextItem* m_wText;
    QGraphicsSimpleTextItem* m_hText;
};

#endif // MEASURELINEITEM_H
