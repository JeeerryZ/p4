# Release Automation Implementation Plan

> **For Claude:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task.

**Goal:** Replace hand-written changelogs with a chat-driven release flow — auto-detected
version bump, `git-cliff`-generated release notes, and CI that builds macOS (arm64 +
Rosetta x86_64) and Windows artifacts and attaches them to the GitHub Release for each tag.

**Architecture:** Three static config files (`cliff.toml`, modified `build-macos.yml`, new
`build-windows.yml`) plus a documented chat procedure (no new script — Claude computes the
version bump and runs `git tag`/`git push` on request, per the approved design doc at
`docs/plans/2026-06-23-release-automation-design.md`).

**Tech Stack:** git-cliff (changelog generation), GitHub Actions (`softprops/action-gh-release@v2`),
MSYS2/MinGW (Windows build), Homebrew + Rosetta 2 (macOS Intel cross-build).

---

### Task 1: Add git-cliff config

**Files:**
- Create: `cliff.toml`

**Step 1: Write the config**

```toml
[changelog]
header = ""
body = """
{% for group, commits in commits | group_by(attribute="group") %}
### {{ group | upper_first }}
{% for commit in commits %}
- {{ commit.message | upper_first }}
{% endfor %}
{% endfor %}
"""
trim = true

[git]
conventional_commits = true
filter_unconventional = false
commit_parsers = [
  { message = "^feat", group = "Features" },
  { message = "^fix", group = "Fixes" },
  { message = "^ci", group = "CI" },
  { message = "^docs", group = "Documentation" },
  { message = "^refactor", group = "Refactoring" },
  { message = "^chore", group = "Chore" },
  { message = ".*", group = "Other" },
]
filter_commits = false
tag_pattern = "[0-9]*.[0-9]*.[0-9]*"
```

**Step 2: Validate TOML syntax**

Run: `python3 -c "import tomllib; tomllib.load(open('cliff.toml','rb'))"`
Expected: no output, exit code 0 (the file parses as valid TOML)

**Step 3: Commit**

```bash
git add cliff.toml
git commit -m "ci: add git-cliff config for release notes generation"
```

---

### Task 2: Re-scope build-macos.yml — bare semver tag trigger

**Files:**
- Modify: `.github/workflows/build-macos.yml:1-10`

**Step 1: Change the tag trigger**

Current (line 7):
```yaml
    tags: ['v*']
```

New:
```yaml
    tags: ['[0-9]+.[0-9]+.[0-9]+']
```

**Step 2: Validate YAML syntax**

Run: `python3 -c "import yaml; yaml.safe_load(open('.github/workflows/build-macos.yml'))"`
Expected: no output, exit code 0

**Step 3: Commit**

```bash
git add .github/workflows/build-macos.yml
git commit -m "ci: trigger macOS build on bare semver tags, not v-prefixed"
```

---

### Task 3: Re-scope build-macos.yml — drop release-attach v-prefix check

**Files:**
- Modify: `.github/workflows/build-macos.yml` (the "Attach DMG to GitHub Release" step,
  currently `if: startsWith(github.ref, 'refs/tags/v')`)

**Step 1: Change the condition**

Current:
```yaml
      - name: Attach DMG to GitHub Release
        if: startsWith(github.ref, 'refs/tags/v')
```

New:
```yaml
      - name: Attach DMG to GitHub Release
        if: startsWith(github.ref, 'refs/tags/')
```

**Step 2: Validate YAML syntax**

Run: `python3 -c "import yaml; yaml.safe_load(open('.github/workflows/build-macos.yml'))"`
Expected: no output, exit code 0

**Step 3: Commit**

```bash
git add .github/workflows/build-macos.yml
git commit -m "ci: attach macOS DMG to release for any tag, not just v-prefixed"
```

---

### Task 4: Replace the Intel matrix entry with a Rosetta cross-build on arm64

