#include <QGraphicsRectItem>
#include <QGraphicsLineItem>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include "scene.h"

Scene::Scene(QObject* parent)
    : QGraphicsScene(parent)
{
    initialize();
}

void Scene::setRenderData(const RenderData& renderData)
{
    setVisibility(renderData);

    m_screenImageItem->setPixmap(renderData.screenImage);

    m_cursorHLineItem->setLine(toFloat(renderData.cursorHLine));
    m_cursorVLineItem->setLine(toFloat(renderData.cursorVLine));
    m_cursorRectangleItem->setRect(toFloat(renderData.cursorRectangle));

    m_fixedRectangleItem->setRect(toFloat(renderData.fixedRectangle));
    m_measureHLineItem->setLine(toFloat(renderData.measureHLine));
    m_measureVLineItem->setLine(toFloat(renderData.measureVLine));
    for(auto i = 0; i < m_fixedLinesItem.size(); ++i)
    {
        m_fixedLinesItem[i]->setLine(toFloat(renderData.fixedLines[i]));
    }

    setSceneRect(itemsBoundingRect());
}

void Scene::setPalette(const Palette& palette)
{
    m_cursorHLineItem->setColor(palette.cursorLines);
    m_cursorVLineItem->setColor(palette.cursorLines);

    m_measureHLineItem->setColor(palette.measurerLines);
    m_measureVLineItem->setColor(palette.measurerLines);

    m_cursorRectangleItem->setColor(palette.cursorRectangle);
    m_fixedRectangleItem->setColor(palette.fixedRectangle);

    for (auto lineItem : m_fixedLinesItem)
    {
        lineItem->setColor(palette.fixedLines);
    }

    for (auto item : m_items)
    {
        item->setBgColor(palette.background);
    }
}

void Scene::initialize()
{
    m_screenImageItem = addPixmap({});

    m_cursorHLineItem = addMeasureGraphicsItem<MeasureSimpleLineItem>();
    m_cursorVLineItem = addMeasureGraphicsItem<MeasureSimpleLineItem>();

    for (auto& lineItem : m_fixedLinesItem)
    {
        lineItem = addMeasureGraphicsItem<MeasureSimpleLineItem>();
        lineItem->setPenStyle(Qt::PenStyle::DashLine);
    }

    m_cursorRectangleItem = addMeasureGraphicsItem<MeasureRectItem>();
    m_fixedRectangleItem = addMeasureGraphicsItem<MeasureRectItem>();

    m_measureHLineItem = addMeasureGraphicsItem<MeasureLineItem>();
    m_measureHLineItem->setPenStyle(Qt::PenStyle::DotLine);

    m_measureVLineItem = addMeasureGraphicsItem<MeasureLineItem>();
    m_measureVLineItem->setPenStyle(Qt::PenStyle::DotLine);

    hideAll();
    setSceneRect(itemsBoundingRect());
}

void Scene::hideAll()
{
    for (auto item : m_items)
    {
        item->setItemsVisible(false);
    }
}

void Scene::setVisibility(const RenderData& renderData)
{
    m_screenImageItem->setVisible(true);
    m_cursorHLineItem->setItemsVisible(renderData.isCursorRectPresent);
    m_cursorVLineItem->setItemsVisible(renderData.isCursorRectPresent);
    m_cursorRectangleItem->setItemsVisible(renderData.isCursorRectPresent);
    m_measureHLineItem->setItemsVisible(renderData.isFixedRectPresent);
    m_measureVLineItem->setItemsVisible(renderData.isFixedRectPresent);
    m_fixedRectangleItem->setItemsVisible(renderData.isFixedRectPresent);

    for(auto i = 0; i < m_fixedLinesItem.size(); ++i)
    {
        m_fixedLinesItem[i]->setItemsVisible(renderData.isFixedRectPresent);
    }
}

template<typename T>
T* Scene::addMeasureGraphicsItem()
{
    auto item = new T();

    addItem(item);
    m_items.push_back(item);
    item->initialize();

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
