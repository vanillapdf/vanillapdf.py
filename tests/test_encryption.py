import pytest
from vanillapdf import (
    Document,
    File,
    PageObject,
    Rectangle,
    DocumentEncryptionSettings,
    EncryptionAlgorithm,
    UserAccessPermission,
    PdfError,
)

OWNER_PASSWORD = "owner-secret"
USER_PASSWORD = "user-secret"


@pytest.fixture
def plain_pdf(tmp_path) -> str:
    """Create a fresh, unencrypted single-page PDF and return its path.

    The bundled assets/pdf-test.pdf is itself encrypted (empty user password),
    so encryption tests need a document known to start out unencrypted.
    """
    path = str(tmp_path / "plain.pdf")
    with Document.create(path) as doc:
        page = PageObject.create_from_document(doc)
        rect = Rectangle.create()
        rect.lower_left_x = 0
        rect.lower_left_y = 0
        rect.upper_right_x = 200
        rect.upper_right_y = 300
        page.media_box = rect
        doc.get_catalog().get_pages().append(page)
        doc.save(path)

    with File(path) as f:
        f.initialize()
        assert f.is_encrypted is False  # sanity: the source really is plaintext
    return path


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
