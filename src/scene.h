#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>

#include "data.h"
#include "items.h"

class Scene : public QGraphicsScene
{
public:
    Scene(QObject* parent = nullptr);

    void setRenderData(const RenderData& renderData);
    void setPalette(const Palette& palette);

private:
    QGraphicsPixmapItem* m_screenImageItem;
    MeasureSimpleLineItem* m_cursorHLineItem;
    MeasureSimpleLineItem* m_cursorVLineItem;
    MeasureLineItem* m_measureHLineItem;
    MeasureLineItem* m_measureVLineItem;
    MeasureRectItem* m_cursorRectangleItem;
    MeasureRectItem* m_fixedRectangleItem;
    std::array<MeasureSimpleLineItem*, 2> m_referenceVLinesItem;
    std::array<MeasureSimpleLineItem*, 2> m_referenceHLinesItem;
    std::array<MeasureSimpleLineItem*, 4> m_fixedLinesItem;

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
