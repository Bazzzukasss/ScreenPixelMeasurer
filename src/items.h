#ifndef ITEMS_H
#define ITEMS_H

#include <QGraphicsItem>
#include <QColor>

class IGraphicsItem
{
public:
    IGraphicsItem(){};

    virtual void setPenStyle(Qt::PenStyle){};
    virtual void setPenColor(const QColor&){};
    virtual void setBgColor(const QColor& color){ m_bgColor = color; };
    bool isHovered() const { return m_isHovered; };

protected:
    QColor m_bgColor;
    bool m_isHovered{false};
};

class GraphicsTextItem : public IGraphicsItem, public QGraphicsTextItem
{
    enum class TextPosCorrection{
        None,
        ByX,
        ByY
    };

public:
    GraphicsTextItem(QGraphicsItem* parent = nullptr);

    void setText(const QString& value, const QPointF& point, TextPosCorrection posCorrection);
    void setData(const QLineF& line);
    void setData(const QRectF& rect, bool isHeightValue);
    void setPenColor(const QColor& color) override;
    void setBgColor(const QColor& color) override;

private:
    QString m_value;
    QPointF m_point;
    TextPosCorrection m_posCorrection;

private:
    void applyText();
};

class GraphicsLineItem : public QObject, public IGraphicsItem, public QGraphicsLineItem
{
    Q_OBJECT

public:
    GraphicsLineItem(QGraphicsItem* parent = nullptr);

    void setPenStyle(Qt::PenStyle style) override;
    void setPenColor(const QColor& color) override;
    virtual void setData(const QLineF& line);

signals:
    void positionChanged(const QPointF& pos);

protected:
    const float kBoundingGap{1.5};

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
};

class GraphicsMeasureLineItem : public GraphicsLineItem
{
    Q_OBJECT

public:
    GraphicsMeasureLineItem(QGraphicsItem* parent = nullptr);

    void setPenColor(const QColor& color) override;
    void setData(const QLineF& line) override;
    void setBgColor(const QColor& color) override;

protected:
    const qreal kTickSize{1.0};
    std::vector<QGraphicsLineItem*> m_ticks;
    GraphicsTextItem* m_label;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
};

class GraphicsFixedHorLineItem : public GraphicsLineItem
{
    Q_OBJECT

public:
    GraphicsFixedHorLineItem(QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    QPainterPath shape() const override;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
};

class GraphicsFixedVertLineItem : public GraphicsLineItem
{
    Q_OBJECT

public:
    GraphicsFixedVertLineItem(QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    QPainterPath shape() const override;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
};

class GraphicsMeasureRectItem : public IGraphicsItem, public QGraphicsRectItem
{
public:
    GraphicsMeasureRectItem(QGraphicsItem* parent = nullptr);

    void setData(const QRectF& rect);
    void setPenStyle(Qt::PenStyle style) override;
    void setPenColor(const QColor& color) override;
    void setBgColor(const QColor& color) override;

protected:
    std::vector<GraphicsTextItem*> m_labels;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
};

#endif // ITEMS_H
