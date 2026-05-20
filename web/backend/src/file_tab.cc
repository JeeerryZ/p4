/*  This file is part of P4
 *
 *  Copyright (C) 1996-2017  J.C. Artés, P. De Maesschalck, F. Dumortier
 *                           C. Herssens, J. Llibre, O. Saleta, J. Torregrosa
 *
 *  Ported for the P4 REST backend: Qt/Wt dependencies removed.
 *  Wt::WString -> std::string, QFile paths -> plain std::string + fopen.
 */

#include "file_tab.h"
#include "math_polynom.h"

#include <clocale>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>

// -----------------------------------------------------------------------
//                        WVFStudy CONSTRUCTOR
// -----------------------------------------------------------------------
WVFStudy::WVFStudy()
{
    f_vec_field_[0] = f_vec_field_[1] = nullptr;
    vec_field_U1_[0] = vec_field_U1_[1] = nullptr;
    vec_field_U2_[0] = vec_field_U2_[1] = nullptr;
    vec_field_V1_[0] = vec_field_V1_[1] = nullptr;
    vec_field_V2_[0] = vec_field_V2_[1] = nullptr;
    vec_field_C_[0]  = vec_field_C_[1]  = nullptr;

    first_saddle_point_ = nullptr;
    first_se_point_     = nullptr;
    first_node_point_   = nullptr;
    first_sf_point_     = nullptr;
    first_wf_point_     = nullptr;
    first_de_point_     = nullptr;

    gcf_        = nullptr;
    gcf_U1_     = nullptr;
    gcf_U2_     = nullptr;
    gcf_V1_     = nullptr;
    gcf_V2_     = nullptr;
    gcf_C_      = nullptr;
    gcf_points_ = nullptr;

    first_lim_cycle_ = nullptr;
    first_orbit_     = nullptr;

    xmin_ = ymin_ = -1.0;
    xmax_ = ymax_ =  1.0;
    p_ = q_ = 1;
    typeofstudy_  = TYPEOFSTUDY_ALL;
    singinf_      = false;
    dir_vec_field_ = 1;

    config_lc_value_     = DEFAULT_LCORBITS;
    config_lc_numpoints_ = DEFAULT_LCPOINTS;
    config_hma_          = DEFAULT_HMA;
    config_hmi_          = DEFAULT_HMI;
    config_step_         = DEFAULT_STEPSIZE;
    config_currentstep_  = DEFAULT_STEPSIZE;
    config_tolerance_    = DEFAULT_TOLERANCE;
    config_projection_   = DEFAULT_PROJECTION;
    config_intpoints_    = DEFAULT_INTPOINTS;
    config_dashes_       = DEFAULT_LINESTYLE;
    config_kindvf_       = DEFAULT_INTCONFIG;
}

// -----------------------------------------------------------------------
//                        WVFStudy DESTRUCTOR
// -----------------------------------------------------------------------
WVFStudy::~WVFStudy() { deleteVF(); }

// -----------------------------------------------------------------------
//                        WVFStudy::deleteVF
// -----------------------------------------------------------------------
void WVFStudy::deleteVF()
{
    delete_term2(f_vec_field_[0]);  delete_term2(f_vec_field_[1]);
    delete_term2(vec_field_U1_[0]); delete_term2(vec_field_U1_[1]);
    delete_term2(vec_field_U2_[0]); delete_term2(vec_field_U2_[1]);
    delete_term2(vec_field_V1_[0]); delete_term2(vec_field_V1_[1]);
    delete_term2(vec_field_V2_[0]); delete_term2(vec_field_V2_[1]);
    delete_term3(vec_field_C_[0]);  delete_term3(vec_field_C_[1]);

    f_vec_field_[0] = f_vec_field_[1] = nullptr;
    vec_field_U1_[0] = vec_field_U1_[1] = nullptr;
    vec_field_U2_[0] = vec_field_U2_[1] = nullptr;
    vec_field_V1_[0] = vec_field_V1_[1] = nullptr;
    vec_field_V2_[0] = vec_field_V2_[1] = nullptr;
    vec_field_C_[0]  = vec_field_C_[1]  = nullptr;

    deleteSaddle(first_saddle_point_);
    deleteSemiElementary(first_se_point_);
    deleteNode(first_node_point_);
    deleteDegenerate(first_de_point_);
    deleteStrongFocus(first_sf_point_);
    deleteWeakFocus(first_wf_point_);

    first_saddle_point_ = nullptr;
    first_se_point_     = nullptr;
    first_node_point_   = nullptr;
    first_sf_point_     = nullptr;
    first_wf_point_     = nullptr;
    first_de_point_     = nullptr;

    delete_term2(gcf_);   delete_term2(gcf_U1_); delete_term2(gcf_U2_);
    delete_term2(gcf_V1_); delete_term2(gcf_V2_); delete_term3(gcf_C_);
    deleteOrbitPoint(gcf_points_);

    gcf_ = gcf_U1_ = gcf_U2_ = gcf_V1_ = gcf_V2_ = nullptr;
    gcf_C_ = nullptr;
    gcf_points_ = nullptr;

    curve_vector_.clear();
    isocline_vector_.clear();

    deleteOrbit(first_orbit_);
    first_orbit_ = nullptr;
    deleteLimitCycle(first_lim_cycle_);
    first_lim_cycle_ = nullptr;

    xmin_ = ymin_ = -1.0;
    xmax_ = ymax_ =  1.0;
    p_ = q_ = 1;
    typeofstudy_ = TYPEOFSTUDY_ALL;
    singinf_ = false;
    dir_vec_field_ = 1;
    lasterror_ = "";
}

