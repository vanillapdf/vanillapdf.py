from .document import Document
from .buffer import Buffer

try:
    from ._version import version as __version__
except ImportError:
    __version__ = "0.0.0.dev"

__all__ = ["Document", "Buffer", "__version__"]

