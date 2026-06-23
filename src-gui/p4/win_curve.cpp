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

#include "win_curve.h"

#include "custom.h"
#include "file_vf.h"
#include "math_curve.h"
#include "math_polynom.h"

#include <QButtonGroup>
#include <QFrame>
#include <QLabel>
#include <QMessageBox>
#include <QSpinBox>
#include <QStyle>

QCurveDlg::QCurveDlg(QPlotWnd *plt, QWinSphere *sp)
    : QWidget(nullptr, Qt::Tool | Qt::WindowStaysOnTopHint), mainSphere_(sp),
      plotwnd_(plt)
{
    edt_curve_ = new QLineEdit("", this);
    edt_curve_->setMaximumWidth(160);
    QLabel *lbl0 = new QLabel("Curve: ", this);

    QButtonGroup *btngrp = new QButtonGroup(this);
    btn_dots_ = new QRadioButton("Dots", this);
    btn_dashes_ = new QRadioButton("Dashes", this);
    btngrp->addButton(btn_dots_);
    btngrp->addButton(btn_dashes_);
    QLabel *lbl1 = new QLabel("Appearance: ", this);

    edt_points_ = new QSpinBox(this);
    edt_points_->setRange(MIN_CURVEPOINTS, MAX_CURVEPOINTS);
    QLabel *lbl2 = new QLabel("Num. Points: ", this);

    edt_precis_ = new QSpinBox(this);
    edt_precis_->setRange(MIN_CURVEPRECIS, MAX_CURVEPRECIS);
    QLabel *lbl3 = new QLabel("Precision: ", this);

    edt_memory_ = new QSpinBox(this);
    edt_memory_->setRange(MIN_CURVEMEMORY, MAX_CURVEMEMORY);
    QLabel *lbl4 = new QLabel("Max. Memory: ", this);

    btnEvaluate_ = new QPushButton("Evaluate", this);
    btnPlot_ = new QPushButton("&Plot", this);
    btnDelLast_ = new QPushButton("&Delete Last Curve", this);
    btnDelAll_ = new QPushButton("Delete &All Curves", this);
    btnDefaults_ = new QPushButton("Defaults", this);

    btnEvaluate_->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
    btnPlot_->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    btnDelLast_->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
    btnDelAll_->setIcon(style()->standardIcon(QStyle::SP_DialogResetButton));
    btnDefaults_->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));

#ifdef TOOLTIPS
    btn_dots_->setToolTip("Plot individual points of the curve");
    btn_dashes_->setToolTip(
        "Connect points of the curve with small line segments");
    btnEvaluate_->setToolTip("Evaluate singular points of polynomial curve");
    btnPlot_->setToolTip("Plot curve (using symbolic manipulator)");
    btnDelAll_->setToolTip("Delete all curves");
    edt_points_->setToolTip("Number of sample points");
    edt_precis_->setToolTip("Required precision (digits)");
    edt_memory_->setToolTip("Maximum memory budget (kilobytes)");
#endif

    // layout
    mainLayout_ = new QBoxLayout(QBoxLayout::TopToBottom, this);
    mainLayout_->setContentsMargins(10, 10, 10, 10);
    mainLayout_->setSpacing(10);

    QHBoxLayout *layout0 = new QHBoxLayout();
    layout0->addWidget(lbl1);
    layout0->addWidget(btn_dots_);
    layout0->addWidget(btn_dashes_);
    layout0->addStretch(0);

    QGridLayout *layout1 = new QGridLayout();
    layout1->addWidget(lbl0, 0, 0);
    layout1->addWidget(edt_curve_, 0, 1);
    layout1->addWidget(lbl2, 1, 0);
    layout1->addWidget(edt_points_, 1, 1);
    layout1->addWidget(lbl3, 2, 0);
    layout1->addWidget(edt_precis_, 2, 1);
    layout1->addWidget(lbl4, 3, 0);
    layout1->addWidget(edt_memory_, 3, 1);

    QFrame *sep1 = new QFrame(this);
    sep1->setFrameShape(QFrame::HLine);
    sep1->setFrameShadow(QFrame::Sunken);

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->addWidget(btnEvaluate_);
    layout2->addStretch(0);
    layout2->addWidget(btnDelLast_);

    QHBoxLayout *layout3 = new QHBoxLayout();
    layout3->addWidget(btnPlot_);
    layout3->addStretch(0);
    layout3->addWidget(btnDelAll_);

    QHBoxLayout *layout4 = new QHBoxLayout();
    layout4->addStretch(1);
    layout4->addWidget(btnDefaults_);
    layout4->addStretch(1);

    mainLayout_->addLayout(layout0);
    mainLayout_->addLayout(layout1);
    mainLayout_->addWidget(sep1);
    mainLayout_->addLayout(layout2);
    mainLayout_->addLayout(layout3);
    mainLayout_->addLayout(layout4);
    mainLayout_->addStretch(0);

    setLayout(mainLayout_);

    // connections

    connect(btnEvaluate_, &QPushButton::clicked, this,
            &QCurveDlg::onBtnEvaluate);
    connect(btnPlot_, &QPushButton::clicked, this, &QCurveDlg::onBtnPlot);
    connect(btnDelLast_, &QPushButton::clicked, this, &QCurveDlg::onBtnDelLast);
    connect(btnDelAll_, &QPushButton::clicked, this, &QCurveDlg::onBtnDelAll);
    connect(btnDefaults_, &QPushButton::clicked, this,
            &QCurveDlg::onBtnDefaults);

    // finishing

    reset();
    setP4WindowTitle(this, "Curve plot");
}