// -----------------------------------------------------------------------
//  Deletion helpers
// -----------------------------------------------------------------------
void WVFStudy::deleteTransformations(transformations *t)
{
    transformations *u;
    while (t != nullptr) { u = t; t = t->next_trans; delete u; }
}

void WVFStudy::deleteOrbitPoint(P4ORBIT p)
{
    P4ORBIT q;
    while (p != nullptr) { q = p; p = p->next_point; delete q; }
}

void WVFStudy::deleteOrbit(orbits *p)
{
    orbits *q;
    while (p != nullptr) { q = p; p = p->next_orbit; deleteOrbitPoint(q->f_orbits); delete q; }
}

void WVFStudy::deleteLimitCycle(orbits *p) { deleteOrbit(p); }

void WVFStudy::deleteSeparatrices(sep *p)
{
    sep *q;
    while (p != nullptr) {
        q = p; p = p->next_sep;
        deleteOrbitPoint(q->first_sep_point);
        if (q->notadummy) delete_term1(q->separatrice);
        delete q;
    }
}

void WVFStudy::deleteBlowup(blow_up_points *b)
{
    blow_up_points *c;
    while (b != nullptr) {
        c = b; b = b->next_blow_up_point;
        deleteTransformations(c->trans);
        delete_term2(c->vector_field[0]);
        delete_term2(c->vector_field[1]);
        delete_term1(c->sep);
        deleteOrbitPoint(c->first_sep_point);
        delete c;
    }
}

void WVFStudy::deleteSaddle(saddle *p)
{
    saddle *q;
    while (p != nullptr) {
        q = p; p = p->next_saddle;
        delete_term2(q->vector_field[0]);
        delete_term2(q->vector_field[1]);
        if (q->notadummy) deleteSeparatrices(q->separatrices);
        delete q;
    }
}

void WVFStudy::deleteSemiElementary(semi_elementary *p)
{
    semi_elementary *q;
    while (p != nullptr) {
        q = p; p = p->next_se;
        delete_term2(q->vector_field[0]);
        delete_term2(q->vector_field[1]);
        if (q->notadummy) deleteSeparatrices(q->separatrices);
        delete q;
    }
}

void WVFStudy::deleteNode(node *p)
{
    node *q;
    while (p != nullptr) { q = p; p = p->next_node; delete q; }
}

void WVFStudy::deleteStrongFocus(strong_focus *p)
{
    strong_focus *q;
    while (p != nullptr) { q = p; p = p->next_sf; delete q; }
}

void WVFStudy::deleteWeakFocus(weak_focus *p)
{
    weak_focus *q;
    while (p != nullptr) { q = p; p = p->next_wf; delete q; }
}

void WVFStudy::deleteDegenerate(degenerate *p)
{
    degenerate *q;
    while (p != nullptr) {
        q = p; p = p->next_de;
        if (q->notadummy && q->blow_up != nullptr) deleteBlowup(q->blow_up);
        delete q;
    }
}

