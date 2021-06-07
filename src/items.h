#ifndef ITEMS_H
#define ITEMS_H

#include <QGraphicsItemGroup>
#include <QPen>
#include <QFont>
#include <QBrush>

class MeasureGraphicsItem : public QObject, public QGraphicsItemGroup
{
    Q_OBJECT

public:
    MeasureGraphicsItem(QGraphicsItem* parent = nullptr);
    virtual ~MeasureGraphicsItem() = default;

    virtual void initialize() = 0;
    virtual void setItemsVisible(bool isVisible) = 0;
    virtual void setItemsFlags(GraphicsItemFlags flags) = 0;
    virtual void setItemsAcceptHoverEvents(bool isAccept) = 0;

    void setPenStyle(Qt::PenStyle style);
    void setFontSize(const float size);
    void setColor(const QColor& color);
    void setBgColor(const QColor& color);

signals:
    void positionChanged(const QPointF& pos);

protected:
    virtual void applyPen(const QPen& pen) = 0;
    virtual void applyFont(const QFont& fnt) = 0;
    virtual void applyBgColor(const QColor& color) = 0;
    virtual void changePosition(const QPointF& pos);
    void setTextValue(
            QGraphicsTextItem* item,
            float value,
            const QPointF& point,
            bool isHeightValue);

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

protected:
    QPen m_pen;
    QFont m_font;
    QColor m_bgColor;
    QPointF m_anchorPoint;
};

class MeasureLineItem : public MeasureGraphicsItem
{
public:
    MeasureLineItem(QGraphicsItem* parent = nullptr);
    MeasureLineItem(bool isTextPresent, bool isTicksPresent, QGraphicsItem* parent = nullptr);

    void setLine(const QLineF& line);
    void initialize() override;
    void setItemsVisible(bool isVisible) override;
    void setItemsFlags(GraphicsItemFlags flags) override;
    void setItemsAcceptHoverEvents(bool isAccept) override;

protected:
    void applyPen(const QPen& pen) override;
    void applyFont(const QFont& fnt) override;
    void applyBgColor(const QColor& color) override;
    QRectF boundingRect() const override;    

protected:
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

class MeasureSimpleHorLineItem : public MeasureSimpleLineItem
{
public:
    MeasureSimpleHorLineItem(QGraphicsItem* parent = nullptr);

protected:
    void changePosition(const QPointF& pos) override;
    QRectF boundingRect() const override;
};

class MeasureSimpleVertLineItem : public MeasureSimpleLineItem
{
public:
    MeasureSimpleVertLineItem(QGraphicsItem* parent = nullptr);

protected:
    void changePosition(const QPointF& pos) override;
    QRectF boundingRect() const override;
};

class MeasureRectItem : public MeasureGraphicsItem
{
public:
    MeasureRectItem(QGraphicsItem* parent = nullptr);

    void setRect(const QRectF& rect);
    void initialize() override;
    void setItemsVisible(bool isVisible) override;
    void setItemsFlags(GraphicsItemFlags flags) override;
    void setItemsAcceptHoverEvents(bool isAccept) override;

protected:
    void applyPen(const QPen& pen) override;
    void applyFont(const QFont& fnt) override;
    void applyBgColor(const QColor& color) override;
    QRectF boundingRect() const override;

protected:
    QGraphicsRectItem* m_rect;
    QGraphicsTextItem* m_wText;
    QGraphicsTextItem* m_hText;

private:
    void updateText();
};

#endif // ITEMS_H
