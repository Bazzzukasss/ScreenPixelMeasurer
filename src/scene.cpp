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
    m_screenImageItem->setPixmap(renderData.screenImage);

    m_cursorHLineItem->setLine(renderData.cursorHLine);
    m_cursorVLineItem->setLine(renderData.cursorVLine);

    m_measureHLineItem->setLine(renderData.measureHLine);
    m_measureVLineItem->setLine(renderData.measureVLine);

    m_cursorRectangleItem->setRect(renderData.cursorRectangle);
    m_fixedRectangleItem->setRect(renderData.fixedRectangle);

    //m_referenceVLinesItem[0] = addLine({});
    //m_referenceVLinesItem[1] = addLine({});
    //m_referenceHLinesItem[0] = addLine({});
    //m_referenceHLinesItem[1] = addLine({});
}

void Scene::setPalette(const Palette& palette)
{
    m_cursorHLineItem->setPen(palette.cursorLines);
    m_cursorVLineItem->setPen(palette.cursorLines);

    m_measureHLineItem->setPen(palette.measurerLines);
    m_measureVLineItem->setPen(palette.measurerLines);

    m_cursorRectangleItem->setPen(palette.cursorRectangle);
    m_fixedRectangleItem->setPen(palette.fixedRectangle);

    m_referenceVLinesItem[0]->setPen(palette.measurerLines);
    m_referenceVLinesItem[1]->setPen(palette.measurerLines);
    m_referenceHLinesItem[0]->setPen(palette.measurerLines);
    m_referenceHLinesItem[1]->setPen(palette.measurerLines);
}

void Scene::initialize()
{
    m_screenImageItem = addPixmap({});
    m_cursorHLineItem = addLine({});
    m_cursorVLineItem = addLine({});
    m_measureHLineItem = addLine({});
    m_measureVLineItem = addLine({});
    m_cursorRectangleItem = addRect({});
    m_fixedRectangleItem = addRect({});
    m_referenceVLinesItem[0] = addLine({});
    m_referenceVLinesItem[1] = addLine({});
    m_referenceHLinesItem[0] = addLine({});
    m_referenceHLinesItem[1] = addLine({});
}