// -----------------------------------------------------------------------
//                        WVFStudy::readTables
// -----------------------------------------------------------------------
bool WVFStudy::readTables(const std::string &basename)
{
    FILE *fp;
    int j, flag;

    setlocale(LC_ALL, "C");
    deleteVF();

    // --- vector field table ---
    std::string vecfile = basename + "_vec.tab";
    fp = fopen(vecfile.c_str(), "rt");
    if (fp == nullptr) {
        std::cerr << "[ERROR] Cannot open " << vecfile << "\n";
        deleteVF(); return false;
    }

    if (fscanf(fp, "%d %d %d ", &typeofstudy_, &p_, &q_) != 3) {
        std::cerr << "[ERROR] Cannot read typeofstudy_ in *_vec.tab\n";
        fclose(fp); deleteVF(); return false;
    }

    if (typeofstudy_ == TYPEOFSTUDY_ONE) {
        if (fscanf(fp, "%lf %lf %lf %lf", &xmin_, &xmax_, &ymin_, &ymax_) != 4) {
            std::cerr << "[ERROR] Cannot read min-max coords\n";
            fclose(fp); deleteVF(); return false;
        }
        p_ = q_ = 1;
        typeofview_ = TYPEOFVIEW_PLANE;
    } else {
        typeofview_ = TYPEOFVIEW_SPHERE;
    }

    plweights_         = !(p_ == 1 && q_ == 1);
    double_p_          = (double)p_;
    double_q_          = (double)q_;
    double_p_plus_q_   = (double)(p_ + q_);
    double_p_minus_1_  = (double)(p_ - 1);
    double_q_minus_1_  = (double)(q_ - 1);
    double_q_minus_p_  = (double)(q_ - p_);

    if (!readGCF(fp)) {
        std::cerr << "[ERROR] Cannot read GCF\n";
        fclose(fp); deleteVF(); return false;
    }
    if (!readVectorField(fp, f_vec_field_)
     || !readVectorField(fp, vec_field_U1_)
     || !readVectorField(fp, vec_field_V1_)
     || !readVectorField(fp, vec_field_U2_)
     || !readVectorField(fp, vec_field_V2_)) {
        std::cerr << "[ERROR] Cannot read vector field charts\n";
        fclose(fp); deleteVF(); return false;
    }

    if (plweights_) {
        if (!readVectorFieldCylinder(fp, vec_field_C_)) {
            std::cerr << "[ERROR] Cannot read cylinder chart\n";
            fclose(fp); deleteVF(); return false;
        }
        singinf_ = false;
    } else {
        if (fscanf(fp, "%d %d", &flag, &dir_vec_field_) != 2) {
            std::cerr << "[ERROR] Cannot read singinf/dir flags\n";
            fclose(fp); deleteVF(); return false;
        }
        singinf_ = (flag != 0);
    }
    fclose(fp);

    // --- finite singularities ---
    if (typeofstudy_ != TYPEOFSTUDY_INF) {
        std::string finfile = basename + "_fin.tab";
        fp = fopen(finfile.c_str(), "rt");
        if (fp == nullptr) {
            std::cerr << "[ERROR] Cannot open " << finfile << "\n";
            deleteVF(); return false;
        }
        if (!readPoints(fp)) {
            std::cerr << "[ERROR] Problem reading *_fin.tab: " << lasterror_ << "\n";
            fclose(fp); deleteVF(); return false;
        }
        fclose(fp);
    }

    // --- infinite singularities ---
    if (typeofstudy_ != TYPEOFSTUDY_ONE && typeofstudy_ != TYPEOFSTUDY_FIN) {
        std::string inffile = basename + "_inf.tab";
        fp = fopen(inffile.c_str(), "rt");
        if (fp == nullptr) {
            std::cerr << "[ERROR] Cannot open " << inffile << "\n";
            deleteVF(); return false;
        }
        int iters = (p_ == 1 && q_ == 1) ? 2 : 4;
        for (j = 1; j <= iters; j++) {
            if (!readPoints(fp)) {
                std::cerr << "[ERROR] Cannot read sing in *_inf.tab (" << j
                          << "): " << lasterror_ << "\n";
                fclose(fp); deleteVF(); return false;
            }
        }
        fclose(fp);
    }

    return true;
}

// -----------------------------------------------------------------------
//                        WVFStudy::readGCF
// -----------------------------------------------------------------------
bool WVFStudy::readGCF(FILE *fp)
{
    int N, degree_gcf;

    if (fscanf(fp, "%d", &degree_gcf) != 1) return false;

    if (degree_gcf) {
        if (fscanf(fp, "%d", &N) != 1) return false;
        gcf_ = new term2;
        if (!readTerm2(fp, gcf_, N)) return false;

        if (fscanf(fp, "%d", &N) != 1) return false;
        gcf_U1_ = new term2;
        if (!readTerm2(fp, gcf_U1_, N)) return false;

        if (fscanf(fp, "%d", &N) != 1) return false;
        gcf_U2_ = new term2;
        if (!readTerm2(fp, gcf_U2_, N)) return false;

        if (fscanf(fp, "%d", &N) != 1) return false;
        gcf_V1_ = new term2;
        if (!readTerm2(fp, gcf_V1_, N)) return false;

        if (fscanf(fp, "%d", &N) != 1) return false;
        gcf_V2_ = new term2;
        if (!readTerm2(fp, gcf_V2_, N)) return false;

        if (p_ != 1 || q_ != 1) {
            if (fscanf(fp, "%d", &N) != 1) return false;
            gcf_C_ = new term3;
            if (!readTerm3(fp, gcf_C_, N)) return false;
        }
    } else {
        gcf_ = gcf_U1_ = gcf_U2_ = gcf_V1_ = gcf_V2_ = nullptr;
        gcf_C_ = nullptr;
    }
    return true;
}

// -----------------------------------------------------------------------
//                        WVFStudy::readVectorField
// -----------------------------------------------------------------------
bool WVFStudy::readVectorField(FILE *fp, P4POLYNOM2 *vf)
{
    int M, N;
    vf[0] = new term2;
    vf[1] = new term2;
    if (fscanf(fp, "%d", &M) != 1) return false;
    if (!readTerm2(fp, vf[0], M))  return false;
    if (fscanf(fp, "%d", &N) != 1) return false;
    if (!readTerm2(fp, vf[1], N))  return false;
    return true;
}

// -----------------------------------------------------------------------
//                        WVFStudy::readVectorFieldCylinder
// -----------------------------------------------------------------------
bool WVFStudy::readVectorFieldCylinder(FILE *fp, P4POLYNOM3 *vf)
{
    int N;
    vf[0] = new term3;
    vf[1] = new term3;
    if (fscanf(fp, "%d", &N) != 1) return false;
    if (!readTerm3(fp, vf[0], N))  return false;
    if (fscanf(fp, "%d", &N) != 1) return false;
    if (!readTerm3(fp, vf[1], N))  return false;
    return true;
}