void QCurveDlg::reset()
{
    edt_curve_->setText("");

    edt_points_->setValue(DEFAULT_CURVEPOINTS);
    edt_memory_->setValue(DEFAULT_CURVEMEMORY);
    edt_precis_->setValue(DEFAULT_CURVEPRECIS);

    btnEvaluate_->setEnabled(true);
    btnPlot_->setEnabled(false);

    if (!g_VFResults.curve_vector_.empty()) {
        btnDelAll_->setEnabled(true);
        btnDelLast_->setEnabled(true);
    }

    if (g_VFResults.config_dashes_)
        btn_dashes_->toggle();
    else
        btn_dots_->toggle();
}

void QCurveDlg::onBtnEvaluate()
{
    if (edt_curve_->text().isNull() || edt_curve_->text().isEmpty()) {
        QMessageBox::information(this, "P4",
                                 "The curve field has to be filled\n"
                                 "with the equation of a curve.\n");
        return;
    }
    g_ThisVF->curve_ = edt_curve_->text().trimmed();

    // FIRST: create filename_veccurve.tab for transforming the curve QString to
    // a list of P4POLYNOM2
    g_ThisVF->curveDlg_ = this;
    g_ThisVF->evaluateCurveTable();
    btnPlot_->setEnabled(true);
}

void QCurveDlg::onBtnPlot()
{
    bool dashes, result;
    int points, precis, memory;

    dashes = btn_dashes_->isChecked();
    points = edt_points_->value();
    precis = edt_precis_->value();
    memory = edt_memory_->value();

    // SECOND: read the resulting file and store the list
    if (!g_VFResults.readCurve(g_ThisVF->getbarefilename())) {
        QMessageBox::critical(this, "P4", "Cannot read curve.\n"
                                          "Please check the input field!\n");
        return;
    }

    // THIRD: evaluate curve with given parameters {dashes, points, memory}.

    evaluating_points_ = points;
    evaluating_memory_ = memory;
    evaluating_precision_ = precis;

    btnPlot_->setEnabled(false);

    g_ThisVF->curveDlg_ = this;
    result = evalCurveStart(mainSphere_, dashes, precis, points);
    if (!result) {
        btnPlot_->setEnabled(true);
        QMessageBox::critical(this, "P4", "An error occured while plotting the "
                                          "curve.\nThe singular locus may not "
                                          "be visible, or may be partially "
                                          "visible.");
        return;
    }

    btnDelAll_->setEnabled(true);
    btnDelLast_->setEnabled(true);
}

void QCurveDlg::onBtnDelAll()
{
    plotwnd_->getDlgData();

    btnEvaluate_->setEnabled(true);
    btnPlot_->setEnabled(false);
    btnDelAll_->setEnabled(false);
    btnDelLast_->setEnabled(false);

    g_VFResults.curve_vector_.clear();

    mainSphere_->refresh();
}

void QCurveDlg::onBtnDelLast()
{
    plotwnd_->getDlgData();

    deleteLastCurve(mainSphere_);

    btnEvaluate_->setEnabled(true);
    btnPlot_->setEnabled(false);

    if (g_VFResults.curve_vector_.empty()) {
        btnDelAll_->setEnabled(false);
        btnDelLast_->setEnabled(false);
    }
}

void QCurveDlg::onBtnDefaults()
{
    edt_points_->setValue(DEFAULT_CURVEPOINTS);
    edt_precis_->setValue(DEFAULT_CURVEPRECIS);
    edt_memory_->setValue(DEFAULT_CURVEMEMORY);
}

void QCurveDlg::finishCurveEvaluation()
{
    bool result;

    if (btnPlot_->isEnabled() == true)
        return; // not busy??

    result = evalCurveContinue(evaluating_precision_, evaluating_points_);

    if (result) {
        btnPlot_->setEnabled(false);
        result = evalCurveFinish(); // return false in case an error occured
        if (!result) {
            QMessageBox::critical(this, "P4", "An error occured while plotting "
                                              "the curve.\nThe singular locus "
                                              "may not be visible, or may "
                                              "be partially visible.");
        }
    }
}
