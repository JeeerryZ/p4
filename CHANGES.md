### Features

- Save prompts for file location; new app icon for windows/macos
- Optional .app/dmg creation step in install_mac.sh

### Fixes

- Cap dock width to fix squashed sphere; prettify dock panel
- Crash on closing plot window (use-after-delete in undo stack)
- Don't auto-expand maple log; ensure exe icon resource rebuilds
- Re-converge plot window aspect ratio after dock-driven sphere resize
- Auto-load vector field on browse/recent file; fix window jumping to (0,0)
- Browse button styling, enable orbit forward/backward+continue together
- Scale initial plot window size to available screen space
- macOS browse button, save path, and sumtable path issues
- Bootstrap rosetta homebrew for x86_64 cross-build
- Grant contents:write permission for release creation
- Pick GMP/MPFR/Qt homebrew prefix matching target architecture
- Resolve windows GMP/MPFR linking and wire up release changelogs
- Run git-cliff on a linux job, not directly on macos/windows

### CI

- Add intel (x86_64) macOS build to matrix alongside arm64
- Generate P4.icns and set cfbundleiconfile in macOS workflow
- Add git-cliff config for release notes generation
- Trigger macOS build on bare semver tags, not v-prefixed
- Attach macOS dmg to release for any tag, not just v-prefixed
- Cross-build macOS intel via rosetta 2 on arm64 runner
- Verify build architecture after compile step
- Add windows build workflow, attach zip to releases
- Drop unused make package, use forward slashes consistently

### Documentation

- Add release automation design doc
- Add release automation implementation plan
- Fix stale v-prefixed tag example in comment

### Chore

- Bump version to 7.2.0
