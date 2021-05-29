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
    m_screenImageItem->setPixmap(renderData.screenImage);

    m_cursorHLineItem->setLine(toFloat(renderData.cursorHLine));
    m_cursorVLineItem->setLine(toFloat(renderData.cursorVLine));

    m_measureHLineItem->setLine(toFloat(renderData.measureHLine));
    m_measureVLineItem->setLine(toFloat(renderData.measureVLine));

    m_cursorRectangleItem->setRect(toFloat(renderData.cursorRectangle));
    m_fixedRectangleItem->setRect(toFloat(renderData.fixedRectangle));

    for(auto i = 0; i < m_fixedLinesItem.size(); ++i)
    {
        m_fixedLinesItem[i]->setLine(toFloat(renderData.fixedLines[i]));
    }

    //m_referenceVLinesItem[0] = addLine(toFloat(QLine{}));
    //m_referenceVLinesItem[1] = addLine(toFloat(QLine{}));
    //m_referenceHLinesItem[0] = addLine(toFloat(QLine{}));
    //m_referenceHLinesItem[1] = addLine(toFloat(QLine{}));

    setVisibility(renderData);
}

void Scene::setPalette(const Palette& palette)
{
    m_cursorHLineItem->setColor(palette.cursorLines);
    m_cursorVLineItem->setColor(palette.cursorLines);

    m_measureHLineItem->setColor(palette.measurerLines);
    m_measureVLineItem->setColor(palette.measurerLines);

    m_cursorRectangleItem->setColor(palette.cursorRectangle);
    m_fixedRectangleItem->setColor(palette.fixedRectangle);

    //m_referenceVLinesItem[0]->setColor(palette.measurerLines);
    //m_referenceVLinesItem[1]->setColor(palette.measurerLines);
    //m_referenceHLinesItem[0]->setColor(palette.measurerLines);
    //m_referenceHLinesItem[1]->setColor(palette.measurerLines);

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
    m_screenImageItem->setPos(0, 0);

    m_cursorHLineItem = addMeasureGraphicsItem<MeasureSimpleLineItem>();
    m_cursorVLineItem = addMeasureGraphicsItem<MeasureSimpleLineItem>();

    //m_referenceVLinesItem[0] = addMeasureGraphicsItem<MeasureSimpleLineItem>();
    //m_referenceVLinesItem[1] = addMeasureGraphicsItem<MeasureSimpleLineItem>();
    //m_referenceHLinesItem[0] = addMeasureGraphicsItem<MeasureSimpleLineItem>();
    //m_referenceHLinesItem[1] = addMeasureGraphicsItem<MeasureSimpleLineItem>();

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
}

void Scene::hideAll()
{
    for(auto item : items())
    {
        item->setVisible(false);
    }
}

void Scene::setVisibility(const RenderData& renderData)
{
    m_screenImageItem->setVisible(true);
    m_cursorHLineItem->setVisible(true);
    m_cursorVLineItem->setVisible(true);
    m_cursorRectangleItem->setVisible(true);
    m_measureHLineItem->setVisible(renderData.isFixedRectVisible);
    m_measureVLineItem->setVisible(renderData.isFixedRectVisible);
    m_fixedRectangleItem->setVisible(renderData.isFixedRectVisible);
    //m_referenceVLinesItem[0]->setVisible(renderData.isMeasurerRectPresent);
    //m_referenceVLinesItem[1]->setVisible(renderData.isMeasurerRectPresent);
    //m_referenceHLinesItem[0]->setVisible(renderData.isMeasurerRectPresent);
    //m_referenceHLinesItem[1]->setVisible(renderData.isMeasurerRectPresent);

    for(auto i = 0; i < m_fixedLinesItem.size(); ++i)
    {
        m_fixedLinesItem[i]->setVisible(renderData.isFixedRectVisible);
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
