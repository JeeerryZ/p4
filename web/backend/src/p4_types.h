#pragma once

// Polynomial linked-list types shared between file_tab and math_polynom.

struct term1 {
    int    exp;
    double coeff;
    struct term1 *next_term1;
    term1() : exp(0), coeff(0.0), next_term1(nullptr) {}
};
typedef struct term1 *P4POLYNOM1;

struct term2 {
    int    exp_x;
    int    exp_y;
    double coeff;
    struct term2 *next_term2;
    term2() : exp_x(0), exp_y(0), coeff(0.0), next_term2(nullptr) {}
};
typedef struct term2 *P4POLYNOM2;

struct term3 {
    int    exp_r;
    int    exp_Co;
    int    exp_Si;
    double coeff;
    struct term3 *next_term3;
    term3() : exp_r(0), exp_Co(0), exp_Si(0), coeff(0.0), next_term3(nullptr) {}
};
typedef struct term3 *P4POLYNOM3;
