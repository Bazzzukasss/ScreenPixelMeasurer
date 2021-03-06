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
    void startDragging();
    bool isDragableItemSelected(const QPoint& pos) const;
    bool isDragableItemHovered(const QPoint& pos) const;

signals:
    void fixedRectanglChanged(const QRect& rect);

private:
    QGraphicsPixmapItem* m_screenImageItem;
    GraphicsLineItem* m_cursorHLineItem;
    GraphicsLineItem* m_cursorVLineItem;
    GraphicsLineItem* m_measureHLineItem;
    GraphicsLineItem* m_measureVLineItem;
    GraphicsMeasureRectItem* m_cursorRectangleItem;
    GraphicsMeasureRectItem* m_fixedRectangleItem;
    std::array<GraphicsLineItem*, 4> m_fixedLinesItem;

    QRect m_originalFixedRectangle;
    QRect m_currentFixedRectangle;
    bool m_isDragging{false};

private:
    void initialize();
    void hideAll();
    void setOpacity(float opacity);
    void setVisibility(const RenderData& renderData);
    void onFixedLinesChanged(int index, const QPointF &point);

    template<typename T>
    T* addGraphicsItem();

    QRectF toFloat(const QRect& rectangle);
    QLineF toFloat(const QLine& line);
    bool isRectangleValid(QRect& rect);
};

#endif // SCENE_H
