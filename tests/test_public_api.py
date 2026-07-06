"""Package-level checks that cut across objects (kept at the tests root)."""
from pathlib import Path

import vanillapdf


def test_public_api_importable():
    """Every name advertised in ``__all__`` resolves -- guards against broken or
    circular imports (the typing refactor added many TYPE_CHECKING imports)."""
    for name in vanillapdf.__all__:
        assert hasattr(vanillapdf, name), f"missing export: {name}"
        assert getattr(vanillapdf, name) is not None, name


def test_ships_pep561_type_information():
    """The installed package must carry its PEP 561 marker and the native stub,
    so downstream type checkers pick up the types (see #43)."""
    package_dir = Path(vanillapdf.__file__).parent
    assert (package_dir / "py.typed").is_file()
    assert (package_dir / "_vanillapdf.pyi").is_file()