// -----------------------------------------------------------------------
//                        WVFStudy::readPoints
// -----------------------------------------------------------------------
bool WVFStudy::readPoints(FILE *fp)
{
    int N, i, typ;

    if (fscanf(fp, "%d ", &N) != 1) {
        lasterror_ = "#sing not readable";
        return false;
    }

    for (i = 1; i <= N; i++) {
        if (fscanf(fp, "%d ", &typ) != 1) {
            lasterror_ = "sing #" + std::to_string(i) + " type not readable";
            return false;
        }
        bool ok = false;
        switch (typ) {
        case SADDLE:          ok = readSaddlePoint(fp);          break;
        case SEMI_HYPERBOLIC: ok = readSemiElementaryPoint(fp);  break;
        case NODE:            ok = readNodePoint(fp);            break;
        case STRONG_FOCUS:    ok = readStrongFocusPoint(fp);     break;
        case WEAK_FOCUS:      ok = readWeakFocusPoint(fp);       break;
        case NON_ELEMENTARY:  ok = readDegeneratePoint(fp);      break;
        default:
            lasterror_ = "sing #" + std::to_string(i)
                       + " unknown type (" + std::to_string(typ) + ")";
            return false;
        }
        if (!ok) {
            lasterror_ = "sing #" + std::to_string(i) + ": " + lasterror_;
            return false;
        }
    }
    return true;
}

// -----------------------------------------------------------------------
//                        readTerm1 / readTerm2 / readTerm3
// -----------------------------------------------------------------------
bool WVFStudy::readTerm1(FILE *fp, P4POLYNOM1 p, int N)
{
    P4POLYNOM1 _p = p;
    p->next_term1 = nullptr;
    if (fscanf(fp, "%d %lf", &(p->exp), &(p->coeff)) != 2) return false;
    for (int i = 2; i <= N; i++) {
        p->next_term1 = new term1;
        p = p->next_term1;
        p->next_term1 = nullptr;
        if (fscanf(fp, "%d %lf", &(p->exp), &(p->coeff)) != 2) {
            delete_term1(_p->next_term1); _p->next_term1 = nullptr;
            return false;
        }
    }
    return true;
}

bool WVFStudy::readTerm2(FILE *fp, P4POLYNOM2 p, int N)
{
    P4POLYNOM2 _p = p;
    p->next_term2 = nullptr;
    if (fscanf(fp, "%d %d %lf", &(p->exp_x), &(p->exp_y), &(p->coeff)) != 3)
        return false;
    for (int i = 2; i <= N; i++) {
        p->next_term2 = new term2;
        p = p->next_term2;
        p->next_term2 = nullptr;
        if (fscanf(fp, "%d %d %lf", &(p->exp_x), &(p->exp_y), &(p->coeff)) != 3) {
            delete_term2(_p->next_term2); _p->next_term2 = nullptr;
            return false;
        }
    }
    return true;
}

bool WVFStudy::readTerm3(FILE *fp, P4POLYNOM3 p, int N)
{
    P4POLYNOM3 _p = p;
    p->next_term3 = nullptr;
    if (fscanf(fp, "%d %d %d %lf",
               &(p->exp_r), &(p->exp_Co), &(p->exp_Si), &(p->coeff)) != 4)
        return false;
    for (int i = 2; i <= N; i++) {
        p->next_term3 = new term3;
        p = p->next_term3;
        p->next_term3 = nullptr;
        if (fscanf(fp, "%d %d %d %lf",
                   &(p->exp_r), &(p->exp_Co), &(p->exp_Si), &(p->coeff)) != 4) {
            delete_term3(_p->next_term3); _p->next_term3 = nullptr;
            return false;
        }
    }
    return true;
}

// -----------------------------------------------------------------------
//                        WVFStudy::readSaddlePoint
// -----------------------------------------------------------------------
bool WVFStudy::readSaddlePoint(FILE *fp)
{
    int N;
    saddle *last = nullptr;
    saddle *point = first_saddle_point_;
    while (point != nullptr) { last = point; point = point->next_saddle; }

    point = new saddle;
    if (last == nullptr) first_saddle_point_ = point;
    else                 last->next_saddle   = point;

    if (fscanf(fp, "%lf %lf", &point->x0, &point->y0) != 2) return false;
    if (fscanf(fp, "%lf %lf %lf %lf",
               &point->a11, &point->a12, &point->a21, &point->a22) != 4) return false;
    readVectorField(fp, point->vector_field);
    if (fscanf(fp, "%d ", &point->chart) != 1) return false;

    sep *sep1 = new sep;
    point->separatrices = sep1;
    if (fscanf(fp, "%d ", &sep1->type) != 1) return false;
    if (fscanf(fp, "%d ", &N) != 1) return false;
    sep1->notadummy = true;
    sep1->separatrice = new term1;
    readTerm1(fp, sep1->separatrice, N);
    sep1->direction = 1; sep1->d = 0;

    if (point->chart == CHART_R2 || singinf_) {
        sep *sep2 = new sep;
        sep1->next_sep = sep2;
        sep2->type = sep1->type;
        sep2->direction = -1; sep2->d = 0;
        sep2->notadummy = false;
        sep2->separatrice = sep1->separatrice;

        sep1 = sep2->next_sep = new sep;
        if (fscanf(fp, "%d", &sep1->type) != 1) return false;
        if (fscanf(fp, "%d ", &N) != 1) return false;
        sep1->notadummy = true;
        sep1->separatrice = new term1;
        readTerm1(fp, sep1->separatrice, N);
        sep1->direction = 1; sep1->d = 1;

        sep2 = new sep;
        sep1->next_sep = sep2;
        sep2->type = sep1->type;
        sep2->direction = -1; sep2->d = 1;
        sep2->notadummy = false;
        sep2->separatrice = sep1->separatrice;
    }

    if (fscanf(fp, "%lf ", &point->epsilon) != 1) return false;
    point->notadummy = true;

    if (singinf_ && point->chart != CHART_R2) {
        last = point;
        point = new saddle;
        last->next_saddle = point;
        point->next_saddle = nullptr;
        point->x0 = last->x0; point->y0 = 0.0;
        point->chart = (last->chart == CHART_U1) ? CHART_V1 : CHART_V2;
        point->notadummy = false;
        point->separatrices = last->separatrices;
        point->epsilon = last->epsilon;
        point->a11 = last->a11; point->a12 = last->a12;
        point->a21 = last->a21; point->a22 = last->a22;
        point->vector_field[0] = point->vector_field[1] = nullptr;
    }
    return true;
}