**Files:**
- Modify: `.github/workflows/build-macos.yml` (matrix block, dependency-install step,
  qmake step, compile step — everything currently keyed on `matrix.brew_prefix`/`matrix.runner`)

**Step 1: Update the matrix**

Current:
```yaml
      matrix:
        include:
          - runner: macos-14    # Apple Silicon (ARM64)
            arch: arm64
            brew_prefix: /opt/homebrew
          - runner: macos-13    # Intel (x86_64) — last Intel GitHub-hosted runner
            arch: x86_64
            brew_prefix: /usr/local
```

New (both entries run on the same arm64 runner; x86_64 cross-builds under Rosetta 2):
```yaml
      matrix:
        include:
          - runner: macos-14    # Apple Silicon (ARM64), native build
            arch: arm64
            brew_prefix: /opt/homebrew
            arch_prefix: ""
          - runner: macos-14    # Intel (x86_64), cross-built via Rosetta 2
            arch: x86_64
            brew_prefix: /usr/local
            arch_prefix: "arch -x86_64"
```

**Step 2: Update the dependency-install step to use the arch prefix**

Current:
```yaml
      - name: Install dependencies
        run: brew install qt gmp mpfr
```

New:
```yaml
      - name: Install dependencies
        run: ${{ matrix.arch_prefix }} brew install qt gmp mpfr
```

**Step 3: Update the qmake/compile steps**

Current:
```yaml
      - name: Configure with qmake
        run: |
          export PATH="${{ matrix.brew_prefix }}/opt/qt/bin:$PATH"
          qmake -r P4.pro

      - name: Compile
        run: make -j$(sysctl -n hw.logicalcpu)
```

New:
```yaml
      - name: Configure with qmake
        run: |
          export PATH="${{ matrix.brew_prefix }}/opt/qt/bin:$PATH"
          ${{ matrix.arch_prefix }} qmake -r P4.pro

      - name: Compile
        run: ${{ matrix.arch_prefix }} make -j$(sysctl -n hw.logicalcpu)
```

**Step 4: Update the macdeployqt and create-dmg steps the same way**

In the "Wrap in .app and deploy Qt frameworks" step, prefix `macdeployqt P4.app` with
`${{ matrix.arch_prefix }}`. In the "Create DMG" step, prefix `brew install create-dmg`
and the `create-dmg ...` invocation with `${{ matrix.arch_prefix }}` as well — Rosetta
binaries need the emulated brew prefix for `create-dmg` too.

**Step 5: Validate YAML syntax**

Run: `python3 -c "import yaml; yaml.safe_load(open('.github/workflows/build-macos.yml'))"`
Expected: no output, exit code 0

**Step 6: Commit**

```bash
git add .github/workflows/build-macos.yml
git commit -m "ci: cross-build macOS Intel via Rosetta 2 on arm64 runner"
```

**Note:** This cannot be fully verified locally (no Mac runner here). The actual proof
comes from the next tagged push triggering CI — flag this to the user before relying on
it for a real release.

---

### Task 5: Create build-windows.yml

**Files:**
- Create: `.github/workflows/build-windows.yml`
- Reference: `install_win.ps1` (mirrors its build steps), `CLAUDE.md` Windows Build Note
  (MinGW path), `P4.pri` (GMP/MPFR auto-detection)

**Step 1: Read install_win.ps1 to confirm the exact build steps to mirror**

Run: `Read E:\dev\p4\install_win.ps1`

