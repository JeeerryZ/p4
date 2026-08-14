# P4 7.3.0

*August 2026*

A bug-fix and interface release. The most important change is that vector
fields with more than four parameters are now evaluated with the values you
actually entered, and that weak-focus studies no longer fail on a fresh
installation.

---

## Fixed

### Parameters beyond the fourth were silently lost

The parameter panel shows four rows at a time and kept the rest in an internal
copy. Loading a vector field refreshed only the visible rows, so parameters 5
and up were replaced by the values left over from the previously opened file —
usually empty. Scrolling down showed them as blank, and, worse, the evaluation
ran with those wrong values without any warning.

If you work with more than four parameters, results produced with earlier
versions are worth re-checking.

### "Error (readlyapunovresult) invalid subscript selector"

Studying a weak focus failed with this message from Maple. The `lyapunov`
helper stores summation tables in a `sumtables` directory next to the program
and creates them the first time it runs; that directory was missing from every
package, so the helper stopped before writing its result and Maple read an
empty file. It is now included in the Windows, macOS and Linux packages.

The arbitrary-precision helper (`lyapunov_mpf`, used when you raise the
precision) could not start at all on Windows for a related reason, producing
exactly the same error. It is now self-contained.

### Crashes

- **Clicking outside the disc.** Every click on the plot window outside the
  Poincaré disc corrupted the program's memory. The damage usually surfaced
  later and elsewhere — most often when leaving the plot window — which made it
  look unrelated to clicking.
- **Selecting a zoom region or a limit-cycle section** had the same defect.
- **Batch mode** (`p4 -x`) crashed as soon as Maple finished, and then, once
  that was fixed, kept running instead of exiting. It now completes and exits
  cleanly.

### Malformed input files

A vector field file with a very long line could overwrite memory while being
read. All the affected reads are now bounded.

---

## Changed

### Plot window

The two toolbars are gone. The side panel with the eight control tabs is now
the only place controls live, and the menu bar carries what has no tab of its
own:

| Menu | Contains |
|---|---|
| File | Print, Close |
| Edit | Undo, Redo |
| Plot | Refresh, Legend, Plot All Separatrices |
| Panels | the eight tabs, so their keyboard shortcuts keep working |

Print and Close now use the standard shortcuts (`Ctrl+P`, `Ctrl+W`).

### The phase portrait stays circular

Resizing the window used to stretch the Poincaré disc into an ellipse, and the
program compensated by resizing the window itself until the drawing area was
square again — so the window fought you whenever you tried to reshape it.

The drawing now uses the same scale on both axes and centres the result, so the
disc is circular at any window size and the window keeps the shape you give it.
This also removes a calculation based on the screen's reported physical size,
which is unreliable on high-resolution displays and made the behaviour differ
between Windows and macOS.

### Layout

The control panel is wider and easier to read, and the plot window opens
shorter than it is wide, leaving more room for the portrait itself.

---

## Packaging

- The `sumtables` directory ships with the macOS disk image and the Windows
  archive.
- The macOS application bundle reports the real version number instead of a
  fixed one.
- The Windows install script now stops with a clear message if it cannot
  replace a running copy of the program, instead of reporting success and
  leaving the previous build in place.

---

## Downloads

| Platform | File |
|---|---|
| macOS (Apple Silicon) | `P4-macOS-arm64.dmg` |
| macOS (Intel) | `P4-macOS-x86_64.dmg` |
| Windows (64-bit) | `P4-windows-x86_64.zip` |

Maple is required at run time and is not included.
