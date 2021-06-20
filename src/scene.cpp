#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QGraphicsPixmapItem>

#include "scene.h"

Scene::Scene(QObject* parent)
    : QGraphicsScene(parent)
{
    initialize();
}

void Scene::setRenderData(const RenderData& renderData)
{
    if (renderData.screenImage.isNull())
        return;

    setVisibility(renderData);

    m_screenImageItem->setPixmap(renderData.screenImage);

    m_cursorHLineItem->setData(toFloat(renderData.cursorHLine));
    m_cursorVLineItem->setData(toFloat(renderData.cursorVLine));
    m_cursorRectangleItem->setData(toFloat(renderData.cursorRectangle));

    m_fixedRectangleItem->setData(toFloat(renderData.fixedRectangle));
    m_measureHLineItem->setData(toFloat(renderData.measureHLine));
    m_measureVLineItem->setData(toFloat(renderData.measureVLine));

    int i{0};
    for (auto& fixedLineItem : m_fixedLinesItem)
    {
        fixedLineItem->setLine(toFloat(renderData.fixedLines[i++]));
    }

    m_currentFixedRectangle = renderData.fixedRectangle;

    setSceneRect(itemsBoundingRect());
}

void Scene::setPalette(const Palette& palette)
{
    m_cursorHLineItem->setPenColor(palette.cursorLines);
    m_cursorVLineItem->setPenColor(palette.cursorLines);

    m_measureHLineItem->setPenColor(palette.measurerLines);
    m_measureVLineItem->setPenColor(palette.measurerLines);

    m_cursorRectangleItem->setPenColor(palette.cursorRectangle);
    m_fixedRectangleItem->setPenColor(palette.fixedRectangle);

    for (auto& fixedLineItem : m_fixedLinesItem)
    {
        fixedLineItem->setPenColor(palette.fixedLines);
    }

    for (auto item : items())
    {
        if (auto it = dynamic_cast<GraphicsTextItem*>(item))
        {
            it->setBgColor(palette.background);
        }
    }
}

void Scene::startDragging()
{
    m_originalFixedRectangle = m_currentFixedRectangle;
}

bool Scene::isMovableItemSelected(const QPoint& pos) const
{
    QPointF fpos {pos.x() + 0.5, pos.y() + 0.5};

    for (auto item : items(fpos))
    {
        if (item->flags() & QGraphicsItem::ItemIsMovable)
        {
            return true;
        }
    }

    return false;
}

void Scene::initialize()
{
    m_screenImageItem = addPixmap({});

    m_cursorHLineItem = addGraphicsItem<GraphicsLineItem>();
    m_cursorVLineItem = addGraphicsItem<GraphicsLineItem>();
    m_measureHLineItem = addGraphicsItem<GraphicsMeasureLineItem>();
    m_measureVLineItem = addGraphicsItem<GraphicsMeasureLineItem>();

    m_measureHLineItem->setPenStyle(Qt::PenStyle::DotLine);
    m_measureVLineItem->setPenStyle(Qt::PenStyle::DotLine);

    int i{0};
    for (auto& fixedLineItem : m_fixedLinesItem)
    {
        if (i < 2)
        {
            fixedLineItem = addGraphicsItem<GraphicsFixedHorLineItem>();
        }
        else
        {
            fixedLineItem = addGraphicsItem<GraphicsFixedVertLineItem>();
        }

        fixedLineItem->setPenStyle(Qt::PenStyle::DashLine);
        fixedLineItem->setAcceptHoverEvents(true);
        fixedLineItem->setFlags(QGraphicsItem::ItemIsMovable |
                                QGraphicsItem::ItemSendsGeometryChanges);
        i++;
    }

    m_cursorRectangleItem = addGraphicsItem<GraphicsMeasureRectItem>();
    m_fixedRectangleItem  = addGraphicsItem<GraphicsMeasureRectItem>();

    connect(m_fixedLinesItem[0], &GraphicsLineItem::positionChanged,
            this, [&](const QPointF& point){
        auto rect = m_originalFixedRectangle.adjusted(0, point.y(), 0, 0);
        if (isRectangleValid(rect))
        {
            emit fixedRectanglChanged(rect);
        }
    });

    connect(m_fixedLinesItem[1], &GraphicsLineItem::positionChanged,
            this, [&](const QPointF& point){
        auto rect = m_originalFixedRectangle.adjusted(0, 0, 0, point.y());
        if (isRectangleValid(rect))
        {
            emit fixedRectanglChanged(rect);
        }
    });

    connect(m_fixedLinesItem[2], &GraphicsLineItem::positionChanged,
            this, [&](const QPointF& point){
        auto rect = m_originalFixedRectangle.adjusted(point.x(), 0, 0, 0);
        if (isRectangleValid(rect))
        {
            emit fixedRectanglChanged(rect);
        }
    });

    connect(m_fixedLinesItem[3], &GraphicsLineItem::positionChanged,
            this, [&](const QPointF& point){
        auto rect = m_originalFixedRectangle.adjusted(0, 0, point.x(), 0);
        if (isRectangleValid(rect))
        {
            emit fixedRectanglChanged(rect);
        }
    });

    hideAll();
    setOpacity(0.7);

    setSceneRect(itemsBoundingRect());
}

void Scene::hideAll()
{
    for (auto item : items())
    {
        item->setVisible(false);
    }
}

void Scene::setOpacity(float opacity)
{
    for (auto item : items())
    {
        auto op = (dynamic_cast<GraphicsTextItem*>(item))
                ? 1.0
                : opacity;

        item->setOpacity(op);
    }

    m_screenImageItem->setOpacity(1.0);
}

void Scene::setVisibility(const RenderData& renderData)
{
    m_screenImageItem->setVisible(true);
    m_cursorRectangleItem->setVisible(!renderData.isItemDragging &&
                                      renderData.isCursorRectPresent);

    m_cursorHLineItem->setVisible(m_cursorRectangleItem->isVisible());
    m_cursorVLineItem->setVisible(m_cursorRectangleItem->isVisible());

    m_measureHLineItem->setVisible(!renderData.isItemDragging &&
                                   renderData.isFixedRectPresent &&
                                   renderData.measureHLine.dx() > 0);
    m_measureVLineItem->setVisible(!renderData.isItemDragging &&
                                   renderData.isFixedRectPresent &&
                                   renderData.measureVLine.dy() > 0);

    m_fixedRectangleItem->setVisible(renderData.isFixedRectPresent);

    for (auto& fixedLineItem : m_fixedLinesItem)
    {
        fixedLineItem->setVisible(renderData.isFixedRectPresent);
    }
}

template<typename T>
T* Scene::addGraphicsItem()
{
    auto item = new T();
    addItem(item);

    return item;
}

QRectF Scene::toFloat(const QRect& rectangle)
{
    float x = rectangle.x() + 0.5;
    float y = rectangle.y() + 0.5;
    float w = rectangle.width();
    float h = rectangle.height();

    return QRectF{x, y, w , h};
}

QLineF Scene::toFloat(const QLine& line)
{
    float x1 = line.x1() + 0.5;
    float y1 = line.y1() + 0.5;
    float x2 = line.x2() + 0.5;
    float y2 = line.y2() + 0.5;

    return QLineF{x1, y1, x2, y2};
}

bool Scene::isRectangleValid(QRect& rect)
{
    return (rect.left() <= rect.right()) &&
           (rect.top() <= rect.bottom());
}
