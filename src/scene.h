#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>

#include "data.h"

class Scene : public QGraphicsScene
{
public:
    Scene(QObject* parent = nullptr);

    void setRenderData(const RenderData& renderData);
    void setPalette(const Palette& palette);

private:
    QGraphicsPixmapItem* m_screenImageItem;
    QGraphicsLineItem* m_cursorHLineItem;
    QGraphicsLineItem* m_cursorVLineItem;
    QGraphicsLineItem* m_measureHLineItem;
    QGraphicsLineItem* m_measureVLineItem;
    QGraphicsRectItem* m_cursorRectangleItem;
    QGraphicsRectItem* m_fixedRectangleItem;
    std::array<QGraphicsLineItem*, 2> m_referenceVLinesItem;
    std::array<QGraphicsLineItem*, 2> m_referenceHLinesItem;

private:
    void initialize();
};

#endif // SCENE_H
