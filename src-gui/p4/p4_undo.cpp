#include "p4_undo.h"

#include "file_vf.h"
#include "math_orbits.h"

// -----------------------------------------------------------------------
//                          Deep-copy helpers
// -----------------------------------------------------------------------

static orbits_points *deepCopyPoints(const orbits_points *src)
{
    orbits_points *head = nullptr;
    orbits_points *tail = nullptr;
    while (src) {
        orbits_points *p = new orbits_points;
        p->color = src->color;
        p->pcoord[0] = src->pcoord[0];
        p->pcoord[1] = src->pcoord[1];
        p->pcoord[2] = src->pcoord[2];
        p->dashes = src->dashes;
        p->dir = src->dir;
        p->type = src->type;
        p->next_point = nullptr;
        if (!head) head = p;
        if (tail) tail->next_point = p;
        tail = p;
        src = src->next_point;
    }
    return head;
}

orbits *deepCopyOrbit(const orbits *src)
{
    if (!src) return nullptr;
    orbits *o = new orbits;
    o->pcoord[0] = src->pcoord[0];
    o->pcoord[1] = src->pcoord[1];
    o->pcoord[2] = src->pcoord[2];
    o->color = src->color;
    o->f_orbits = deepCopyPoints(src->f_orbits);
    o->current_f_orbits = o->f_orbits;
    o->next_orbit = nullptr;
    return o;
}

// -----------------------------------------------------------------------
//                          AddOrbitCmd
// -----------------------------------------------------------------------

AddOrbitCmd::AddOrbitCmd(QWinSphere *sphere, double x0, double y0, int dir)
    : QUndoCommand("Add Orbit"), sphere_(sphere), x0_(x0), y0_(y0), dir_(dir)
{
}

void AddOrbitCmd::redo()
{
    sphere_->prepareDrawing();
    bool ok = startOrbit(sphere_, x0_, y0_, true);
    if (!ok) {
        sphere_->finishDrawing();
        setObsolete(true);
        return;
    }
    integrateOrbit(sphere_, dir_);
    sphere_->finishDrawing();
}

void AddOrbitCmd::undo()
{
    sphere_->prepareDrawing();
    deleteLastOrbit(sphere_);
    sphere_->finishDrawing();
}

// -----------------------------------------------------------------------
//                          DelLastOrbitCmd
// -----------------------------------------------------------------------

DelLastOrbitCmd::DelLastOrbitCmd(QWinSphere *sphere)
    : QUndoCommand("Delete Last Orbit"), sphere_(sphere), saved_(nullptr)
{
}

DelLastOrbitCmd::~DelLastOrbitCmd()
{
    if (saved_) {
        g_VFResults.deleteOrbitPoint(saved_->f_orbits);
        delete saved_;
    }
}

void DelLastOrbitCmd::redo()
{
    if (saved_) {
        g_VFResults.deleteOrbitPoint(saved_->f_orbits);
        delete saved_;
        saved_ = nullptr;
    }
    saved_ = deepCopyOrbit(g_VFResults.current_orbit_);

    sphere_->prepareDrawing();
    deleteLastOrbit(sphere_);
    sphere_->finishDrawing();
}

void DelLastOrbitCmd::undo()
{
    if (!saved_) return;

    if (!g_VFResults.first_orbit_) {
        g_VFResults.first_orbit_ = saved_;
    } else {
        orbits *last = g_VFResults.first_orbit_;
        while (last->next_orbit) last = last->next_orbit;
        last->next_orbit = saved_;
    }
    g_VFResults.current_orbit_ = saved_;
    saved_ = nullptr;

    sphere_->prepareDrawing();
    drawOrbit(sphere_, g_VFResults.current_orbit_->pcoord,
              g_VFResults.current_orbit_->f_orbits,
              g_VFResults.current_orbit_->color);
    sphere_->finishDrawing();
}