// -----------------------------------------------------------------------
//                        WVFStudy::readSemiElementaryPoint
// -----------------------------------------------------------------------
bool WVFStudy::readSemiElementaryPoint(FILE *fp)
{
    int N, s;
    semi_elementary *last = nullptr;
    semi_elementary *point = first_se_point_;
    while (point != nullptr) { last = point; point = point->next_se; }

    point = new semi_elementary;
    if (last == nullptr) first_se_point_ = point;
    else                 last->next_se   = point;
    point->next_se = nullptr;

    if (fscanf(fp, "%lf %lf ", &point->x0, &point->y0) != 2) return false;
    if (fscanf(fp, "%lf %lf %lf %lf ",
               &point->a11, &point->a12, &point->a21, &point->a22) != 4) return false;
    readVectorField(fp, point->vector_field);
    if (fscanf(fp, "%d %d %d", &point->type, &s, &point->chart) != 3) return false;

    sep *sep1 = nullptr;

    // Helper lambda-like inline to read one separatrix term1
    auto readSep1 = [&]() -> bool {
        if (fscanf(fp, "%d ", &N) != 1) return false;
        sep1->notadummy = true;
        sep1->separatrice = new term1;
        return readTerm1(fp, sep1->separatrice, N);
    };

    switch (point->type) {
    case 1:
        if (s && point->chart != CHART_R2 && !singinf_) {
            point->separatrices = nullptr;
        } else {
            point->separatrices = sep1 = new sep;
            sep1->type = s ? OT_CENT_STABLE : OT_UNSTABLE;
            sep1->d = 0;
            if (((p_ == 1) && (q_ == 1) &&
                 ((point->chart == CHART_V1) || (point->chart == CHART_V2))) ||
                (point->chart == CHART_R2 || singinf_))
                sep1->direction = -1;
            else
                sep1->direction = 1;
            if (!readSep1()) return false;
            if (point->chart == CHART_R2 || singinf_) {
                sep1->next_sep = sep1 = new sep;
                sep1->type = OT_UNSTABLE; sep1->d = 1; sep1->direction = 1;
                if (!readSep1()) return false;
                sep1->next_sep = new sep;
                sep1->next_sep->type = OT_UNSTABLE; sep1->next_sep->d = 1;
                sep1->next_sep->direction = -1; sep1->next_sep->notadummy = false;
                sep1->next_sep->separatrice = sep1->separatrice;
            }
        }
        break;

    case 2:
        point->separatrices = sep1 = new sep;
        sep1->type = s ? STYPE_CENUNSTABLE : STYPE_STABLE;
        sep1->d = 0;
        sep1->direction = ((p_==1 && q_==1 &&
                            (point->chart==CHART_V1 || point->chart==CHART_V2))
                          ? -1 : 1);
        if (!readSep1()) return false;
        if (point->chart == CHART_R2 || singinf_) {
            sep1->next_sep = sep1 = new sep;
            sep1->type = STYPE_STABLE; sep1->d = 1; sep1->direction = 1;
            if (!readSep1()) return false;
            sep1->next_sep = new sep;
            sep1->next_sep->type = STYPE_STABLE; sep1->next_sep->d = 1;
            sep1->next_sep->direction = -1; sep1->next_sep->notadummy = false;
            sep1->next_sep->separatrice = sep1->separatrice;
        }
        break;

    case 3:
        point->separatrices = sep1 = new sep;
        sep1->type = s ? STYPE_CENSTABLE : STYPE_UNSTABLE;
        sep1->d = 0;
        sep1->direction = ((p_==1 && q_==1 &&
                            (point->chart==CHART_V1 || point->chart==CHART_V2))
                          ? -1 : 1);
        if (!readSep1()) return false;
        if (point->chart == CHART_R2 || singinf_) {
            sep1->next_sep = sep1 = new sep;
            sep1->type = STYPE_UNSTABLE; sep1->d = 1; sep1->direction = 1;
            if (!readSep1()) return false;
            sep1->next_sep = new sep;
            sep1->next_sep->type = STYPE_UNSTABLE; sep1->next_sep->d = 1;
            sep1->next_sep->direction = -1; sep1->next_sep->notadummy = false;
            sep1->next_sep->separatrice = sep1->separatrice;
        }
        break;

    case 4:
        if (s && point->chart != CHART_R2 && !singinf_) {
            point->separatrices = nullptr;
        } else {
            point->separatrices = sep1 = new sep;
            sep1->type = s ? STYPE_CENUNSTABLE : STYPE_STABLE;
            sep1->d = 0;
            if (((p_==1) && (q_==1) &&
                 (point->chart==CHART_V1 || point->chart==CHART_V2)) ||
                (point->chart == CHART_R2 || singinf_))
                sep1->direction = -1;
            else
                sep1->direction = 1;
            if (!readSep1()) return false;
            if (point->chart == CHART_R2 || singinf_) {
                sep1->next_sep = sep1 = new sep;
                sep1->type = STYPE_STABLE; sep1->d = 1; sep1->direction = 1;
                if (!readSep1()) return false;
                sep1->next_sep = new sep;
                sep1->next_sep->type = STYPE_STABLE; sep1->next_sep->d = 1;
                sep1->next_sep->direction = -1; sep1->next_sep->notadummy = false;
                sep1->next_sep->separatrice = sep1->separatrice;
            }
        }
        break;

    case 6:
        point->separatrices = sep1 = new sep;
        sep1->type = s ? STYPE_CENUNSTABLE : STYPE_STABLE;
        sep1->d = 0;
        sep1->direction = ((p_==1 && q_==1 &&
                            (point->chart==CHART_V1 || point->chart==CHART_V2))
                          ? -1 : 1);
        if (!readSep1()) return false;
        if (point->chart == CHART_R2 || singinf_) {
            sep1->next_sep = new sep;
            sep1->next_sep->type = STYPE_CENUNSTABLE; sep1->next_sep->d = 0;
            sep1->next_sep->direction = -1; sep1->next_sep->notadummy = false;
            sep1->next_sep->separatrice = sep1->separatrice;
            sep1->next_sep->next_sep = sep1 = new sep;
            sep1->type = STYPE_STABLE; sep1->d = 1; sep1->direction = 1;
            if (!readSep1()) return false;
            sep1->next_sep = new sep;
            sep1->next_sep->type = STYPE_STABLE; sep1->next_sep->d = 1;
            sep1->next_sep->direction = -1; sep1->next_sep->notadummy = false;
            sep1->next_sep->separatrice = sep1->separatrice;
        }
        break;

    case 7:
        point->separatrices = sep1 = new sep;
        sep1->type = s ? STYPE_CENSTABLE : STYPE_UNSTABLE;
        sep1->d = 0;
        sep1->direction = ((p_==1 && q_==1 &&
                            (point->chart==CHART_V1 || point->chart==CHART_V2))
                          ? -1 : 1);
        if (!readSep1()) return false;
        if (point->chart == CHART_R2 || singinf_) {
            sep1->next_sep = new sep;
            sep1->next_sep->type = STYPE_CENSTABLE; sep1->next_sep->d = 0;
            sep1->next_sep->direction = -1; sep1->next_sep->notadummy = false;
            sep1->next_sep->separatrice = sep1->separatrice;
            sep1->next_sep->next_sep = sep1 = new sep;
            sep1->type = STYPE_UNSTABLE; sep1->d = 1; sep1->direction = 1;
            if (!readSep1()) return false;
            sep1->next_sep = new sep;
            sep1->next_sep->type = STYPE_UNSTABLE; sep1->next_sep->d = 1;
            sep1->next_sep->direction = -1; sep1->next_sep->notadummy = false;
            sep1->next_sep->separatrice = sep1->separatrice;
        }
        break;

    default:
        point->separatrices = nullptr;
        {
            double y[2] = {point->x0, point->y0};
            bool flip = false;
            switch (point->chart) {
            case CHART_R2: flip = eval_term2(gcf_,    y) < 0; break;
            case CHART_U1: flip = eval_term2(gcf_U1_, y) < 0; break;
            case CHART_V1:
                if (p_==1 && q_==1) y[0] = -y[0];
                flip = eval_term2(gcf_V1_, y) < 0; break;
            case CHART_U2: flip = eval_term2(gcf_U2_, y) < 0; break;
            case CHART_V2:
                if (p_==1 && q_==1) y[0] = -y[0];
                flip = eval_term2(gcf_V2_, y) < 0; break;
            }
            if (flip) point->type = (point->type == 5) ? 8 : 5;
        }
        break;
    }

    if (fscanf(fp, "%lf ", &point->epsilon) != 1) return false;
    point->notadummy = true;

    if (singinf_ && point->chart != CHART_R2) {
        point->next_se = new semi_elementary;
        point->next_se->x0 = point->x0; point->next_se->y0 = 0.0;
        point->next_se->chart = (point->chart==CHART_U1) ? CHART_V1 : CHART_V2;
        point->next_se->type = (dir_vec_field_ == 1) ? point->type :
            (point->type==1?3 : point->type==2?4 : point->type==3?1 :
             point->type==4?2 : point->type==6?7 : point->type==7?6 : point->type);
        point->next_se->notadummy = false;
        point->next_se->separatrices = point->separatrices;
        point->next_se->epsilon = point->epsilon;
        point->next_se->a11 = point->a11; point->next_se->a12 = point->a12;
        point->next_se->a21 = point->a21; point->next_se->a22 = point->a22;
        point->next_se->vector_field[0] = point->next_se->vector_field[1] = nullptr;
        point->next_se->next_se = nullptr;
    }
    return true;
}