(Do this before writing the workflow — confirm qmake/make/install ordering and any
pacman package names match what's mirrored below.)

**Step 2: Write the workflow**

```yaml
name: Windows build

on:
  push:
    branches: [main]
    tags: ['[0-9]+.[0-9]+.[0-9]+']
  pull_request:
    branches: [main]
  workflow_dispatch:

jobs:
  build:
    runs-on: windows-latest
    defaults:
      run:
        shell: msys2 {0}
    steps:
      - name: Checkout
        uses: actions/checkout@v4

      - name: Set up MSYS2
        uses: msys2/setup-msys2@v2
        with:
          msystem: MINGW64
          update: true
          install: >-
            mingw-w64-x86_64-qt6-base
            mingw-w64-x86_64-qt6-tools
            mingw-w64-x86_64-gmp
            mingw-w64-x86_64-mpfr
            mingw-w64-x86_64-toolchain
            make

      - name: Configure with qmake
        run: qmake -r P4.pro

      - name: Compile
        run: mingw32-make -j$(nproc)

      - name: Install (assemble dist/)
        run: mingw32-make install

      - name: Bundle Qt runtime dependencies
        run: windeployqt dist\p4\p4.exe

      - name: Zip distribution
        shell: pwsh
        run: Compress-Archive -Path dist\* -DestinationPath P4-windows-x86_64.zip

      - name: Upload zip as artifact
        uses: actions/upload-artifact@v4
        with:
          name: P4-windows-x86_64
          path: P4-windows-x86_64.zip
          retention-days: 30

      - name: Attach zip to GitHub Release
        if: startsWith(github.ref, 'refs/tags/')
        uses: softprops/action-gh-release@v2
        with:
          files: P4-windows-x86_64.zip
```

**Step 3: Validate YAML syntax**

Run: `python3 -c "import yaml; yaml.safe_load(open('.github/workflows/build-windows.yml'))"`
Expected: no output, exit code 0

**Step 4: Commit**

```bash
git add .github/workflows/build-windows.yml
git commit -m "ci: add Windows build workflow, attach zip to releases"
```

**Note:** Like Task 4, this cannot be exercised locally — `windows-latest` + MSYS2
package names and `dist/` layout need verification on the first real CI run. Watch the
Actions log on the next tag push and be ready to patch package names/paths if the
`mingw32-make install` layout doesn't match what `windeployqt`/`Compress-Archive` expect.

---

### Task 6: Dry-run the release-notes generation locally (no git-cliff binary needed)

**Files:** none (verification only)

**Step 1: Confirm the commit range and grouping manually**

Run: `git log 7.1.0..HEAD --oneline`
Expected: the 8+ commits already known from this session (`feat:`, `fix:`, `ci:`, `docs:`
prefixes) — confirms there's at least one `feat:` commit, so the first real release
under this scheme bumps **minor** (7.1.0 → 7.2.0), per the agreed bump rule.

**Step 2: No commit for this task** — it's a sanity check that the bump rule and
`cliff.toml` grouping config (Task 1) produce the expected result before ever using them
for a real release. If the grouping looks wrong, fix `cliff.toml`'s `commit_parsers`
before proceeding.

---

### Task 7: Update CLAUDE.md with the release procedure

**Files:**
- Modify: `CLAUDE.md` (append a new section, do not restructure existing content)

**Step 1: Add a "Release Process" section**

Append after the existing "Windows Build Note" section:

```markdown
## Release Process

Tags are bare semver (`7.1.1`, `7.2.0` — no `v` prefix). To cut a release, ask Claude
("cut a release") — it will:
1. Inspect commits since the last tag; any `feat:` commit bumps minor, otherwise patch.
2. Generate release notes from `cliff.toml`'s grouping.
3. Update `VERSION`/`VERSIONDATE` in `src-gui/version.h` and commit.
4. Show you the version + notes for confirmation before tagging.
5. On confirmation, tag and `git push origin main --tags` — CI builds macOS (arm64 +
   Rosetta x86_64) and Windows artifacts and attaches them to the GitHub Release.

Never tag/push without explicit confirmation in chat.
```

**Step 2: Commit**

```bash
git add CLAUDE.md
git commit -m "docs: document the release process in CLAUDE.md"
```

---

## Out of scope (per design doc)

- No `CHANGELOG.md` committed to the repo — release body only.
- No silent auto-updater.
- No universal/fat macOS binary.
- No automated trigger — every release requires an explicit chat confirmation.
