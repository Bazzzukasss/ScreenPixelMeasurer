#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>

#include "data.h"
#include "items.h"

class Scene : public QGraphicsScene
{
    Q_OBJECT

public:
    Scene(QObject* parent = nullptr);

    void setRenderData(const RenderData& renderData);
    void setPalette(const Palette& palette);
    bool isHoveredItemPresent();

signals:
    void fixedRectanglChanged(const QRect& rect);

private:
    QGraphicsPixmapItem* m_screenImageItem;
    MeasureSimpleLineItem* m_cursorHLineItem;
    MeasureSimpleLineItem* m_cursorVLineItem;
    MeasureLineItem* m_measureHLineItem;
    MeasureLineItem* m_measureVLineItem;
    MeasureRectItem* m_cursorRectangleItem;
    MeasureRectItem* m_fixedRectangleItem;
    std::array<MeasureSimpleLineItem*, 4> m_fixedLinesItem;
    QVector<MeasureGraphicsItem*> m_items;
    QRect m_originalFixedRectangle;
    QRect m_currentFixedRectangle;

private:
    void initialize();
    void hideAll();
    void setVisibility(const RenderData& renderData);
    template<typename T>
    T* addMeasureGraphicsItem();

    QRectF toFloat(const QRect& rectangle);
    QLineF toFloat(const QLine& line);
};

#endif // SCENE_H