// -----------------------------------------------------------------------
//                        WVFStudy::readStrongFocusPoint
// -----------------------------------------------------------------------
bool WVFStudy::readStrongFocusPoint(FILE *fp)
{
    strong_focus *last = nullptr;
    strong_focus *point = first_sf_point_;
    while (point != nullptr) { last = point; point = point->next_sf; }

    point = new strong_focus;
    if (last == nullptr) first_sf_point_ = point;
    else                 last->next_sf   = point;
    point->next_sf = nullptr;

    if (fscanf(fp, "%d %lf %lf ", &point->stable, &point->x0, &point->y0) != 3)
        return false;
    if (fscanf(fp, "%d ", &point->chart) != 1) return false;

    double y[2] = {point->x0, point->y0};
    switch (point->chart) {
    case CHART_R2: if (eval_term2(gcf_,    y) < 0) point->stable *= -1; break;
    case CHART_U1: if (eval_term2(gcf_U1_, y) < 0) point->stable *= -1; break;
    case CHART_V1:
        if (p_==1 && q_==1) y[0]=-y[0];
        if (eval_term2(gcf_V1_, y) < 0) point->stable *= -1; break;
    case CHART_U2: if (eval_term2(gcf_U2_, y) < 0) point->stable *= -1; break;
    case CHART_V2:
        if (p_==1 && q_==1) y[0]=-y[0];
        if (eval_term2(gcf_V2_, y) < 0) point->stable *= -1; break;
    }

    if (singinf_ && point->chart != CHART_R2) {
        last = point;
        point = new strong_focus;
        last->next_sf = point; point->next_sf = nullptr;
        point->x0 = last->x0; point->y0 = 0.0;
        point->chart = (last->chart==CHART_U1) ? CHART_V1 : CHART_V2;
        point->stable = last->stable * ((dir_vec_field_==-1) ? -1 : 1);
    }
    return true;
}

