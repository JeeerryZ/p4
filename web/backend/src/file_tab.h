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
 *  Ported for the P4 REST backend: Qt/Wt dependencies removed.
 */

#pragma once

#include <cstdio>
#include <string>
#include <vector>

// -----------------------------------------------------------------------
//  Constants (inlined from custom.h / file_vf.h)
// -----------------------------------------------------------------------

#define TYPEOFSTUDY_ALL 0
#define TYPEOFSTUDY_FIN 1
#define TYPEOFSTUDY_INF 2
#define TYPEOFSTUDY_ONE 3

#define CHART_R2 0
#define CHART_U1 1
#define CHART_U2 2
#define CHART_V1 3
#define CHART_V2 4

#define SADDLE          1
#define NODE            2
#define WEAK_FOCUS      3
#define STRONG_FOCUS    4
#define SEMI_HYPERBOLIC 5
#define NON_ELEMENTARY  6

#define STYPE_UNSTABLE    (1)
#define STYPE_STABLE      (-1)
#define STYPE_CENUNSTABLE 2
#define STYPE_CENSTABLE   (-2)
#define STYPE_ORBIT       3

#define FOCUSTYPE_UNKNOWN  0
#define FOCUSTYPE_UNSTABLE (1)
#define FOCUSTYPE_STABLE   (-1)
#define FOCUSTYPE_CENTER   4

#define OT_STABLE        STYPE_STABLE
#define OT_UNSTABLE      STYPE_UNSTABLE
#define OT_CENT_STABLE   STYPE_CENSTABLE
#define OT_CENT_UNSTABLE STYPE_CENUNSTABLE
#define OT_ORBIT         STYPE_ORBIT

#define LINESTYLE_DASHES 1
#define LINESTYLE_POINTS 0

#define DEFAULT_HMA       1.E-1
#define DEFAULT_HMI       1.E-6
#define DEFAULT_STEPSIZE  0.01
#define DEFAULT_PROJECTION -1.0
#define DEFAULT_TOLERANCE 1.E-8
#define DEFAULT_INTPOINTS 200
#define DEFAULT_LCORBITS  1
#define DEFAULT_LCPOINTS  2000
#define DEFAULT_LINESTYLE LINESTYLE_DASHES
#define DEFAULT_INTCONFIG 1   // INTCONFIG_ORIGINAL

enum TYPEOFVIEWS {
    TYPEOFVIEW_PLANE  = 0,
    TYPEOFVIEW_SPHERE = 1,
    TYPEOFVIEW_U1     = 2,
    TYPEOFVIEW_U2     = 3,
    TYPEOFVIEW_V1     = 4,
    TYPEOFVIEW_V2     = 5
};

// -----------------------------------------------------------------------
//  Polynomial linked-list types
// -----------------------------------------------------------------------

#include "p4_types.h"

// -----------------------------------------------------------------------
//  Orbit structures
// -----------------------------------------------------------------------

struct orbits_points {
    int    color;
    double pcoord[3];
    int    dashes;
    int    dir;
    int    type;
    struct orbits_points *next_point;
    orbits_points() : color(0), dashes(0), dir(0), type(0), next_point(nullptr)
    { pcoord[0]=pcoord[1]=pcoord[2]=0.0; }
};
typedef struct orbits_points *P4ORBIT;

struct orbits {
    double  pcoord[3];
    int     color;
    P4ORBIT f_orbits;
    P4ORBIT current_f_orbits;
    struct  orbits *next_orbit;
    orbits() : color(0), f_orbits(nullptr), current_f_orbits(nullptr),
               next_orbit(nullptr) { pcoord[0]=pcoord[1]=pcoord[2]=0.0; }
};

// -----------------------------------------------------------------------
//  Curves / isoclines
// -----------------------------------------------------------------------

struct curves {
    P4POLYNOM2 r2, u1, u2, v1, v2;
    P4POLYNOM3 c;
    P4ORBIT    points;
    curves() : r2(nullptr), u1(nullptr), u2(nullptr), v1(nullptr), v2(nullptr),
               c(nullptr), points(nullptr) {}
};

