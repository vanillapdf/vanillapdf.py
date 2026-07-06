from .. import _vanillapdf
from ..handle import Handle
from ..utils.signature_verifier import SignatureVerificationResult


class DigitalSignature(Handle):
    """A digital signature embedded in a document.

    Obtain one from a signature field
    (:attr:`~vanillapdf.SignatureField.value`) and verify it with
    :meth:`verify`.
    """

    _release = staticmethod(_vanillapdf.digital_signature_release)

    def __init__(self, handle):
        self._handle = handle

    @classmethod
    def _from_handle(cls, handle) -> "DigitalSignature":
        return cls(handle)

    def verify(self, document, trusted_store=None, settings=None) -> SignatureVerificationResult:
        """Verify this signature against `document`.

        `trusted_store` is an optional
        :class:`~vanillapdf.TrustedCertificateStore` for certificate-chain
        validation; `settings` is an optional
        :class:`~vanillapdf.SignatureVerificationSettings`. Returns a
        :class:`~vanillapdf.SignatureVerificationResult`.
        """
        handle = self._require_handle()
        store_handle = None if trusted_store is None else trusted_store._handle
        settings_handle = None if settings is None else settings._handle
        result = _vanillapdf.digital_signature_verify(
            handle, document._handle, store_handle, settings_handle)
        return SignatureVerificationResult._from_handle(result)
