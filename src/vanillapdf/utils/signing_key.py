from .. import _vanillapdf
from ..handle import Handle


class SigningKey(Handle):
    """A private key used to sign documents.

    Obtain one from a :class:`~vanillapdf.PKCS12Key` via
    :meth:`~vanillapdf.PKCS12Key.to_signing_key`.
    """

    _release = staticmethod(_vanillapdf.signing_key_release)

    def __init__(self, handle):
        self._handle = handle

    @classmethod
    def _from_handle(cls, handle) -> "SigningKey":
        return cls(handle)
