from __future__ import annotations

from typing import TYPE_CHECKING

from .. import _vanillapdf
from ..handle import Handle

if TYPE_CHECKING:
    from .._vanillapdf import SigningKeyHandle


class SigningKey(Handle["SigningKeyHandle"]):
    """A private key used to sign documents.

    Obtain one from a :class:`~vanillapdf.PKCS12Key` via
    :meth:`~vanillapdf.PKCS12Key.to_signing_key`.
    """

    _release = staticmethod(_vanillapdf.signing_key_release)

    def __init__(self, handle: SigningKeyHandle) -> None:
        self._handle = handle

    @classmethod
    def _from_handle(cls, handle: SigningKeyHandle) -> SigningKey:
        return cls(handle)
