import pytest
from vanillapdf import (
    Document,
    File,
    DocumentEncryptionSettings,
    EncryptionAlgorithm,
    UserAccessPermission,
    PdfError,
)

OWNER_PASSWORD = "owner-secret"
USER_PASSWORD = "user-secret"


def _encrypt(source: str, destination: str) -> None:
    """Encrypt `source` with AES-128 and write it to `destination`."""
    with Document(source) as doc:
        settings = DocumentEncryptionSettings.create()
        try:
            settings.algorithm = EncryptionAlgorithm.AES
            settings.key_length = 128
            settings.user_access_permissions = UserAccessPermission.NONE
            settings.set_owner_password(OWNER_PASSWORD)
            settings.set_user_password(USER_PASSWORD)
            doc.add_encryption(settings)
            doc.save(destination)
        finally:
            settings.close()


def test_settings_round_trip():
    """The settings object stores what we set."""
    settings = DocumentEncryptionSettings.create()
    try:
        settings.algorithm = EncryptionAlgorithm.AES
        settings.key_length = 128
        settings.user_access_permissions = (
            UserAccessPermission.PRINT_FAITHFUL | UserAccessPermission.EXTRACT_TEXT
        )
        assert settings.algorithm == EncryptionAlgorithm.AES
        assert settings.key_length == 128
        assert UserAccessPermission.EXTRACT_TEXT in settings.user_access_permissions
    finally:
        settings.close()


def test_encrypt_document(plain_pdf, tmp_path):
    """Encrypting an unencrypted document produces an encrypted file."""
    encrypted = str(tmp_path / "encrypted.pdf")
    _encrypt(plain_pdf, encrypted)

    with File(encrypted) as f:
        f.initialize()
        f.set_encryption_password(OWNER_PASSWORD)
        assert f.is_encrypted is True


def test_decrypt_round_trip(plain_pdf, tmp_path):
    """Encrypt, then reopen with the password, remove encryption, and save a
    plaintext copy that reports as not encrypted."""
    encrypted = str(tmp_path / "encrypted.pdf")
    decrypted = str(tmp_path / "decrypted.pdf")
    _encrypt(plain_pdf, encrypted)

    with File(encrypted) as f:
        f.initialize()
        f.set_encryption_password(OWNER_PASSWORD)
        doc = Document.open_file(f)
        try:
            doc.remove_encryption()
            doc.save(decrypted)
        finally:
            doc.close()

    with File(decrypted) as f:
        f.initialize()
        assert f.is_encrypted is False


def test_wrong_password_raises(plain_pdf, tmp_path):
    """Opening an encrypted document with the wrong password surfaces a
    PdfError rather than silently returning garbage."""
    encrypted = str(tmp_path / "encrypted.pdf")
    _encrypt(plain_pdf, encrypted)

    with File(encrypted) as f:
        f.initialize()
        with pytest.raises(PdfError):
            f.set_encryption_password("wrong-password")
