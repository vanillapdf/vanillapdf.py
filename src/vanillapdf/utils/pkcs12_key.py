from .. import _vanillapdf
from ..handle import Handle
from .buffer import Buffer
from .signing_key import SigningKey


class PKCS12Key(Handle):
    """A PKCS#12 (``.pfx`` / ``.p12``) private key and certificate.

    Load one from a file or buffer, then convert it to a
    :class:`~vanillapdf.SigningKey` for document signing.
    """

    _release = staticmethod(_vanillapdf.pkcs12_key_release)

    def __init__(self, handle):
        self._handle = handle

    @staticmethod
    def create_from_file(path: str, password: str) -> "PKCS12Key":
        """Load a PKCS#12 key from a file on disk."""
        handle = _vanillapdf.pkcs12_key_create_from_file(path, password)
        return PKCS12Key(handle)

    @staticmethod
    def create_from_buffer(buffer: Buffer, password: str) -> "PKCS12Key":
        """Load a PKCS#12 key from an in-memory :class:`~vanillapdf.Buffer`."""
        handle = _vanillapdf.pkcs12_key_create_from_buffer(buffer._handle, password)
        return PKCS12Key(handle)

    def to_signing_key(self) -> SigningKey:
        """Return a :class:`~vanillapdf.SigningKey` derived from this key."""
        handle = self._require_handle()
        signing_key = _vanillapdf.pkcs12_key_to_signing_key(handle)
        return SigningKey._from_handle(signing_key)
