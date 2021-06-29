#include <QDesktopWidget>
#include <QScreen>
#include <QApplication>
#include <QShortcut>
#include <QVBoxLayout>
#include <QTimer>

#include "window.h"
#include "view.h"

Window::Window(QWidget* parent) :
    QMainWindow(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setWindowFlags(Qt::WindowStaysOnTopHint |
                   Qt::WindowTitleHint |
                   Qt::WindowCloseButtonHint |
                   Qt::WindowSystemMenuHint);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    setMouseTracking(true);
    setWindowTitle(kTitle);
#ifdef Q_OS_WIN
    setWindowOpacity(0.1);
#endif

    initialize();
}

void Window::initialize()
{
    m_view = new View(this);
    m_view->hide();
    connect(m_view, &View::renderDataChanged, this, &Window::updateTitle);

    auto layout = new QVBoxLayout();
    layout->addWidget(m_view);
    layout->setMargin(0);

    auto centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    auto paletteShortcut = new QShortcut(QKeySequence(Qt::Key_P), this);
    connect(paletteShortcut, &QShortcut::activated, m_view, &View::switchPalette);

    auto clearShortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);
    connect(clearShortcut, &QShortcut::activated, m_view, &View::clearFixedRect);

    QTimer::singleShot(0,[&](){
        grabScreen();
    });
}

void Window::enterEvent(QEvent*)
{
    static bool isFirstEnter{true};

    grabScreen();

    if (!isFirstEnter)
    {
        m_view->shiftScene(m_lastWindowPos.x() - pos().x(),
                           m_lastWindowPos.y() - pos().y());
    }

    isFirstEnter = false;

    m_view->show();
#ifdef Q_OS_WIN
    setWindowOpacity(1);
#endif
}

void Window::leaveEvent(QEvent*)
{
#ifdef Q_OS_WIN
    setWindowOpacity(0.1);
#endif
    m_lastWindowPos = pos();
    m_view->hide();
}

void Window::grabScreen()
{
    auto screen = QGuiApplication::primaryScreen();
    auto descktop = QApplication::desktop();
    auto winId = descktop->winId();

    m_view->setPixmap(
                screen->grabWindow(winId).copy(
                    geometry().adjusted(1, 1, -1, -1)));
}

void Window::updateTitle(const RenderData& renderData)
{
    auto info = renderData.cursorColor.isValid()
            ? "; Color: " + renderData.cursorColor.name()
            : "";

    setWindowTitle(kTitle + info);
}