struct isoclines {
    P4POLYNOM2 r2, u1, u2, v1, v2;
    P4POLYNOM3 c;
    P4ORBIT    points;
    int        color;
    isoclines() : r2(nullptr), u1(nullptr), u2(nullptr), v1(nullptr),
                  v2(nullptr), c(nullptr), points(nullptr), color(0) {}
};

// -----------------------------------------------------------------------
//  Blow-up
// -----------------------------------------------------------------------

struct transformations {
    double x0, y0;
    int    c1, c2, d1, d2, d3, d4, d;
    struct transformations *next_trans;
    transformations() : x0(0), y0(0), c1(0), c2(0), d1(0), d2(0), d3(0),
                        d4(0), d(0), next_trans(nullptr) {}
};

struct blow_up_points {
    int    n;
    struct transformations    *trans;
    double x0, y0;
    double a11, a12, a21, a22;
    struct term2              *vector_field[2];
    struct term1              *sep;
    int    type;
    int    blow_up_vec_field;
    double point[2];
    struct orbits_points      *first_sep_point;
    struct orbits_points      *last_sep_point;
    struct blow_up_points     *next_blow_up_point;
    blow_up_points()
        : n(0), trans(nullptr), x0(0), y0(0),
          a11(0), a12(0), a21(0), a22(0), sep(nullptr), type(0),
          blow_up_vec_field(0), first_sep_point(nullptr),
          last_sep_point(nullptr), next_blow_up_point(nullptr)
    { vector_field[0]=vector_field[1]=nullptr; point[0]=point[1]=0.0; }
};

struct sep {
    struct orbits_points *first_sep_point;
    struct orbits_points *last_sep_point;
    int    type;
    int    direction;
    int    d;
    bool   notadummy;
    struct term1 *separatrice;
    struct sep   *next_sep;
    sep() : first_sep_point(nullptr), last_sep_point(nullptr),
            type(0), direction(0), d(0), notadummy(false),
            separatrice(nullptr), next_sep(nullptr) {}
};

// -----------------------------------------------------------------------
//  Singularity types
// -----------------------------------------------------------------------

struct genericsingularity {
    double x0, y0;
    struct genericsingularity *next;
    int    chart;
    genericsingularity() : x0(0), y0(0), next(nullptr), chart(0) {}
};

struct saddle {
    double x0, y0;
    struct saddle *next_saddle;
    int    chart;
    double epsilon;
    bool   notadummy;
    struct sep   *separatrices;
    struct term2 *vector_field[2];
    double a11, a12, a21, a22;
    saddle() : x0(0), y0(0), next_saddle(nullptr), chart(0),
               epsilon(0), notadummy(false), separatrices(nullptr),
               a11(0), a12(0), a21(0), a22(0)
    { vector_field[0]=vector_field[1]=nullptr; }
};

struct semi_elementary {
    double x0, y0;
    struct semi_elementary *next_se;
    int    chart;
    double epsilon;
    bool   notadummy;
    struct sep   *separatrices;
    struct term2 *vector_field[2];
    double a11, a12, a21, a22;
    int    type;
    semi_elementary() : x0(0), y0(0), next_se(nullptr), chart(0),
                        epsilon(0), notadummy(false), separatrices(nullptr),
                        a11(0), a12(0), a21(0), a22(0), type(0)
    { vector_field[0]=vector_field[1]=nullptr; }
};

struct degenerate {
    double x0, y0;
    struct degenerate     *next_de;
    int    chart;
    double epsilon;
    bool   notadummy;
    struct blow_up_points *blow_up;
    degenerate() : x0(0), y0(0), next_de(nullptr), chart(0),
                   epsilon(0), notadummy(false), blow_up(nullptr) {}
};

struct node {
    double x0, y0;
    struct node *next_node;
    int    chart;
    int    stable;
    node() : x0(0), y0(0), next_node(nullptr), chart(0), stable(0) {}
};

struct strong_focus {
    double x0, y0;
    struct strong_focus *next_sf;
    int    chart;
    int    stable;
    strong_focus() : x0(0), y0(0), next_sf(nullptr), chart(0), stable(0) {}
};

