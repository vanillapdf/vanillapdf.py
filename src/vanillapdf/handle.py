from . import _vanillapdf


class Handle:
    """Base class for objects that own a native capsule handle.

    Subclasses set ``_release`` to the ``_vanillapdf.*_release`` function and
    assign ``self._handle`` in ``__init__``. This base provides context-manager
    support, an idempotent :meth:`close`, and a ``__del__`` safety net.

    ``_handle`` defaults to ``None`` at the class level, so ``__del__`` /
    :meth:`close` stay safe even if ``__init__`` raises before assigning it
    (e.g. the native open call fails and raises :class:`~vanillapdf.errors.PdfError`).
    """

    _handle = None            # class default -> __del__ safe on failed __init__
    _release = None           # subclass overrides with the release callable

    def _require_handle(self):
        """Return the live handle, or raise if this object has been closed."""
        if self._handle is None:
            raise ValueError(f"{type(self).__name__} has been closed")
        return self._handle

    def close(self) -> None:
        """Release the native handle. Idempotent."""
        if self._handle is not None:
            type(self)._release(self._handle)
            self._handle = None

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()

    def __del__(self):
        self.close()
