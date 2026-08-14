/*  This file is part of P4
 *
 *  Copyright (C) 1996-2017  J.C. Artés, P. De Maesschalck, F. Dumortier
 *                           C. Herssens, J. Llibre, O. Saleta, J. Torregrosa
 *
 *  P4 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "win_plot.h"

#include "custom.h"
#include "file_vf.h"
#include "main.h"
#include "math_separatrice.h"
#include "p4application.h"
#include "plot_tools.h"
#include "win_curve.h"
#include "win_event.h"
#include "win_gcf.h"
#include "win_intparams.h"
#include "win_limitcycles.h"
#include "win_orbits.h"
#include "win_print.h"
#include "win_separatrice.h"
#include "win_zoom.h"

#include <QDockWidget>
#include <QGuiApplication>
#include <QMenu>
#include <QMenuBar>
#include <QPrintDialog>
#include <QResizeEvent>
#include <QScreen>
#include <QSettings>
#include <QTabWidget>
#include <QTimer>

QPlotWnd::QPlotWnd(QStartDlg *main) : QMainWindow()
{
    setContextMenuPolicy(Qt::NoContextMenu);
    
    parent_ = main;

    // setAttribute( Qt::WA_PaintOnScreen, true );
    // setAttribute( Qt::WA_PaintOutsidePaintEvent, true );

    if (g_p4smallicon != nullptr)
        setWindowIcon(*g_p4smallicon);

    numZooms_ = 0;
    lastZoomIdentifier_ = 0;
    flagAllSepsPlotted_ = false;

    //    QPalette palette;
    //    palette.setColor(backgroundRole(), QXFIGCOLOR(bgColours::CBACKGROUND) );
    //    setPalette(palette);

    // No toolbars: the docked tab panel is the only control surface, and the
    // menu bar carries the commands that have no tab of their own.  Eight of
    // the old toolbar buttons only revealed the dock and selected a tab, which
    // the tabs already do.
    //
    // The five commands lose their Alt shortcuts because those collide with the
    // menu mnemonics (Close Alt+E vs &Edit, Print Alt+P vs &Plot, Legend Alt+L
    // vs Pane&ls); Print and Close take the standard sequences instead.

    actClose_ = new QAction("Clos&e", this);
    actClose_->setShortcut(QKeySequence::Close);
    connect(actClose_, &QAction::triggered, this, &QPlotWnd::onBtnClose);

    actRefresh_ = new QAction("&Refresh", this);
    connect(actRefresh_, &QAction::triggered, this, &QPlotWnd::onBtnRefresh);

    actLegend_ = new QAction("&Legend", this);
    connect(actLegend_, &QAction::triggered, this, &QPlotWnd::onBtnLegend);

    actPlotAllSeps_ = new QAction("Plot All Separa&trices", this);
    connect(actPlotAllSeps_, &QAction::triggered, this,
            &QPlotWnd::onBtnPlotAllSeps);

    actPrint_ = new QAction("&Print...", this);
    actPrint_->setShortcut(QKeySequence::Print);
    connect(actPrint_, &QAction::triggered, this, &QPlotWnd::onBtnPrint);

    // Panel actions: these reveal the dock and select their tab.  They keep
    // their Alt shortcuts, which do not collide with the menu mnemonics.

    actOrbits_ = new QAction("&Orbits", this);
    actOrbits_->setShortcut(Qt::ALT | Qt::Key_O);
    connect(actOrbits_, &QAction::triggered, this, &QPlotWnd::onBtnOrbits);

    actIntParams_ = new QAction("&Integration Parameters", this);
    actIntParams_->setShortcut(Qt::ALT | Qt::Key_I);
    connect(actIntParams_, &QAction::triggered, this,
            &QPlotWnd::onBtnIntParams);

    actGCF_ = new QAction("&GCF", this);
    actGCF_->setShortcut(Qt::ALT | Qt::Key_G);
    connect(actGCF_, &QAction::triggered, this, &QPlotWnd::onBtnGCF);

    actCurve_ = new QAction("&Curves", this);
    actCurve_->setShortcut(Qt::ALT | Qt::Key_C);
    connect(actCurve_, &QAction::triggered, this, &QPlotWnd::onBtnCurve);

    actPlotSep_ = new QAction("Plot &Separatrice", this);
    actPlotSep_->setShortcut(Qt::ALT | Qt::Key_S);
    connect(actPlotSep_, &QAction::triggered, this, &QPlotWnd::onBtnPlotSep);

    actLimitCycles_ = new QAction("Limit C&ycles", this);
    actLimitCycles_->setShortcut(Qt::ALT | Qt::Key_Y);
    connect(actLimitCycles_, &QAction::triggered, this,
            &QPlotWnd::onBtnLimitCycles);

    actIsoclines_ = new QAction("Isoclines", this);
    connect(actIsoclines_, &QAction::triggered, this,
            &QPlotWnd::onBtnIsoclines);

    actView_ = new QAction("&View", this);
    actView_->setShortcut(Qt::ALT | Qt::Key_V);
    connect(actView_, &QAction::triggered, this, &QPlotWnd::onBtnView);

    undoStack_ = new QUndoStack(this);
    undoStack_->setUndoLimit(50);

    QAction *actUndo = undoStack_->createUndoAction(this, "&Undo");
    actUndo->setShortcut(QKeySequence::Undo);
    QAction *actRedo = undoStack_->createRedoAction(this, "&Redo");
    actRedo->setShortcut(QKeySequence::Redo);

    QMenu *fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(actPrint_);
    fileMenu->addSeparator();
    fileMenu->addAction(actClose_);

    QMenu *editMenu = menuBar()->addMenu("&Edit");
    editMenu->addAction(actUndo);
    editMenu->addAction(actRedo);

    QMenu *plotMenu = menuBar()->addMenu("&Plot");
    plotMenu->addAction(actRefresh_);
    plotMenu->addAction(actLegend_);
    plotMenu->addAction(actPlotAllSeps_);

    QMenu *panelsMenu = menuBar()->addMenu("Pane&ls");
    panelsMenu->addAction(actOrbits_);
    panelsMenu->addAction(actPlotSep_);
    panelsMenu->addAction(actGCF_);
    panelsMenu->addAction(actCurve_);
    panelsMenu->addAction(actIsoclines_);
    panelsMenu->addAction(actLimitCycles_);
    panelsMenu->addAction(actIntParams_);
    panelsMenu->addAction(actView_);

    connect(g_ThisVF, &QInputVF::saveSignal, this, &QPlotWnd::onSaveSignal);
    connect(g_ThisVF, &QInputVF::loadSignal, this, &QPlotWnd::onLoadSignal);

#ifdef TOOLTIPS

    actClose_->setToolTip(
        "Closes the plot window, all subwindows and zoom window");
    actRefresh_->setToolTip("Redraw the plot window");
    actLegend_->setToolTip("Show legend");
    actOrbits_->setToolTip("Opens \"Integrate Orbits\" window");
    actIntParams_->setToolTip("Opens \"Integration Parameters\" window");
    actGCF_->setToolTip("Opens Greatest-Common-Factor window.\n"
                        "Disabled if there is no GCF");
    actCurve_->setToolTip("Opens curve plot window.");
    actPlotSep_->setToolTip("Opens \"Plot separatrices\" window");
    actPlotAllSeps_->setToolTip(
        "Plots all separatrices of all singular points "
        "with default integration parameters.\n"
        "Change integration parameters if the effect is "
        "too small to be visible.");
    actLimitCycles_->setToolTip("Opens limit cycle window");
    actView_->setToolTip("Opens the \"View parameter\" window");
    actPrint_->setToolTip("Opens the print window");
#endif

    coordLabel_ = new QLabel(this);
    coordLabel_->setMinimumWidth(280);
    statusBar()->addPermanentWidget(coordLabel_);
    statusBar()->showMessage("Ready");

    sphere_ = new QWinSphere(this, statusBar(), false, 0, 0, 0, 0);
    connect(sphere_, &QWinSphere::coordsChanged, coordLabel_, &QLabel::setText);
    legendWindow_ = new QLegendWnd();
    orbitsWindow_ = new QOrbitsDlg(this, sphere_);
    sepWindow_ = new QSepDlg(this, sphere_);
    intParamsWindow_ = new QIntParamsDlg();
    viewParamsWindow_ = new QViewDlg(this);
    lcWindow_ = new QLimitCyclesDlg(this, sphere_);
    gcfWindow_ = new QGcfDlg(this, sphere_);
    curveWindow_ = new QCurveDlg(this, sphere_);
    isoclinesWindow_ = new QIsoclinesDlg(this, sphere_);
    g_LCWindowIsUp = false; // Limit cycles: initially hidden

    connect(undoStack_, &QUndoStack::indexChanged, this, [=](int) {
        orbitsWindow_->reset();
        sphere_->update();
    });

    // Dock panel — embed all plot-control dialogs into a tabbed side panel
    auto makeTab = [](QWidget *w, QTabWidget *tabs, const QString &label) {
        w->setWindowFlags(Qt::Widget);
        tabs->addTab(w, label);
    };
    controlTabs_ = new QTabWidget(this);
    makeTab(orbitsWindow_,    controlTabs_, "Orbits");
    makeTab(sepWindow_,       controlTabs_, "Separatrices");
    makeTab(gcfWindow_,       controlTabs_, "GCF");
    makeTab(curveWindow_,     controlTabs_, "Curves");
    makeTab(isoclinesWindow_, controlTabs_, "Isoclines");
    makeTab(lcWindow_,        controlTabs_, "Limit Cycles");
    makeTab(intParamsWindow_, controlTabs_, "Int. Params");
    makeTab(viewParamsWindow_, controlTabs_, "View");

    controlDock_ = new QDockWidget("Controls", this);
    controlDock_->setObjectName("ControlDock");
    controlDock_->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    controlDock_->setFeatures(QDockWidget::DockWidgetClosable |
                               QDockWidget::DockWidgetMovable);
    controlDock_->setWidget(controlTabs_);
    // Wider than before so all eight tabs are readable without scroll buttons.
    controlDock_->setMinimumWidth(300);
    controlDock_->setMaximumWidth(480);
    // Keep the scroll buttons: even at this width the eight tabs do not all
    // fit, and without them the last one cannot be reached by clicking.
    controlTabs_->setElideMode(Qt::ElideNone);
    addDockWidget(Qt::RightDockWidgetArea, controlDock_);

    sphere_->show();
    setCentralWidget(sphere_);

    // Size the sphere relative to the available screen space so that the
    // plot window isn't tiny on high-resolution / large displays, while
    // never going below the original nominal size on small screens.
    constexpr int kDockWidth = 430;
    int sphereSize = NOMINALWIDTHPLOTWINDOW;
    if (QScreen *screen = this->screen() ? this->screen()
                                          : QGuiApplication::primaryScreen()) {
        QRect avail = screen->availableGeometry();
        int maxSphere = qMin(avail.width() - kDockWidth, avail.height());
        sphereSize = qMax(NOMINALWIDTHPLOTWINDOW, maxSphere * 7 / 10);
    }
    // Shorter than it is wide: the disc no longer needs a square sphere, so the
    // height only decides how much empty margin sits above and below it.
    resize(sphereSize + kDockWidth,
           qMax(NOMINALHEIGHTPLOTWINDOW, sphereSize * 3 / 4));

    intParamsWindow_->updateDlgData();
    viewParamsWindow_->updateDlgData();

    //  if( g_ThisVF->evaluated_ )
    setP4WindowTitle(this, "Phase Portrait");
    //  else
    //      SetP4WindowTitle( this, "Phase Portrait (*)" );
}

QPlotWnd::~QPlotWnd()
{
    // undoStack_ is a child QObject and will be destroyed by ~QWidget()'s
    // deleteChildren(), after this destructor body has already deleted and
    // nulled out the dialog pointers below. ~QUndoStack() calls clear(),
    // which would emit indexChanged and run the lambda below on those
    // now-dangling pointers. Disconnect it now to avoid that.
    disconnect(undoStack_, &QUndoStack::indexChanged, this, nullptr);

    zoomWindows_.clear();
    numZooms_ = 0;

    delete legendWindow_;
    legendWindow_ = nullptr;
    delete orbitsWindow_;
    orbitsWindow_ = nullptr;
    delete intParamsWindow_;
    intParamsWindow_ = nullptr;
    delete viewParamsWindow_;
    viewParamsWindow_ = nullptr;
    delete sepWindow_;
    sepWindow_ = nullptr;
    delete lcWindow_;
    lcWindow_ = nullptr;
    delete gcfWindow_;
    gcfWindow_ = nullptr;
    g_ThisVF->gcfDlg_ = nullptr;
    delete curveWindow_;
    curveWindow_ = nullptr;
    g_ThisVF->curveDlg_ = nullptr;
    delete isoclinesWindow_;
    isoclinesWindow_ = nullptr;
    g_ThisVF->isoclinesDlg_ = nullptr;
}

void QPlotWnd::onSaveSignal()
{
    QString fname = g_ThisVF->getbarefilename().append(".conf");
    QSettings settings(fname, QSettings::NativeFormat);
    settings.setValue("QPlotWnd/size", size());
    settings.setValue("QPlotWnd/pos", pos());
    settings.setValue("QPlotWnd/numZooms", numZooms_);
    settings.setValue("QPlotWnd/allSeps",flagAllSepsPlotted_);
}

void QPlotWnd::onLoadSignal()
{
    QString fname = g_ThisVF->getbarefilename().append(".conf");
    QSettings settings(fname, QSettings::NativeFormat);
    resize(settings.value("QPlotWnd/size").toSize());
    move(settings.value("QPlotWnd/pos").toPoint());

    numZooms_ = settings.value("QPlotWnd/numZooms").toInt();
    if (numZooms_ != 0) {
        for (int i = 1; i <= numZooms_; i++) {
            QString zoomName = QString("QZoomWnd%1").arg(i);
            settings.beginGroup(zoomName);
            int currentZoomId = settings.value("id").toInt();
            double currentZoomX1 = settings.value("x1").toDouble();
            double currentZoomX2 = settings.value("x2").toDouble();
            double currentZoomY1 = settings.value("y1").toDouble();
            double currentZoomY2 = settings.value("y2").toDouble();
            QZoomWnd *thiszoom =
                new QZoomWnd(this, currentZoomId, currentZoomX1, currentZoomY1,
                             currentZoomX2, currentZoomY2);
            thiszoom->show();
            thiszoom->raise();
            thiszoom->adjustHeight();
            thiszoom->resize(settings.value("size").toSize());
            thiszoom->move(settings.value("pos").toPoint());
            zoomWindows_.push_back(std::shared_ptr<QZoomWnd>(thiszoom));
            settings.endGroup();
        }
    }

    if (settings.value("QPlotWnd/allSeps").toBool()) {
        onBtnPlotAllSeps(); 
    }
}

void QPlotWnd::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
    QTimer::singleShot(0, this, &QPlotWnd::adjustHeight);
}

void QPlotWnd::adjustHeight(void)
{
    // This used to grow the window until the sphere was square, re-converging
    // on every resize.  The disc is drawn with a uniform scale now, so it stays
    // circular in a window of any shape; only the unused margin changes, and
    // the window keeps the size the user chose.
    sphere_->adjustToNewSize();
    sphere_->refresh();
    statusBar()->showMessage("Ready.");
}

void QPlotWnd::signalChanged(void)
{
    //  SetP4WindowTitle( this, "Phase Portrait (*)" );

    sphere_->signalChanged();
    std::vector<std::shared_ptr<QZoomWnd>>::const_iterator it;
    for (it = zoomWindows_.begin(); it != zoomWindows_.end(); it++)
        (*it)->signalChanged();
}

void QPlotWnd::signalEvaluating(void)
{
    //  SetP4WindowTitle( this, "Phase Portrait (*)" );

    sphere_->signalEvaluating();
    std::vector<std::shared_ptr<QZoomWnd>>::const_iterator it;
    for (it = zoomWindows_.begin(); it != zoomWindows_.end(); it++)
        (*it)->signalEvaluating();
}

void QPlotWnd::signalEvaluated(void)
{
    //  SetP4WindowTitle( this, "Phase Portrait" );

    configure();
    std::vector<std::shared_ptr<QZoomWnd>>::const_iterator it;
    for (it = zoomWindows_.begin(); it != zoomWindows_.end(); it++)
        (*it)->signalEvaluated();
}

void QPlotWnd::onBtnClose(void)
{
    QP4Event *e1 = new QP4Event((QEvent::Type)TYPE_CLOSE_PLOTWINDOW, nullptr);
    g_p4app->postEvent(parent_, e1);
}

bool QPlotWnd::close(void)
{
    QP4Event *e1 = new QP4Event((QEvent::Type)TYPE_CLOSE_PLOTWINDOW, nullptr);
    g_p4app->postEvent(parent_, e1);

    return QMainWindow::close();
}

void QPlotWnd::onBtnRefresh(void)
{
    getDlgData();
    sphere_->refresh();
}

void QPlotWnd::onBtnLegend(void)
{
    if (legendWindow_ == nullptr)
        legendWindow_ = new QLegendWnd();

    legendWindow_->show();
    legendWindow_->raise();
}

void QPlotWnd::onBtnOrbits(void)
{
    controlDock_->show();
    controlTabs_->setCurrentWidget(orbitsWindow_);
}

void QPlotWnd::onBtnIntParams(void)
{
    controlDock_->show();
    controlTabs_->setCurrentWidget(intParamsWindow_);
}

void QPlotWnd::onBtnView(void)
{
    controlDock_->show();
    controlTabs_->setCurrentWidget(viewParamsWindow_);
}

void QPlotWnd::onBtnGCF(void)
{
    controlDock_->show();
    controlTabs_->setCurrentWidget(gcfWindow_);
}

void QPlotWnd::onBtnCurve(void)
{
    controlDock_->show();
    controlTabs_->setCurrentWidget(curveWindow_);
}

void QPlotWnd::onBtnIsoclines(void)
{
    controlDock_->show();
    controlTabs_->setCurrentWidget(isoclinesWindow_);
}

void QPlotWnd::onBtnPlotSep(void)
{
    getDlgData();
    controlDock_->show();
    controlTabs_->setCurrentWidget(sepWindow_);
}

void QPlotWnd::onBtnPlotAllSeps(void)
{
    getDlgData();
    sphere_->prepareDrawing();
    plot_all_sep(sphere_);
    sphere_->finishDrawing();
    flagAllSepsPlotted_ = true;
}

void QPlotWnd::onBtnLimitCycles(void)
{
    controlDock_->show();
    controlTabs_->setCurrentWidget(lcWindow_);
}

void QPlotWnd::onBtnPrint(void)
{
    int res;
    double lw;
    double ss;

    QPrintDlg *pdlg;
    pdlg = new QPrintDlg(this, Qt::WindowFlags());
    int result = pdlg->exec();

    res = pdlg->getChosenResolution();
    lw = pdlg->getChosenLineWidth();
    ss = pdlg->getChosenSymbolSize();

    delete pdlg;
    pdlg = nullptr;

    if (result != P4PRINT_NONE) {
        if (result == P4PRINT_DEFAULT || result == -P4PRINT_DEFAULT) {
            g_p4printer->setResolution(res);

            QPrintDialog dialog(g_p4printer, this);
            if (!dialog.exec())
                return;

            res = g_p4printer->resolution();
        }

        if (result < 0)
            sphere_->preparePrinting(-result, true, res, lw, ss);
        else
            sphere_->preparePrinting(result, false, res, lw, ss);
        sphere_->print();
        sphere_->finishPrinting();
    }
}

void QPlotWnd::configure(void)
{
    statusBar()->showMessage("Ready"); // reset status bar
    plot_l = spherePlotLine; // setup line/plot pointing to routines of the
                             // sphere_ window
    plot_p = spherePlotPoint;
    sphere_->setupPlot(); // setup sphere_ window (define pixel transformations)
    intParamsWindow_->updateDlgData(); // update data of integration parameters
    viewParamsWindow_->updateDlgData();
    orbitsWindow_->reset(); // reset forward/backward buttons to initial state
    sepWindow_->reset();
    lcWindow_->reset();
    gcfWindow_->reset();
    curveWindow_->reset();
    isoclinesWindow_->reset();

    sphere_->update();
    if (g_VFResults.gcf_ == nullptr) // reconfigure GCF button
        actGCF_->setEnabled(false);
    else
        actGCF_->setEnabled(true);
}

void QPlotWnd::openZoomWindow(double x1, double y1, double x2, double y2)
{
    double swap;
    if (x1 > x2) {
        swap = x1;
        x1 = x2;
        x2 = swap;
    }
    if (y1 > y2) {
        swap = y1;
        y1 = y2;
        y2 = swap;
    }
    if (x1 == x2 || y1 == y2)
        return;

    QZoomWnd *newZoom =
        new QZoomWnd(this, ++lastZoomIdentifier_, x1, y1, x2, y2);
    newZoom->show();
    newZoom->raise();
    newZoom->adjustHeight();
    zoomWindows_.push_back(std::shared_ptr<QZoomWnd>(newZoom));
    numZooms_++;
}

void QPlotWnd::closeZoomWindow(int id)
{
    std::vector<std::shared_ptr<QZoomWnd>>::const_iterator it;
    for (it = zoomWindows_.begin(); it != zoomWindows_.end(); it++) {
        if ((*it)->zoomid_ == id) {
            zoomWindows_.erase(it);
            numZooms_--;
            return;
        }
    }
    return; // error, zoom window not found
}

void QPlotWnd::customEvent(QEvent *_e)
{
    QP4Event *e;
    double pcoord[3];
    double ucoord[2];
    double ucoord0[2];
    double ucoord1[2];
    struct DOUBLEPOINT *p;
    double x, y, x0, y0, x1, y1;
    int *oet;

    e = (QP4Event *)_e;

    if (e->type() == TYPE_OPENZOOMWINDOW) {
        double *data1;
        data1 = (double *)(e->data());

        openZoomWindow(data1[0], data1[1], data1[2], data1[3]);
        delete[] data1; // allocated as new double[4] in QWinSphere
        data1 = nullptr;
        return;
    }

    if (e->type() == TYPE_CLOSE_ZOOMWINDOW) {
        int *data2;
        data2 = (int *)(e->data());
        closeZoomWindow(*data2);
        delete data2;
        data2 = nullptr;
        return;
    }

    if (e->type() == TYPE_ORBIT_EVENT) {
        oet = (int *)(e->data());
        orbitsWindow_->orbitEvent(*oet);
        delete oet;
        oet = nullptr;
        return;
    }

    if (e->type() == TYPE_SELECT_ORBIT) {
        p = (struct DOUBLEPOINT *)(e->data());
        x = p->x;
        y = p->y;
        // void *win = *((void **)(p + 1));
        delete p;
        p = nullptr;

        // mouse clicked in position (x,y)  (world coordinates)

        if (MATHFUNC(is_valid_viewcoord)(x, y, pcoord)) {
            MATHFUNC(sphere_to_R2)(pcoord[0], pcoord[1], pcoord[2], ucoord);

            orbitsWindow_->setInitialPoint(ucoord[0], ucoord[1]);
            controlDock_->show();
            controlTabs_->setCurrentWidget(orbitsWindow_);
        }
        return;
    }

    if (e->type() == TYPE_SELECT_LCSECTION) {
        p = (struct DOUBLEPOINT *)(e->data());
        x0 = p->x;
        y0 = p->y;
        x1 = p[1].x;
        y1 = p[1].y;
        delete[] p; // allocated as new DOUBLEPOINT[2] in QWinSphere
        p = nullptr;

        MATHFUNC(viewcoord_to_sphere)(x0, y0, pcoord);
        MATHFUNC(sphere_to_R2)(pcoord[0], pcoord[1], pcoord[2], ucoord0);

        MATHFUNC(viewcoord_to_sphere)(x1, y1, pcoord);
        MATHFUNC(sphere_to_R2)(pcoord[0], pcoord[1], pcoord[2], ucoord1);

        if (x0 == x1 && y0 == y1) {
            orbitsWindow_->setInitialPoint(ucoord0[0], ucoord0[1]);
            controlDock_->show();
            controlTabs_->setCurrentWidget(orbitsWindow_);
            return;
        }

        // mouse clicked in position (x,y)  (world coordinates)

        lcWindow_->setSection(ucoord0[0], ucoord0[1], ucoord1[0], ucoord1[1]);
        controlDock_->show();
        controlTabs_->setCurrentWidget(lcWindow_);
        return;
    }

    if (e->type() == TYPE_SEP_EVENT) {
        oet = (int *)(e->data());
        sepWindow_->sepEvent(*oet);
        delete oet;
        oet = nullptr;
        controlDock_->show();
        controlTabs_->setCurrentWidget(sepWindow_);
        return;
    }

    QMainWindow::customEvent(e);
}

void QPlotWnd::hideEvent(QHideEvent *h)
{
    UNUSED(h);
    if (!isMinimized()) {
        QP4Event *e1 =
            new QP4Event((QEvent::Type)TYPE_CLOSE_PLOTWINDOW, nullptr);
        g_p4app->postEvent(parent_, e1);
    }
}

void QPlotWnd::getDlgData(void)
{
    intParamsWindow_->getDataFromDlg();
    if (viewParamsWindow_->getDataFromDlg()) {
        // true when a big change occured in the view

        g_VFResults.setupCoordinateTransformations();
        configure();
    }
}
