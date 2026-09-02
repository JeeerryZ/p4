### Features

* Plot window keeps only the docked control panel; the menu bar now carries
File, Edit, Plot and Panels, and the toolbars are gone
* Phase portrait stays circular at any window size, so the window no longer
resizes itself to keep the drawing area square
* Wider control panel and a plot window that opens shorter than it is wide

### Fixes

* Parameters beyond the fourth were replaced by the previously loaded file's
values on load, and evaluated with those wrong values
* Weak-focus study failed with "readlyapunovresult: invalid subscript
selector": the sumtables directory was missing
* lyapunov\_mpf could not start on Windows, giving the same error whenever the
precision was raised
* Heap corruption on every click outside the disc, and on selecting a zoom
region or a limit-cycle section
* Reading a vector field file with a very long line could overwrite memory
* Windows install script reported success when it could not replace a running
p4.exe, silently keeping the previous build

### Packaging

* The Windows archive was missing every runtime library that is not part of
Qt itself, so p4.exe stopped with "libstdc++-6.dll was not found" and could
never start. The whole set is now bundled, and the build fails if any
library is missing instead of publishing a package that cannot run
* New Windows installer, `P4-windows-x86_64-setup.exe`: Start menu entry,
uninstaller, and write access to the sumtables folder
* Ship the sumtables directory in the macOS disk image and the Windows archive (was causing some erros while computing vector fields)
* lyapunov\_mpf inside the macOS bundle still pointed at Homebrew and could
not start on a Mac without it, giving the same weak-focus error
* Read the macOS bundle version from version.h instead of a hardcoded value
* Intel macOS builds are no longer produced automatically — build from
source with install\_mac.sh

