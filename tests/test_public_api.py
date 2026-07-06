"""Package-level checks that cut across objects (kept at the tests root)."""
import vanillapdf


def test_public_api_importable():
    """Every name advertised in ``__all__`` resolves -- guards against broken or
    circular imports (the typing refactor added many TYPE_CHECKING imports)."""
    for name in vanillapdf.__all__:
        assert hasattr(vanillapdf, name), f"missing export: {name}"
        assert getattr(vanillapdf, name) is not None, name
