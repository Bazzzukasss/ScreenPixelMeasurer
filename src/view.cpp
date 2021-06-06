#include <QMouseEvent>
#include <QScrollBar>

#include "view.h"
#include "scene.h"
#include "calculator.h"

View::View(QWidget* parent)
    : QGraphicsView(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMouseTracking(true);

    m_scene = new Scene(this);
    m_scene->setPalette(m_palettes[m_paletteIndex]);
    connect(m_scene, &Scene::fixedRectanglChanged, this, &View::correctFixedRectangle);

    setScene(m_scene);
    updateScene();
}

void View::mousePressEvent(QMouseEvent* event)
{
    m_isItemDragging = m_scene->isMovableItemSelected(mapToScene(event->pos()).toPoint());
    if (!m_isItemDragging)
    {
        m_lastMousePos = event->pos();

        if (event->button() == Qt::LeftButton)
        {
            setFixedRectangle();
        }
        updateScene();
    }
    else
    {
        m_scene->startDragging();
    }

    QGraphicsView::mousePressEvent(event);
}

void View::mouseReleaseEvent(QMouseEvent* event)
{
    m_isItemDragging = false;

    QGraphicsView::mouseReleaseEvent(event);
}

void View::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_isItemDragging)
    {
        m_renderData.cursorPoint = mapToScene(event->x(), event->y()).toPoint();
        m_renderData.isCursorRectPresent = true;
        updateScene();

        if (event->buttons() & Qt::RightButton)
        {
            auto dx = event->x() - m_lastMousePos.x();
            auto dy = event->y() - m_lastMousePos.y();

            auto sx = horizontalScrollBar()->value();
            auto sy = verticalScrollBar()->value();

            horizontalScrollBar()->setValue(sx - dx);
            verticalScrollBar()->setValue(sy - dy);

            m_lastMousePos = {event->x(), event->y()};
        }
    }

    QGraphicsView::mouseMoveEvent(event);
}

void View::wheelEvent(QWheelEvent* event)
{
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;

    if (!numPixels.isNull())
    {
        changeScale(numPixels);
    }
    else if (!numDegrees.isNull())
    {
        changeScale(numDegrees);
    }

    auto targetViewportPos = event->position();
    auto targetScenePos = mapToScene(event->position().toPoint());
    QPointF deltaViewportPos = targetViewportPos -
            QPointF(viewport()->width() / 2.0, viewport()->height() / 2.0);

    resetTransform();
    scale(m_scale, m_scale);
    centerOn(targetScenePos);

    QPointF viewportCenter = mapFromScene(targetScenePos) - deltaViewportPos;
    centerOn(mapToScene(viewportCenter.toPoint()));

    updateScene();
}

void View::updateScene()
{
    calculate();
    m_scene->setRenderData(m_renderData);
    update();
}

void View::setFixedRectangle()
{
    m_renderData.isFixedRectPresent =
            m_renderData.fixedRectangle != m_renderData.cursorRectangle;

    if (m_renderData.isFixedRectPresent)
    {
        m_renderData.fixedRectangle = m_renderData.cursorRectangle;
    }
}

void View::correctFixedRectangle(const QRect& rect)
{
    m_renderData.fixedRectangle = rect;
    updateScene();
}

void View::calculate()
{
    if (m_renderData.isCursorRectPresent)
    {
        m_renderData.cursorRectangle =
                Calculator::calculateCursorRectangle(m_renderData.cursorPoint,
                                                     m_renderData.screenImage.toImage());
        auto lines = Calculator::calculateCursorLines(m_renderData.cursorPoint,
                                                      m_renderData.cursorRectangle);
        m_renderData.cursorHLine = lines[0];
        m_renderData.cursorVLine = lines[1];
    }
    else
    {
        m_renderData.cursorPoint = {1,1};
        m_renderData.cursorHLine = {1,1,1,1};
        m_renderData.cursorVLine = {1,1,1,1};
        m_renderData.cursorRectangle = {1,1,1,1};
    }

    if (m_renderData.isFixedRectPresent && m_renderData.isCursorRectPresent)
    {
        m_renderData.fixedLines = Calculator::calculateFixedLines(m_renderData.fixedRectangle,
                                                                  m_renderData.screenImage.toImage());
        auto lines = Calculator::calculateMeasureLines(m_renderData.cursorRectangle,
                                                       m_renderData.fixedRectangle);

        m_renderData.measureVLine = lines[0];
        m_renderData.measureHLine = lines[1];
    }
    else
    {
        m_renderData.fixedRectangle = {1,1,1,1};
        m_renderData.fixedLines[0] = {1,1,1,1};
        m_renderData.fixedLines[1] = {1,1,1,1};
        m_renderData.fixedLines[2] = {1,1,1,1};
        m_renderData.fixedLines[3] = {1,1,1,1};
        m_renderData.measureVLine = {1,1,1,1};
        m_renderData.measureHLine = {1,1,1,1};
    }
}

int View::beamTo(int startPos, int endPos, int coord, int step,
                       Qt::Orientation orientation, const QRgb& color)
{
    int resPos = endPos;
    for (int pos = startPos + step; pos != endPos; pos += step)
    {
        auto point = orientation == Qt::Horizontal ? QPoint(pos, coord)
                                                   : QPoint(coord, pos);

        if (!m_renderData.screenImage.rect().contains(point))
        {
            break;
        }

        if (m_renderData.screenImage.toImage().pixel(point) != color)
        {
            return pos - step;
        }
    }
    return resPos;
}

void View::changeScale(const QPoint& delta)
{
    m_scale += delta.y() > 0 ? 1 : -1;

    if (m_scale > kMaxScale)
    {
        m_scale = kMaxScale;
    }

    if (m_scale < kMinScale)
    {
        m_scale = kMinScale;
    }
}

void View::switchPalette()
{
    if(++m_paletteIndex >= m_palettes.size())
    {
        m_paletteIndex = 0;
    }
    m_scene->setPalette(m_palettes[m_paletteIndex]);
}

void View::shiftScene(int dx, int dy)
{
    m_renderData.fixedRectangle.translate(dx, dy);
    updateScene();
}

void View::setPixmap(const QPixmap& pixmap)
{
    m_renderData.screenImage = pixmap;
    updateScene();
}
