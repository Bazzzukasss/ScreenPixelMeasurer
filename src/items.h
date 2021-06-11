#ifndef ITEMS_H
#define ITEMS_H

#include <QGraphicsItem>
#include <QColor>


class GraphicsLineItem : public QObject, public QGraphicsLineItem
{
    Q_OBJECT

public:
    GraphicsLineItem(QGraphicsItem* parent = nullptr);

    void setPenStyle(Qt::PenStyle style);
    void setPenColor(const QColor& color);

signals:
    void positionChanged(const QPointF& pos);

protected:
    const float kBoundingGap{2.0};
};

class GraphicsHorLineItemExt : public GraphicsLineItem
{
    Q_OBJECT

public:
    GraphicsHorLineItemExt(QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};

class GraphicsVertLineItemExt : public GraphicsLineItem
{
    Q_OBJECT

public:
    GraphicsVertLineItemExt(QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
};

class GraphicsTextItem : public QGraphicsTextItem
{
public:
    GraphicsTextItem(QGraphicsItem* parent = nullptr);

    void setText(const QString &value, const QPointF& point, bool isHeightValue);
    void setBgColor(const QColor& color);
    void setData(const QLineF& line);
    void setData(const QRectF& rect, bool isHeightValue);

private:
    QColor m_bgColor;
};

class GraphicsRectItem : public QGraphicsRectItem
{
public:
    GraphicsRectItem(QGraphicsItem* parent = nullptr);

    void setPenColor(const QColor& color);
};

#endif // ITEMS_H
