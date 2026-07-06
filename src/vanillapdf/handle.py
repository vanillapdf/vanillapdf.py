from __future__ import annotations

from collections.abc import Callable
from types import TracebackType
from typing import Any, Generic, TypeVar

_H = TypeVar("_H")          # this wrapper's native handle marker type
_H2 = TypeVar("_H2")        # another wrapper's handle type (for _handle_of)
_Self = TypeVar("_Self", bound="Handle[Any]")


class Handle(Generic[_H]):
    """Base class for objects that own a native capsule handle.

    Parametrised by the native handle marker type (e.g. ``Handle[BufferHandle]``)
    so the type checker keeps handle types apart. The binding already validates
    each capsule's name at runtime (a mismatched handle raises ``TypeError`` /
    ``PdfError``); carrying the type here moves that check to pyright, so a wrapper
    paired with the wrong ``_release`` -- or a native call given the wrong handle
    -- is caught as a static error in the editor/CI instead of at runtime.

    Subclasses set ``_release`` to the matching ``_vanillapdf.*_release`` and
    assign ``self._handle`` in ``__init__``. The native handle is internal: it
    lives only in ``_handle`` and the ``_from_handle`` / ``__init__`` factories.
    Wrappers pass another wrapper's handle to native calls via :meth:`_handle_of`
    (never by reaching into another object's ``_handle``), so callers only ever
    see real objects.

    ``_handle`` defaults to ``None`` at the class level, so ``__del__`` /
    :meth:`close` stay safe even if ``__init__`` raises before assigning it
    (e.g. the native open call fails and raises :class:`~vanillapdf.errors.PdfError`).
    """

    _handle: _H | None = None                                 # __del__ safe on failed __init__
    _release: Callable[[_H], object] | None = None            # subclass sets the release callable

    def _require_handle(self) -> _H:
        """Return the live handle, or raise if this object has been closed."""
        if self._handle is None:
            raise ValueError(f"{type(self).__name__} has been closed")
        return self._handle

    @staticmethod
    def _handle_of(obj: Handle[_H2]) -> _H2:
        """Return another wrapper's live handle for use in a native call.

        Routes cross-object handle access through the base class so wrappers
        never touch each other's private ``_handle`` attribute directly.
        """
        return obj._require_handle()

    def close(self) -> None:
        """Release the native handle. Idempotent."""
        handle = self._handle
        release = type(self)._release
        if handle is not None and release is not None:
            release(handle)
            self._handle = None

    def __enter__(self: _Self) -> _Self:
        return self

    def __exit__(
        self,
        exc_type: type[BaseException] | None,
        exc_val: BaseException | None,
        exc_tb: TracebackType | None,
    ) -> None:
        self.close()

    def __del__(self) -> None:
        self.close()
