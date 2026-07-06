from enum import IntEnum, IntFlag

from .. import _vanillapdf
from ..handle import Handle
from ..utils.buffer import Buffer


class EncryptionAlgorithm(IntEnum):
    """Document encryption algorithm (mirrors native EncryptionAlgorithmType)."""
    UNDEFINED = 0
    NONE = 1
    RC4 = 2
    AES = 3


class UserAccessPermission(IntFlag):
    """User access permission bit flags (mirrors native UserAccessPermissionFlags).

    These control what a user opening the document with the *user* password may
    do. Combine with ``|``; the values are the PDF permission bit positions.
    """
    NONE = 0
    PRINT_DEGRADED = 4
    MODIFY_CONTENTS = 8
    COPY_AND_EXTRACT = 16
    ADD_ANNOTATIONS = 32
    FILL_FORMS = 256
    EXTRACT_TEXT = 512
    ASSEMBLE_DOCUMENT = 1024
    PRINT_FAITHFUL = 2048


def _as_bytes(password) -> bytes:
    if isinstance(password, str):
        return password.encode("utf-8")
    return bytes(password)


class DocumentEncryptionSettings(Handle):
    """Settings controlling how a document is encrypted.

    Configure the algorithm, key length, permissions, and passwords, then pass
    the settings to :meth:`~vanillapdf.Document.add_encryption`. Passwords are
    write-only, matching the native API.
    """

    _release = staticmethod(_vanillapdf.document_encryption_settings_release)

    def __init__(self, handle):
        self._handle = handle

    @staticmethod
    def create() -> "DocumentEncryptionSettings":
        handle = _vanillapdf.document_encryption_settings_create()
        return DocumentEncryptionSettings(handle)

    @property
    def algorithm(self) -> EncryptionAlgorithm:
        handle = self._require_handle()
        return EncryptionAlgorithm(_vanillapdf.document_encryption_settings_get_algorithm(handle))

    @algorithm.setter
    def algorithm(self, value: EncryptionAlgorithm) -> None:
        handle = self._require_handle()
        _vanillapdf.document_encryption_settings_set_algorithm(handle, int(value))

    @property
    def key_length(self) -> int:
        handle = self._require_handle()
        return _vanillapdf.document_encryption_settings_get_key_length(handle)

    @key_length.setter
    def key_length(self, value: int) -> None:
        handle = self._require_handle()
        _vanillapdf.document_encryption_settings_set_key_length(handle, value)

    @property
    def user_access_permissions(self) -> UserAccessPermission:
        handle = self._require_handle()
        value = _vanillapdf.document_encryption_settings_get_user_access_permissions(handle)
        return UserAccessPermission(value)

    @user_access_permissions.setter
    def user_access_permissions(self, value: UserAccessPermission) -> None:
        handle = self._require_handle()
        _vanillapdf.document_encryption_settings_set_user_access_permissions(handle, int(value))

    def set_user_password(self, password) -> None:
        """Set the user password (``bytes`` or ``str``)."""
        handle = self._require_handle()
        buffer = Buffer.create_from_data(_as_bytes(password))
        try:
            _vanillapdf.document_encryption_settings_set_user_password(handle, buffer._handle)
        finally:
            buffer.close()

    def set_owner_password(self, password) -> None:
        """Set the owner password (``bytes`` or ``str``)."""
        handle = self._require_handle()
        buffer = Buffer.create_from_data(_as_bytes(password))
        try:
            _vanillapdf.document_encryption_settings_set_owner_password(handle, buffer._handle)
        finally:
            buffer.close()
