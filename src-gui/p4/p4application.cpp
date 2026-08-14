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

#include "p4application.h"

#include "custom.h"
#include "file_vf.h"
#include "win_event.h"

QP4Application *g_p4app = nullptr;

QP4Application::QP4Application(int &argc, char **argv)
    : QApplication(argc, argv)
{
    standardFont_ = new QFont();
    standardFont_->setPointSize(standardFont_->pointSize() + FONTSIZE);
    setFont(*standardFont_);

    boldFont_ = new QFont();
    boldFont_->setWeight(QFont::Bold);

    titleFont_ = new QFont();
    titleFont_->setPointSize(titleFont_->pointSize() + TITLEFONTSIZE);
    titleFont_->setWeight(QFont::Bold);

    courierFont_ = new QFont;
    courierFont_->setFamily("Courier");
    courierFont_->setFixedPitch(true);
    courierFont_->setPointSize(courierFont_->pointSize() + FONTSIZE);

    boldCourierFont_ = new QFont;
    boldCourierFont_->setFamily("Courier");
    boldCourierFont_->setFixedPitch(true);
    boldCourierFont_->setWeight(QFont::Bold);
    boldCourierFont_->setPointSize(boldCourierFont_->pointSize() + FONTSIZE);

    legendFont_ = new QFont;
    legendFont_->setFamily("Courier");
    legendFont_->setFixedPitch(true);
    legendFont_->setPointSize(legendFont_->pointSize() + LEGENDFONTSIZE);
}

// -----------------------------------------------------------------------
//                          SIGNALS
// -----------------------------------------------------------------------
//
// Following signals may be called from another thread

void QP4Application::signalChanged(void)
{
    g_ThisVF->evaluated_ = false;
    g_ThisVF->changed_ = true;

    QP4Event *e = new QP4Event((QEvent::Type)TYPE_SIGNAL_CHANGED, nullptr);
    g_p4app->postEvent(g_p4stardlg, e);
}

void QP4Application::signalEvaluated(int exitCode)
{
    g_ThisVF->evaluated_ = true;
    g_ThisVF->evaluating_ = false;

    g_ThisVF->finishEvaluation(exitCode);

    // In batch mode (-x) onQuit() deletes g_ThisVF and the child windows right
    // away.  Posting the "evaluated" event first would have it delivered after
    // that teardown, and QStartDlg::signalEvaluated() dereferences g_ThisVF --
    // which crashed p4 -x with an access violation.  Quit without posting: no
    // window is left to refresh.
    if (g_cmdLine_AutoExit) {
        g_cmdLine_AutoPlot = false;
        g_p4stardlg->onQuit();
        // The start dialog is never shown in batch mode, so closing it does not
        // count as closing the last window and quitOnLastWindowClosed never
        // fires -- exec() would keep running with no window left.  Stop the
        // event loop explicitly.
        quit();
        return;
    }

    QP4Event *e = new QP4Event((QEvent::Type)TYPE_SIGNAL_EVALUATED, nullptr);
    g_p4app->postEvent(g_p4stardlg, e);

    if (g_cmdLine_AutoPlot) {
        g_cmdLine_AutoPlot = false;
        g_p4stardlg->onPlot();
    }
}

void QP4Application::signalCurveEvaluated(int exitCode)
{
    g_ThisVF->evaluated_ = true;
    g_ThisVF->evaluating_ = false;

    g_ThisVF->finishEvaluation(exitCode);
}

void QP4Application::signalEvaluating(void)
{
    g_ThisVF->evaluating_ = true;
    g_ThisVF->evaluated_ = false;

    QP4Event *e = new QP4Event((QEvent::Type)TYPE_SIGNAL_EVALUATING, nullptr);
    g_p4app->postEvent(g_p4stardlg, e);
}

void QP4Application::catchProcessError(QProcess::ProcessError qperr)
{
    g_ThisVF->catchProcessError(qperr);
}

void QP4Application::signalLoaded(void)
{
    QP4Event *e = new QP4Event((QEvent::Type)TYPE_SIGNAL_LOADED, nullptr);
    g_p4app->postEvent(g_p4stardlg, e);

    if (g_cmdLine_AutoEvaluate) {
        g_cmdLine_AutoEvaluate = false;
        g_p4stardlg->findWindow_->onBtnEval();
        return;
    }
    if (g_cmdLine_AutoPlot) {
        g_cmdLine_AutoPlot = false;
        g_p4stardlg->onPlot();
        return;
    }
}

void QP4Application::signalSaved(void)
{
    QP4Event *e = new QP4Event((QEvent::Type)TYPE_SIGNAL_SAVED, nullptr);
    g_p4app->postEvent(g_p4stardlg, e);
}