// -----------------------------------------------------------------------
//                        WVFStudy::readWeakFocusPoint
// -----------------------------------------------------------------------
bool WVFStudy::readWeakFocusPoint(FILE *fp)
{
    weak_focus *last = nullptr;
    weak_focus *point = first_wf_point_;
    while (point != nullptr) { last = point; point = point->next_wf; }

    point = new weak_focus;
    if (last == nullptr) first_wf_point_ = point;
    else                 last->next_wf   = point;
    point->next_wf = nullptr;

    if (fscanf(fp, "%lf %lf ", &point->x0, &point->y0) != 2) return false;
    if (fscanf(fp, "%d %d ", &point->type, &point->chart) != 2) return false;

    if (point->type == STYPE_STABLE || point->type == STYPE_UNSTABLE) {
        double y[2] = {point->x0, point->y0};
        switch (point->chart) {
        case CHART_R2: if (eval_term2(gcf_,    y)<0) point->type*=-1; break;
        case CHART_U1: if (eval_term2(gcf_U1_, y)<0) point->type*=-1; break;
        case CHART_V1:
            if (p_==1&&q_==1) y[0]=-y[0];
            if (eval_term2(gcf_V1_, y)<0) point->type*=-1; break;
        case CHART_U2: if (eval_term2(gcf_U2_, y)<0) point->type*=-1; break;
        case CHART_V2:
            if (p_==1&&q_==1) y[0]=-y[0];
            if (eval_term2(gcf_V2_, y)<0) point->type*=-1; break;
        }
    }

    if (singinf_ && point->chart != CHART_R2) {
        point->next_wf = new weak_focus;
        point->next_wf->x0 = point->x0; point->next_wf->y0 = 0.0;
        point->next_wf->chart = (point->chart==CHART_U1) ? CHART_V1 : CHART_V2;
        if (dir_vec_field_ == 1) {
            point->next_wf->type = point->type;
        } else {
            switch (point->type) {
            case STYPE_STABLE:   point->next_wf->type = STYPE_UNSTABLE; break;
            case STYPE_UNSTABLE: point->next_wf->type = STYPE_STABLE;   break;
            default:             point->next_wf->type = point->type;    break;
            }
        }
        point = point->next_wf;
        point->next_wf = nullptr;
    }
    return true;
}

