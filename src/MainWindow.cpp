#include <QDesktopWidget>
#include <QScreen>
#include <QApplication>
#include <QKeyEvent>
#include <QShortcut>
#include <QVBoxLayout>
#include <QTimer>

#include "MainWindow.h"

MainWindow::MainWindow(QWidget* parent) :
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
#ifdef Q_OS_WIN
    setWindowOpacity(0.1);
#endif

    initialize();
}

void MainWindow::initialize()
{
    m_view = new View(this);
    m_view->hide();

    auto layout = new QVBoxLayout();
    layout->addWidget(m_view);
    layout->setMargin(0);

    auto centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    auto shortcut = new QShortcut(QKeySequence(Qt::Key_P), this);
    connect(shortcut, &QShortcut::activated, m_view, &View::switchPalette);

    QTimer::singleShot(0,[&](){
        grabScreen();
    });
}

void MainWindow::enterEvent(QEvent* event)
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

void MainWindow::leaveEvent(QEvent* event)
{
#ifdef Q_OS_WIN
    setWindowOpacity(0.1);
#endif
    m_lastWindowPos = pos();
    m_view->hide();
}

void MainWindow::grabScreen()
{
    auto screen = QGuiApplication::primaryScreen();
    auto descktop = QApplication::desktop();
    m_view->setPixmap(screen->grabWindow(descktop->winId()).copy(geometry().adjusted(1, 1, -1, -1)));
}

