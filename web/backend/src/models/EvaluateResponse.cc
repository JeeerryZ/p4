#include "EvaluateResponse.h"

// -----------------------------------------------------------------------
//  Helpers
// -----------------------------------------------------------------------
static crow::json::wvalue serializePoly2(P4POLYNOM2 p)
{
    auto terms = crow::json::wvalue::list();
    while (p) {
        crow::json::wvalue t;
        t["coeff"] = p->coeff;
        t["exp_x"] = p->exp_x;
        t["exp_y"] = p->exp_y;
        terms.push_back(std::move(t));
        p = p->next_term2;
    }
    return crow::json::wvalue(std::move(terms));
}

static const char *chartName(int chart)
{
    switch (chart) {
    case CHART_R2: return "R2";
    case CHART_U1: return "U1";
    case CHART_U2: return "U2";
    case CHART_V1: return "V1";
    case CHART_V2: return "V2";
    default:       return "unknown";
    }
}

// -----------------------------------------------------------------------
//  buildResponse
// -----------------------------------------------------------------------
crow::json::wvalue buildResponse(const WVFStudy &study,
                                 const std::string &rawOutput)
{
    crow::json::wvalue resp;

    // --- finite singularities ---
    auto finite_list   = crow::json::wvalue::list();
    auto infinite_list = crow::json::wvalue::list();

    auto addToList = [&](crow::json::wvalue obj, int chart) {
        if (chart == CHART_R2)
            finite_list.push_back(std::move(obj));
        else
            infinite_list.push_back(std::move(obj));
    };

    // saddles
    for (saddle *sp = study.first_saddle_point_; sp; sp = sp->next_saddle) {
        crow::json::wvalue s;
        s["type"]      = "saddle";
        s["x0"]        = sp->x0;
        s["y0"]        = sp->y0;
        s["chart"]     = chartName(sp->chart);
        s["notadummy"] = sp->notadummy;
        addToList(std::move(s), sp->chart);
    }

    // nodes
    for (node *np = study.first_node_point_; np; np = np->next_node) {
        crow::json::wvalue n;
        n["type"]   = "node";
        n["x0"]     = np->x0;
        n["y0"]     = np->y0;
        n["chart"]  = chartName(np->chart);
        n["stable"] = np->stable;
        addToList(std::move(n), np->chart);
    }

    // strong foci
    for (strong_focus *sfp = study.first_sf_point_; sfp; sfp = sfp->next_sf) {
        crow::json::wvalue sf;
        sf["type"]   = "strong_focus";
        sf["x0"]     = sfp->x0;
        sf["y0"]     = sfp->y0;
        sf["chart"]  = chartName(sfp->chart);
        sf["stable"] = sfp->stable;
        addToList(std::move(sf), sfp->chart);
    }

    // weak foci
    for (weak_focus *wfp = study.first_wf_point_; wfp; wfp = wfp->next_wf) {
        crow::json::wvalue wf;
        wf["type"]  = "weak_focus";
        wf["x0"]    = wfp->x0;
        wf["y0"]    = wfp->y0;
        wf["chart"] = chartName(wfp->chart);
        wf["wtype"] = wfp->type;
        addToList(std::move(wf), wfp->chart);
    }

    // semi-elementary
    for (semi_elementary *sep = study.first_se_point_; sep; sep = sep->next_se) {
        crow::json::wvalue se;
        se["type"]     = "semi_elementary";
        se["x0"]       = sep->x0;
        se["y0"]       = sep->y0;
        se["chart"]    = chartName(sep->chart);
        se["setype"]   = sep->type;
        se["notadummy"]= sep->notadummy;
        addToList(std::move(se), sep->chart);
    }

    // degenerate
    for (degenerate *dp = study.first_de_point_; dp; dp = dp->next_de) {
        crow::json::wvalue de;
        de["type"]     = "degenerate";
        de["x0"]       = dp->x0;
        de["y0"]       = dp->y0;
        de["chart"]    = chartName(dp->chart);
        de["notadummy"]= dp->notadummy;
        addToList(std::move(de), dp->chart);
    }

    resp["finite_singularities"]   = crow::json::wvalue(std::move(finite_list));
    resp["infinite_singularities"] = crow::json::wvalue(std::move(infinite_list));

    // --- vector field (R2 chart) ---
    auto vf = crow::json::wvalue::list();
    vf.push_back(serializePoly2(study.f_vec_field_[0]));
    vf.push_back(serializePoly2(study.f_vec_field_[1]));
    resp["vec_field"] = crow::json::wvalue(std::move(vf));

    // --- metadata ---
    resp["p"]              = study.p_;
    resp["q"]              = study.q_;
    resp["typeofstudy"]    = study.typeofstudy_;
    resp["singinf"]        = study.singinf_;
    resp["raw_output"]     = rawOutput;

    return resp;
}

// -----------------------------------------------------------------------
//  buildError
// -----------------------------------------------------------------------
crow::json::wvalue buildError(const std::string &message)
{
    crow::json::wvalue err;
    err["error"] = message;
    return err;
}