// -----------------------------------------------------------------------
//                        WVFStudy::readDegeneratePoint
// -----------------------------------------------------------------------
bool WVFStudy::readDegeneratePoint(FILE *fp)
{
    int n;
    degenerate *last = nullptr;
    degenerate *point = first_de_point_;
    while (point != nullptr) { last = point; point = point->next_de; }

    point = new degenerate;
    if (last == nullptr) first_de_point_ = point;
    else                 last->next_de   = point;

    if (fscanf(fp, "%lf %lf %lf %d ",
               &point->x0, &point->y0, &point->epsilon, &n) != 4) return false;
    if (n) {
        point->blow_up = new blow_up_points;
        readBlowupPoints(fp, point->blow_up, n);
        point->blow_up->blow_up_vec_field = true;
    }
    if (fscanf(fp, "%d ", &point->chart) != 1) return false;
    point->notadummy = true;

    if (singinf_ && point->chart != CHART_R2) {
        last = point;
        point = new degenerate;
        last->next_de = point;
        point->x0 = last->x0; point->y0 = 0.0;
        point->chart = (last->chart==CHART_U1) ? CHART_V1 : CHART_V2;
        point->epsilon = last->epsilon;
        point->notadummy = false;
        point->blow_up = last->blow_up;
    }
    return true;
}

// -----------------------------------------------------------------------
//                        WVFStudy::readNodePoint
// -----------------------------------------------------------------------
bool WVFStudy::readNodePoint(FILE *fp)
{
    node *last = nullptr;
    node *point = first_node_point_;
    while (point != nullptr) { last = point; point = point->next_node; }

    point = new node;
    if (last == nullptr) first_node_point_ = point;
    else                 last->next_node   = point;

    if (fscanf(fp, "%lf %lf %d ", &point->x0, &point->y0, &point->stable) != 3)
        return false;
    if (fscanf(fp, "%d ", &point->chart) != 1) return false;

    double y[2] = {point->x0, point->y0};
    switch (point->chart) {
    case CHART_R2: if (eval_term2(gcf_,    y)<0) point->stable*=-1; break;
    case CHART_U1: if (eval_term2(gcf_U1_, y)<0) point->stable*=-1; break;
    case CHART_V1:
        if (p_==1&&q_==1) y[0]=-y[0];
        if (eval_term2(gcf_V1_, y)<0) point->stable*=-1; break;
    case CHART_U2: if (eval_term2(gcf_U2_, y)<0) point->stable*=-1; break;
    case CHART_V2:
        if (p_==1&&q_==1) y[0]=-y[0];
        if (eval_term2(gcf_V2_, y)<0) point->stable*=-1; break;
    }

    if (singinf_ && point->chart != CHART_R2) {
        last = point;
        point = new node;
        last->next_node = point; point->next_node = nullptr;
        point->x0 = last->x0; point->y0 = 0.0;
        point->chart = (last->chart==CHART_U1) ? CHART_V1 : CHART_V2;
        point->stable = (last->stable && dir_vec_field_==-1) ? -1 : 1;
    }
    return true;
}

// -----------------------------------------------------------------------
//                        WVFStudy::readTransformations
// -----------------------------------------------------------------------
bool WVFStudy::readTransformations(FILE *fp, transformations *trans, int n)
{
    if (fscanf(fp, "%lf %lf %d %d %d %d %d %d %d",
               &trans->x0, &trans->y0, &trans->c1, &trans->c2,
               &trans->d1, &trans->d2, &trans->d3, &trans->d4, &trans->d) != 9)
        return false;
    trans->next_trans = nullptr;

    for (int i = 2; i <= n; i++) {
        trans->next_trans = new transformations;
        trans = trans->next_trans;
        if (fscanf(fp, "%lf %lf %d %d %d %d %d %d %d",
                   &trans->x0, &trans->y0, &trans->c1, &trans->c2,
                   &trans->d1, &trans->d2, &trans->d3, &trans->d4, &trans->d) != 9)
            return false;
        trans->next_trans = nullptr;
    }
    return true;
}

// -----------------------------------------------------------------------
//                        WVFStudy::readBlowupPoints
// -----------------------------------------------------------------------
bool WVFStudy::readBlowupPoints(FILE *fp, blow_up_points *b, int n)
{
    for (int i = 1; i <= n; i++) {
        if (fscanf(fp, "%d ", &b->n) != 1) return false;
        b->trans = new transformations;
        readTransformations(fp, b->trans, b->n);
        if (fscanf(fp, "%lf %lf ", &b->x0, &b->y0) != 2) return false;
        if (fscanf(fp, "%lf %lf %lf %lf ", &b->a11, &b->a12, &b->a21, &b->a22) != 4)
            return false;
        readVectorField(fp, b->vector_field);
        b->sep = new term1;
        int N;
        if (fscanf(fp, "%d ", &N) != 1) return false;
        readTerm1(fp, b->sep, N);

        int typ;
        if (fscanf(fp, "%d ", &typ) != 1) return false;
        switch (typ) {
        case 1: case 3: case 9:  case 10: case 13: case 14: b->type = STYPE_UNSTABLE;    break;
        case 2: case 4: case 11: case 12: case 15: case 16: b->type = STYPE_STABLE;      break;
        case 5: case 7: b->type = STYPE_CENUNSTABLE; break;
        case 6: case 8: b->type = STYPE_CENSTABLE;   break;
        default:        b->type = STYPE_UNSTABLE;     break;
        }
        b->first_sep_point = b->last_sep_point = nullptr;
        b->next_blow_up_point = nullptr;

        if (i < n) {
            b->next_blow_up_point = new blow_up_points;
            b = b->next_blow_up_point;
        }
    }
    return true;
}
