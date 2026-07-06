"""Sphinx configuration for the VanillaPDF Python bindings documentation."""

import os
import sys

# -- Make the (uninstalled) package importable -----------------------------
#
# The docs are built with autodoc against the source tree, so put ``src`` on
# the path. The native ``_vanillapdf`` C extension is mocked below, which means
# this build never has to compile the C++ library -- it imports the pure-Python
# wrappers only, reading their docstrings.
sys.path.insert(0, os.path.abspath("../src"))

# -- Project information ----------------------------------------------------

project = "VanillaPDF Python Bindings"
copyright = "2026, Vanilla.PDF Labs"
author = "Vanilla.PDF Labs"


def _resolve_release():
    """Best-effort version string: prefer an installed distribution, fall back
    to deriving it from git tags via setuptools-scm, then to a placeholder."""
    try:
        from importlib.metadata import PackageNotFoundError, version

        return version("vanillapdf")
    except PackageNotFoundError:
        pass
    except ImportError:
        pass

    try:
        from setuptools_scm import get_version

        return get_version(root="..", relative_to=__file__)
    except Exception:
        return "0.0.0"


release = _resolve_release()
version = ".".join(release.split(".")[:2])

# -- General configuration --------------------------------------------------

extensions = [
    "sphinx.ext.autodoc",
    "sphinx.ext.napoleon",
    "sphinx.ext.viewcode",
    "sphinx.ext.intersphinx",
    "sphinx_copybutton",
    "sphinxext.opengraph",
]

# The C extension is a compiled module with no docstrings we render; mock it so
# importing the wrappers works without a build.
autodoc_mock_imports = ["vanillapdf._vanillapdf"]

autodoc_default_options = {
    "members": True,
    "undoc-members": True,
    "show-inheritance": True,
    "member-order": "bysource",
}

# Napoleon (Google-style docstrings, as used across the wrappers).
napoleon_google_docstring = True
napoleon_numpy_docstring = False

intersphinx_mapping = {
    "python": ("https://docs.python.org/3", None),
}

templates_path = ["_templates"]
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store"]

# -- Options for HTML output ------------------------------------------------

html_theme = "sphinx_rtd_theme"
html_static_path = ["_static"]

html_theme_options = {
    "navigation_depth": 3,
}

html_context = {
    "display_github": True,
    "github_user": "vanillapdf",
    "github_repo": "vanillapdf.py",
    "github_version": "main",
    "conf_py_path": "/docs/",
}

# -- OpenGraph metadata (link previews) -------------------------------------

ogp_site_url = "https://vanillapdf-py.readthedocs.io/"
ogp_site_name = "VanillaPDF Python Bindings"
ogp_description_length = 200
