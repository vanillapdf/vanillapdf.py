# Contributing to Vanilla.PDF Python Bindings

Thank you for considering contributing to the **Vanilla.PDF Python bindings**!
This repository packages the [Vanilla.PDF](https://github.com/vanillapdf/vanillapdf)
C++17 library as a Python extension; it follows the same contribution conventions
as the native repository.

---

## 🐞 Reporting Bugs

Report bugs on [GitHub Issues](https://github.com/vanillapdf/vanillapdf.py/issues).
Please include:

- Version of `vanillapdf` (`python -c "import vanillapdf; print(vanillapdf.__version__)"`)
- Python version, operating system, and architecture
- Whether you installed a published wheel or built from source
- A clear description of what happens and what you expected instead
- A minimal code snippet that reproduces it
- A sample PDF (anonymized if needed)

If the bug is in PDF parsing or writing rather than in the Python layer, it may
belong in the [native repository](https://github.com/vanillapdf/vanillapdf/issues)
instead — report it here either way and we will move it if needed.

Still unsure? Reach out to [info@vanillapdf.com](mailto:info@vanillapdf.com).

---

## 💡 Feedback & Feature Requests

Missing a key feature? Have a great idea? We'd love to hear it — send a note to
[jzikmund@vanillapdf.com](mailto:jzikmund@vanillapdf.com).

---

## 🔁 Submitting Pull Requests

**🚨 MANDATORY: Branch and PR workflow**

- **All changes must go through a branch and pull request** — this is required by
  repository permissions.
- **Never commit directly to `main` or `release/*`** — they are protected.
- Create a new branch for every change, no matter how small.
- Follow the naming convention: **`feature/description`** or **`fix/description`**.

### 🌳 Branch naming

| Prefix              | Use for                                                       |
| ------------------- | ------------------------------------------------------------- |
| `feature/…`         | New functionality                                             |
| `fix/…`             | Bug fixes, dependency bumps, and other corrections            |
| `release/x.y`       | **Reserved** — long-lived maintenance lines, never a change    |

`release/x.y` branches exist to maintain an already-released line (for example
`release/2.2` receiving a backport). They are **not** a place to prepare a
release and must never be used as the branch for a pull request. Base your work
on `main`, or on the appropriate `release/x.y` branch for a hotfix to that line.

### 📝 Pull request guidelines

- 🧠 **Discuss major changes first**: for anything beyond a bug fix, contact the
  team before investing time.
- 🌳 **Base your PR on `main`** (or the appropriate `release/*` branch for hotfixes).
- 🏷️ **PR titles follow [Conventional Commits](https://www.conventionalcommits.org/)**.
  This repository squash-merges, so the PR title becomes the commit subject on
  `main`. `lint-pr-title.yml` enforces this.
- 🧹 **Match the existing coding style** — see [`docs/STYLE.md`](docs/STYLE.md).
- ✅ **Sign your commits** (see DCO below).
- 🧪 **Ensure checks pass**: `pytest tests/`, `ruff check .`, and pyright strict.

---

## 🧪 Local development

```bash
python -m venv venv
source venv/bin/activate      # Windows: venv\Scripts\activate
pip install -e .[test]

pytest tests/                 # tests
ruff check .                  # lint
```

If you changed the native extension surface, regenerate the type stub:

```bash
python scripts/generate_native_stub.py
python scripts/generate_native_stub.py --check   # verify it is in sync
```

Note that flaky tests are treated as bugs in this project, not noise — see the
"Zero tolerance for flakiness" section of [`CLAUDE.md`](CLAUDE.md) before
re-running a failed test.

---

## ✅ Developer Certificate of Origin (DCO)

By submitting a contribution, you certify the following:

> "I certify that I have the right to submit this code under the open source
> license indicated in this repository and that I am doing so in good faith."

All commits must be signed off with a `Signed-off-by:` line. Commit with the
`-s` flag to add it automatically:

```bash
git commit -s -m "fix: correct page tree index validation"
```

This appends a line like:

```
Signed-off-by: Your Name <you@example.com>
```

---

## 🚀 Releases

Releases are cut by maintainers via the `release.yml` workflow — tags are never
created by hand. See [`RELEASE-GUIDE.md`](RELEASE-GUIDE.md) for the process and
for how this package's version relates to the native library's.
