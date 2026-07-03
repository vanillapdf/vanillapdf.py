from enum import IntEnum
from .. import _vanillapdf
from ..handle import Handle
from .buffer import Buffer


class SignatureVerificationStatus(IntEnum):
    """Overall signature verification status (mirrors native
    SignatureVerificationStatusType)."""
    UNDEFINED = 0
    VALID = 1
    INVALID = 2
    CERTIFICATE_EXPIRED = 3
    CERTIFICATE_NOT_YET_VALID = 4
    CERTIFICATE_REVOKED = 5
    CERTIFICATE_UNTRUSTED = 6
    DOCUMENT_MODIFIED = 7
    WEAK_ALGORITHM = 8
    MISSING_CERTIFICATE = 9
    UNKNOWN = 10


def _as_bytes(data) -> bytes:
    if isinstance(data, str):
        return data.encode("utf-8")
    return bytes(data)


class TrustedCertificateStore(Handle):
    """A set of trusted CA certificates used for signature chain validation."""

    _release = staticmethod(_vanillapdf.trusted_certificate_store_release)

    def __init__(self, handle):
        self._handle = handle

    @staticmethod
    def create() -> "TrustedCertificateStore":
        handle = _vanillapdf.trusted_certificate_store_create()
        return TrustedCertificateStore(handle)

    def add_certificate_from_pem(self, data) -> None:
        """Add a PEM-encoded certificate (``bytes`` or ``str``)."""
        handle = self._require_handle()
        with Buffer.create_from_data(_as_bytes(data)) as buffer:
            _vanillapdf.trusted_certificate_store_add_certificate_from_pem(handle, buffer._handle)

    def add_certificate_from_der(self, data) -> None:
        """Add a DER-encoded certificate (``bytes``)."""
        handle = self._require_handle()
        with Buffer.create_from_data(_as_bytes(data)) as buffer:
            _vanillapdf.trusted_certificate_store_add_certificate_from_der(handle, buffer._handle)

    def load_from_directory(self, path: str) -> None:
        """Load all certificates from a directory of PEM/DER files."""
        handle = self._require_handle()
        _vanillapdf.trusted_certificate_store_load_from_directory(handle, path)

    def load_system_defaults(self) -> None:
        """Load the operating system's default trusted certificates."""
        handle = self._require_handle()
        _vanillapdf.trusted_certificate_store_load_system_defaults(handle)


class SignatureVerificationSettings(Handle):
    """Flags controlling how strictly a signature is verified.

    All flags default to ``False``. Set ``skip_certificate_validation`` to check
    only the cryptographic signature and document integrity without requiring a
    trusted certificate chain.
    """

    _release = staticmethod(_vanillapdf.signature_verification_settings_release)

    def __init__(self, handle):
        self._handle = handle
        self._skip_certificate_validation = False
        self._allow_weak_algorithms = False
        self._check_signing_time = False

    @staticmethod
    def create() -> "SignatureVerificationSettings":
        handle = _vanillapdf.signature_verification_settings_create()
        return SignatureVerificationSettings(handle)

    @property
    def skip_certificate_validation(self) -> bool:
        return self._skip_certificate_validation

    @skip_certificate_validation.setter
    def skip_certificate_validation(self, value: bool) -> None:
        handle = self._require_handle()
        _vanillapdf.signature_verification_settings_set_skip_certificate_validation(handle, bool(value))
        self._skip_certificate_validation = bool(value)

    @property
    def allow_weak_algorithms(self) -> bool:
        return self._allow_weak_algorithms

    @allow_weak_algorithms.setter
    def allow_weak_algorithms(self, value: bool) -> None:
        handle = self._require_handle()
        _vanillapdf.signature_verification_settings_set_allow_weak_algorithms(handle, bool(value))
        self._allow_weak_algorithms = bool(value)

    @property
    def check_signing_time(self) -> bool:
        return self._check_signing_time

    @check_signing_time.setter
    def check_signing_time(self, value: bool) -> None:
        handle = self._require_handle()
        _vanillapdf.signature_verification_settings_set_check_signing_time(handle, bool(value))
        self._check_signing_time = bool(value)


class SignatureVerificationResult(Handle):
    """The outcome of verifying a digital signature."""

    _release = staticmethod(_vanillapdf.signature_verification_result_release)

    def __init__(self, handle):
        self._handle = handle

    @classmethod
    def _from_handle(cls, handle) -> "SignatureVerificationResult":
        return cls(handle)

    @property
    def status(self) -> SignatureVerificationStatus:
        handle = self._require_handle()
        return SignatureVerificationStatus(
            _vanillapdf.signature_verification_result_get_status(handle))

    @property
    def is_signature_valid(self) -> bool:
        """True if the signature is cryptographically valid."""
        handle = self._require_handle()
        return _vanillapdf.signature_verification_result_is_signature_valid(handle)

    @property
    def is_document_intact(self) -> bool:
        """True if the document is unmodified since it was signed."""
        handle = self._require_handle()
        return _vanillapdf.signature_verification_result_is_document_intact(handle)

    @property
    def is_certificate_trusted(self) -> bool:
        """True if the signer's certificate chains to a trusted root."""
        handle = self._require_handle()
        return _vanillapdf.signature_verification_result_is_certificate_trusted(handle)

    def _text(self, native_getter):
        handle = self._require_handle()
        result = native_getter(handle)
        if result is None:
            return None
        buffer = Buffer._from_handle(result)
        data = buffer.data
        return data.decode("utf-8", "replace") if data else None

    @property
    def signer_common_name(self):
        """The signer certificate's common name, or None."""
        return self._text(_vanillapdf.signature_verification_result_get_signer_common_name)

    @property
    def message(self):
        """A human-readable verification message, or None."""
        return self._text(_vanillapdf.signature_verification_result_get_message)
