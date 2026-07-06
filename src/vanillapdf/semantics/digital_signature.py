from __future__ import annotations

from typing import TYPE_CHECKING

from .. import _vanillapdf
from ..handle import Handle
from ..utils.signature_verifier import SignatureVerificationResult

if TYPE_CHECKING:
    from .._vanillapdf import DigitalSignatureHandle
    from ..utils.signature_verifier import (
        SignatureVerificationSettings,
        TrustedCertificateStore,
    )
    from .document import Document


class DigitalSignature(Handle["DigitalSignatureHandle"]):
    """A digital signature embedded in a document.

    Obtain one from a signature field
    (:attr:`~vanillapdf.SignatureField.value`) and verify it with
    :meth:`verify`.
    """

    _release = staticmethod(_vanillapdf.digital_signature_release)

    def __init__(self, handle: DigitalSignatureHandle) -> None:
        self._handle = handle

    @classmethod
    def _from_handle(cls, handle: DigitalSignatureHandle) -> DigitalSignature:
        return cls(handle)

    def verify(
        self,
        document: Document,
        trusted_store: TrustedCertificateStore | None = None,
        settings: SignatureVerificationSettings | None = None,
    ) -> SignatureVerificationResult:
        """Verify this signature against `document`.

        `trusted_store` is an optional
        :class:`~vanillapdf.TrustedCertificateStore` for certificate-chain
        validation; `settings` is an optional
        :class:`~vanillapdf.SignatureVerificationSettings`. Returns a
        :class:`~vanillapdf.SignatureVerificationResult`.
        """
        handle = self._require_handle()
        store_handle = None if trusted_store is None else self._handle_of(trusted_store)
        settings_handle = None if settings is None else self._handle_of(settings)
        result = _vanillapdf.digital_signature_verify(
            handle, self._handle_of(document), store_handle, settings_handle)
        return SignatureVerificationResult._from_handle(result)