struct weak_focus {
    double x0, y0;
    struct weak_focus *next_wf;
    int    chart;
    int    type;
    weak_focus() : x0(0), y0(0), next_wf(nullptr), chart(0), type(0) {}
};

// -----------------------------------------------------------------------
//  WVFStudy — I/O-only port of QVFStudy (no Qt, no coordinate transforms)
// -----------------------------------------------------------------------

class WVFStudy
{
public:
    WVFStudy();
    ~WVFStudy();

    // General info
    int         typeofstudy_;
    TYPEOFVIEWS typeofview_;
    int         p_, q_;
    bool        plweights_;
    double      double_p_, double_q_, double_p_plus_q_;
    double      double_p_minus_1_, double_q_minus_1_, double_q_minus_p_;
    double      xmin_, xmax_, ymin_, ymax_;
    bool        singinf_;
    int         dir_vec_field_;

    std::string lasterror_;

    // Vector fields in various charts
    P4POLYNOM2 f_vec_field_[2];
    P4POLYNOM2 vec_field_U1_[2];
    P4POLYNOM2 vec_field_U2_[2];
    P4POLYNOM2 vec_field_V1_[2];
    P4POLYNOM2 vec_field_V2_[2];
    P4POLYNOM3 vec_field_C_[2];

    // Singular points
    saddle          *first_saddle_point_;
    semi_elementary *first_se_point_;
    node            *first_node_point_;
    strong_focus    *first_sf_point_;
    weak_focus      *first_wf_point_;
    degenerate      *first_de_point_;

    // GCF
    P4POLYNOM2 gcf_;
    P4POLYNOM2 gcf_U1_, gcf_U2_, gcf_V1_, gcf_V2_;
    P4POLYNOM3 gcf_C_;
    P4ORBIT    gcf_points_;

    // Curves / isoclines
    std::vector<curves>   curve_vector_;
    std::vector<isoclines> isocline_vector_;

    // Orbits / limit cycles (not populated for the REST backend)
    orbits *first_lim_cycle_;
    orbits *first_orbit_;

    // Config
    int    config_lc_value_;
    double config_hma_, config_hmi_, config_step_, config_currentstep_;
    double config_tolerance_, config_projection_;
    int    config_intpoints_, config_lc_numpoints_;
    bool   config_dashes_, config_kindvf_;

    // ---- I/O methods ----
    bool readTables(const std::string &basename);
    bool readGCF(FILE *fp);
    bool readVectorField(FILE *fp, P4POLYNOM2 *vf);
    bool readVectorFieldCylinder(FILE *fp, P4POLYNOM3 *vf);
    bool readPoints(FILE *fp);
    bool readTerm1(FILE *fp, P4POLYNOM1 p, int N);
    bool readTerm2(FILE *fp, P4POLYNOM2 p, int N);
    bool readTerm3(FILE *fp, P4POLYNOM3 p, int N);
    bool readSaddlePoint(FILE *fp);
    bool readSemiElementaryPoint(FILE *fp);
    bool readStrongFocusPoint(FILE *fp);
    bool readWeakFocusPoint(FILE *fp);
    bool readDegeneratePoint(FILE *fp);
    bool readNodePoint(FILE *fp);
    bool readBlowupPoints(FILE *fp, blow_up_points *b, int n);
    bool readTransformations(FILE *fp, transformations *trans, int n);

    // ---- Cleanup ----
    void deleteVF();
    void deleteSaddle(saddle *p);
    void deleteSemiElementary(semi_elementary *p);
    void deleteNode(node *p);
    void deleteStrongFocus(strong_focus *p);
    void deleteWeakFocus(weak_focus *p);
    void deleteDegenerate(degenerate *p);
    void deleteSeparatrices(sep *p);
    void deleteBlowup(blow_up_points *b);
    void deleteTransformations(transformations *t);
    void deleteOrbitPoint(P4ORBIT p);
    void deleteOrbit(orbits *p);
    void deleteLimitCycle(orbits *p);
};
