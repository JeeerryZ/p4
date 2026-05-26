#ifndef P4_UNDO_H
#define P4_UNDO_H

#include "file_tab.h"
#include "win_sphere.h"

#include <QUndoCommand>

orbits *deepCopyOrbit(const orbits *src);

class AddOrbitCmd : public QUndoCommand
{
  public:
    AddOrbitCmd(QWinSphere *sphere, double x0, double y0, int dir);
    void redo() override;
    void undo() override;

  private:
    QWinSphere *sphere_;
    double x0_, y0_;
    int dir_;
};

class DelLastOrbitCmd : public QUndoCommand
{
  public:
    explicit DelLastOrbitCmd(QWinSphere *sphere);
    ~DelLastOrbitCmd() override;
    void redo() override;
    void undo() override;

  private:
    QWinSphere *sphere_;
    orbits *saved_;
};

#endif // P4_UNDO_H
