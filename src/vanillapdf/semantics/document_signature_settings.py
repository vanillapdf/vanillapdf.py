from enum import IntEnum
from .. import _vanillapdf
from ..handle import Handle
from ..utils.signing_key import SigningKey


class MessageDigestAlgorithm(IntEnum):
    """Message digest algorithm (mirrors native MessageDigestAlgorithmType)."""
    UNDEFINED = 0
    MDNULL = 1
    MD2 = 2
    MD4 = 3
    MD5 = 4
    SHA1 = 5
    SHA224 = 6
    SHA256 = 7
    SHA384 = 8
    SHA512 = 9
    MDC2 = 10
    RIPEMD160 = 11
    WHIRLPOOL = 12


class DocumentSignatureSettings(Handle):
    """Settings controlling how a document is signed.

    Set the signing key and digest algorithm, then pass the settings to
    :meth:`~vanillapdf.Document.sign`.
    """

    _release = staticmethod(_vanillapdf.document_signature_settings_release)

    def __init__(self, handle):
        self._handle = handle

    @staticmethod
    def create() -> "DocumentSignatureSettings":
        handle = _vanillapdf.document_signature_settings_create()
        return DocumentSignatureSettings(handle)

    @property
    def digest(self) -> MessageDigestAlgorithm:
        handle = self._require_handle()
        return MessageDigestAlgorithm(_vanillapdf.document_signature_settings_get_digest(handle))

    @digest.setter
    def digest(self, value: MessageDigestAlgorithm) -> None:
        handle = self._require_handle()
        _vanillapdf.document_signature_settings_set_digest(handle, int(value))

    def set_signing_key(self, signing_key: SigningKey) -> None:
        """Set the :class:`~vanillapdf.SigningKey` used to sign the document."""
        handle = self._require_handle()
        _vanillapdf.document_signature_settings_set_signing_key(handle, signing_key._handle)
