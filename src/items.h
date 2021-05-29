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
    QGraphicsTextItem* m_text;
    bool m_isTextPresent;
    bool m_isTicksPresent;

private:
    void updateText();
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
    QGraphicsTextItem* m_wText;
    QGraphicsTextItem* m_hText;

private:
    void updateText();
};

#endif // MEASURELINEITEM_H
