#include "calculator.h"

Calculator::Calculator()
{
}

QRect Calculator::calculateCursorRectangle(const QPoint& pos, const QImage& img)
{
    auto x = pos.x();
    auto y = pos.y();
    auto color = img.pixel(x, y);
    auto cr = Calculator::beamTo(x, img.width() - 1, y, 1, Qt::Horizontal, color, img);
    auto cl = Calculator::beamTo(x, 0, y, -1, Qt::Horizontal, color, img);
    auto cb = Calculator::beamTo(y, img.height() - 1, x, 1, Qt::Vertical, color, img);
    auto ct = Calculator::beamTo(y, 0, x, -1, Qt::Vertical, color, img);

    return {cl, ct, cr - cl, cb - ct};
}

std::array<QLine, 2> Calculator::calculateCursorLines(const QPoint& pos, const QRect& cursorRect)
{
    int l, r, t, b;
    auto x = pos.x();
    auto y = pos.y();

    cursorRect.getCoords(&l, &t, &r, &b);

    return {
        QLine{x, t, x, b},
        QLine{l, y, r, y}
    };
}

std::array<QLine, 4> Calculator::calculateFixedLines(const QRect& fixedRect, const QImage& img)
{
    int l, t, b, r;
    auto w = img.width();
    auto h = img.height();
    fixedRect.getCoords(&l, &t, &r, &b);

    return {
        QLine{1, t, w - 1, t},
        QLine{1, b + 1, w - 1, b + 1},
        QLine{l, 1, l, h - 1},
        QLine{r + 1, 1, r + 1, h - 1}
    };
}

std::array<QLine, 2> Calculator::calculateMeasureLines(const QRect& cursorRect, const QRect& fixedRect)
{
    int fl, ft, fb, fr, cl, cr, ct, cb;
    int hx1{1}, hx2{1}, hy1{1}, hy2{1};
    int vx1{1}, vx2{1}, vy1{1}, vy2{1};

    cursorRect.getCoords(&cl, &ct, &cr, &cb);
    fixedRect.getCoords(&fl, &ft, &fr, &fb);

    auto ccx = cursorRect.center().x();
    auto ccy = cursorRect.center().y();
    auto fcx = fixedRect.center().x();
    auto fcy = fixedRect.center().y();

    if (cursorRect != fixedRect)
    {
        if (cursorRect.contains(fixedRect))
        {
            hx2 = fl - 1;   hy2 = fcy;      hx1 = cl;   hy1 = fcy;
            vx2 = fcx;      vy2= ft - 1;    vx1 = fcx;  vy1 = ct;
        }
        else if (fixedRect.contains(cursorRect))
        {
            hx2 = cl - 1;   hy2 = ccy;      hx1 =fl;    hy1 = ccy;
            vx2 = ccx;      vy2 = ct - 1;   vx1 = ccx;  vy1 = ft;
        }
        else
        {
            vx1 = vx2 = ccx;
            hy1 = hy2 = ccy;

            if (cb < ft)
            {
                vy1 = cb + 1; vy2 = ft - 1;
            }
            else if (ct > fb)
            {
                vy1 = fb + 2; vy2 = ct - 1;
            }
            else
            {
                if (ct < ft)
                {
                    vy1 = ct; vy2 = ft - 1;
                }
                else if (ct > ft)
                {
                     vy1 = ft; vy2 = ct - 1;
                }
            }

            if (cr < fl)
            {
                hx1 = cr + 1; hx2 = fl - 1;
            }
            else if (cl > fr)
            {
                hx1 = fr + 2; hx2 = cl - 1;
            }
            else
            {
                if (cl < fl)
                {
                    hx1 = cl;  hx2 = fl - 1;
                }
                else if (cl > fl)
                {
                    hx1 = fl; hx2 = cl - 1;
                }
            }
        }
    }

    return {
        QLine{vx1, vy1, vx2, vy2},
        QLine{hx1, hy1, hx2, hy2}
    };
}

int Calculator::beamTo(int startPos, int endPos, int coord, int step,
                       Qt::Orientation orientation, const QRgb& color, const QImage& img)
{
    int resPos = endPos;
    for (int pos = startPos + step; pos != endPos; pos += step)
    {
        auto point = orientation == Qt::Horizontal ? QPoint(pos, coord)
                                                   : QPoint(coord, pos);

        if (!img.rect().contains(point))
        {
            break;
        }

        if (img.pixel(point) != color)
        {
            return pos - step;
        }
    }
    return resPos;
}
