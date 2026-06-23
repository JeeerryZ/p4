# Release Automation Design

## Goal

Stop hand-writing a changelog `.md` file for every tag. Replace it with a chat-driven
release flow: Claude computes the next version + changelog from conventional commits,
you confirm, Claude tags and pushes, CI builds artifacts for macOS (arm64 + x86_64) and
Windows and attaches them to a GitHub Release with auto-generated notes.

## Background

- One existing tag: `7.1.0` (no `v` prefix), pointing at `fed0d1e` — 8 unreleased commits
  sit on top of it today.
- `src-gui/version.h` hardcodes `VERSION`/`VERSIONDATE`, shown in the About dialog.
- `.github/workflows/build-macos.yml` triggers on `tags: ['v*']` and builds DMGs for
  `macos-14` (arm64) and `macos-13` (Intel x86_64). GitHub is retiring Intel macOS
  runners, so the `macos-13` matrix entry will stop resolving.
- No CI build exists for Windows; Windows builds are local-only via `install_win.ps1`.
- Commits already follow a `type: message` convention (`feat:`, `fix:`, `ci:`, `docs:`).

## Flow

1. User asks Claude to cut a release (no fixed phrase — "cut a release", "let's tag
   this", etc.).
2. Claude inspects commits since the last tag. Any `feat:` commit → minor bump
   (7.1.0 → 7.2.0); otherwise → patch bump (7.1.0 → 7.1.1).
3. Claude generates release notes from those commits (grouped by type) via `git-cliff`.
4. Claude updates `VERSION`/`VERSIONDATE` in `src-gui/version.h` and commits that change.
5. Claude shows the proposed version + changelog text and waits for explicit
   confirmation.
6. On confirmation: `git tag <version>` (bare semver, no `v` prefix) +
   `git push origin main --tags`.
7. CI reacts to the new tag:
   - `build-macos.yml`: builds arm64 natively on `macos-14`; builds x86_64 on the same
     `macos-14` runner under Rosetta 2 (`arch -x86_64 brew install qt gmp mpfr`,
     `arch -x86_64 qmake`, `arch -x86_64 make`, separate `/usr/local` Rosetta prefix).
     Produces two DMGs as today.
   - New `build-windows.yml`: MSYS2/MinGW setup mirroring `install_win.ps1`
     (qmake → mingw32-make → mingw32-make install), zips the `dist/` folder, uploads as
     artifact.
   - One job creates the GitHub Release (with the changelog body); the other(s) attach
     their artifact to the same release rather than each trying to create it, to avoid a
     race. Use `softprops/action-gh-release@v2`, which is idempotent per tag — multiple
     jobs targeting the same tag append `files:` rather than overwrite, so no explicit
     "first job creates, others append" logic is needed beyond all jobs guarding on
     `startsWith(github.ref, 'refs/tags/')` with no `v` prefix.

## Components

- `cliff.toml` — git-cliff config grouping commits by existing prefixes
  (`feat`, `fix`, `ci`, `docs`, `refactor`, etc.) for the release-notes body.
- `.github/workflows/build-macos.yml` changes:
  - trigger: `tags: ['v*']` → `tags: ['*.*.*']`
  - matrix: drop `macos-13`/Intel entry; add Rosetta x86_64 cross-build steps on
    `macos-14`
  - release-attach step: drop the `v` prefix check
- New `.github/workflows/build-windows.yml`:
  - MSYS2/MinGW + Qt + GMP/MPFR setup
  - `qmake -r P4.pro` → `mingw32-make` → `mingw32-make install`
  - zip `dist/` → `P4-windows-x86_64.zip`
  - attach to the release for tags matching bare semver
- No new repo files for changelog persistence — release body only, nothing committed to
  the working tree besides the `version.h` bump.

## Out of scope

- No silent/self-updating client (covered separately, declined for macOS permission
  reasons).
- No `CHANGELOG.md` committed to the repo.
- No universal/fat macOS binary — two separate DMGs (arm64, x86_64) continue as today.
