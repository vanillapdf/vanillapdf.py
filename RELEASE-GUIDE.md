# 🧩 Vanilla.PDF Python Bindings — Release Guide

How to cut stable and prerelease versions of the `vanillapdf` Python package.
This mirrors the [native Vanilla.PDF release guide](https://github.com/vanillapdf/vanillapdf/blob/main/RELEASE-GUIDE.md);
differences specific to Python packaging are called out below.

---

## 🔢 Versioning

This package **mirrors the version of the native C++ library it wraps**, the same
convention [vanillapdf.net](https://github.com/vanillapdf/vanillapdf.net) uses.
Package `2.3.0` wraps native `2.3.0`, so the three repositories are readable
together. The pinned native tag is `VANILLAPDF_GIT_TAG` in `CMakeLists.txt`.

The version is therefore **not** a measure of how much of the native API the
bindings cover — it identifies which library they wrap.

### Binding-only releases

A fix in the Python or extension layer with no corresponding native release gets
a fourth component:

```
2.3.0    -> wraps native 2.3.0
2.3.0.1  -> binding-only fix, still wraps native 2.3.0
2.3.0.2  -> another binding-only fix
2.3.1    -> wraps native 2.3.1
```

This is a normal PEP 440 release segment. It is not treated as a prerelease, it
matches `~=2.3.0`, and it sorts below the next native patch.

PEP 440 **post-releases (`2.3.0.post1`) are deliberately not used** — the spec
reserves those for metadata corrections, explicitly stating that post-releases
should not be used for bug fixes.

### Prereleases

Prerelease tags follow the native repository: `-alpha.N`, `-beta.N`, `-rc.N`.
`v2.3.0-rc.2` normalizes to the PEP 440 version `2.3.0rc2`. Prereleases are never
marked as the "latest" GitHub release, and `pip` will not install them without
`--pre`.

---

## ✅ Branching strategy

### `main`

- Development happens here.
- Prerelease tags (`alpha`, `beta`, `rc`) can be cut directly from here.

### `release/x.y` (optional)

- Created only when stabilizing a major/minor release, or to maintain an older
  line after `main` has moved on.
- 🔒 Not merged back into `main`; development continues from `main`.
- **Never used as a branch for a pull request** — see [`CONTRIBUTING.md`](CONTRIBUTING.md).

```bash
git checkout -b release/2.3 main
```

Fixes land on `main` first, then get cherry-picked into `release/x.y` if they
apply. If the release branch is protected, the cherry-pick needs its own PR.

---

## 🏷️ Cutting a release

Tags are **not** created manually with `git tag` / `git push`. The `release.yml`
workflow owns tag creation end-to-end.

**Before dispatching**, land a PR on `main` that sets `VANILLAPDF_GIT_TAG` in
`CMakeLists.txt` to the native tag being wrapped. These bindings mirror the
native version, so the release tag must equal that pin (`v2.3.0`,
`v2.3.0-rc.2`), or — for a binding-only release of a stable pin — add a fourth
component (`v2.3.0.1`). The `prepare` job fails the run on any other tag.

Then:

1. **Dry run.** Dispatch `release.yml` with the tag and `dry_run: true` (Actions
   tab, or `gh workflow run release.yml -f tag=v2.3.0-rc.2 -f dry_run=true`).
   This validates the tag against the pinned native version, and builds and
   tests the full wheel matrix — publishing nothing and creating no tag.
2. **Real run.** Re-dispatch with `dry_run: false`. Wheels are rebuilt, pushed to
   **TestPyPI** as staging, and a GitHub release is created as a **draft** —
   still without creating the tag.
3. **Review.** The `pypi` environment gate pauses the workflow (required
   reviewer plus wait timer). Check the staged wheels on TestPyPI, edit the
   draft release body in the UI, then approve.
4. **Publish.** Approving publishes the draft, and *that* is what **creates the
   tag**, at the commit the draft targets. The PyPI upload runs in the same
   `pypi` environment, so it asks for approval once more, then the wheels go to
   **PyPI**.

Whether a tag counts as a prerelease is derived automatically from its suffix —
see the `prepare` job in `release.yml`.

### 🔁 Why the tag comes last

If the tag were created first and the build then failed, the tag would have to be
deleted and a fresh version chosen before retrying. Because the tag is created
only after every wheel has built, installed, and passed its tests, a failed
*build* is simply re-dispatched with the same tag. This matters more here than in
a pure Python project: the matrix is five platforms × five Python versions, each
doing a full C++ build via vcpkg.

The same reasoning is why the draft release exists before the gate — it can be
reviewed, and abandoned, without leaving anything permanent behind. A
re-dispatch reuses an existing draft for the tag and replaces its assets.

---

## 📦 How the version reaches the wheels

`setuptools-scm` normally derives the version from a git tag, but the tag does
not exist while the wheels are being built. `release.yml` therefore passes the
version explicitly through `SETUPTOOLS_SCM_PRETEND_VERSION_FOR_VANILLAPDF`, set
via `CIBW_ENVIRONMENT_*` so it reaches the build inside cibuildwheel's isolated
environments and Linux containers.

A guard step then checks every built wheel's filename against the expected
version, so a wheel stamped with a guessed `.devN` version can never be
published.

Outside of releases (local installs, nightly, bleeding-edge CI) setuptools-scm
still derives versions from git as usual — no environment variable is involved.

---

## 🧪 Workflow summary

| Workflow             | Trigger                          | Publishes?                          |
| -------------------- | -------------------------------- | ----------------------------------- |
| `release.yml`        | `workflow_dispatch` (tag input)  | ✅ Yes, unless `dry_run: true`      |
| `release.yml`        | PR touching the release workflows | ❌ No (always a dry run)           |
| `github-release.yml` | Called by `release.yml`          | Creates the **draft** release       |
| `sanity-check.yml`   | Push / PR to `main`              | ❌ No                               |
| `nightly-check.yml`  | Schedule                         | ❌ No                               |

`github-release.yml` attaches the wheels, a signed provenance bundle
(`*.intoto.jsonl`), and an SPDX SBOM to the release, and attests both.

---

## 🔧 Repository configuration

These must exist for a release to complete:

| Kind        | Name                    | Purpose                                                    |
| ----------- | ----------------------- | ---------------------------------------------------------- |
| Environment | `testpypi`              | Staging (the native repo's `staging`); holds `PYPI_REPOSITORY_URL` / `PYPI_PROJECT_URL` |
| Environment | `pypi`                  | Production (the native repo's `production`): the PyPI publish *and* the **human gate** before the tag is created. Required reviewers, a wait timer, and deployment rules for `main`, `release/*` and `v*.*.*`; same variables, pointing at PyPI |
| Ruleset     | Protect release tags    | `refs/tags/v*.*.*`: restricts deletion, non-fast-forward and update — **not creation**, since publishing the draft with `GITHUB_TOKEN` is what creates the tag |

No bot token is needed: the draft is created and published with `GITHUB_TOKEN`,
as in the native repository.

⚠️ **PyPI Trusted Publishing is bound to the repository, the workflow filename,
and the environment name.** Renaming `release.yml`, `pypi`, or `testpypi`
requires updating the publisher configuration on PyPI first, or publishing will
fail with an OIDC error.

---

## 📌 Notes

- **TestPyPI staging uses `skip-existing`**, so re-dispatching the same version
  after a partial failure does not error on files that are already there. PyPI
  itself is immutable: once a version is published it can never be replaced, so a
  botched *publish* always needs a new version number.
- The `0.0.0` release on TestPyPI dates from 2025-05-17, before `fetch-depth: 0`
  was added to the release checkout. It predates this process and can be ignored.
- As in the native repository, a PR that touches `release.yml` or
  `github-release.yml` runs `release.yml` as a dry run, with the tag derived from
  the pinned `VANILLAPDF_GIT_TAG`. That builds the full wheel matrix (hours of
  CI), but only for PRs that change the release workflows.
