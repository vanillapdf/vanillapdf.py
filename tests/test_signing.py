import os
import pytest
from vanillapdf import (
    Document,
    File,
    PKCS12Key,
    SigningKey,
    DocumentSignatureSettings,
    MessageDigestAlgorithm,
    PdfError,
)

KEY_PASSWORD = "test"


def _sign(source: str, destination: str, signing_key: SigningKey) -> None:
    settings = DocumentSignatureSettings.create()
    try:
        settings.digest = MessageDigestAlgorithm.SHA256
        settings.set_signing_key(signing_key)
        with Document(source) as doc:
            dest = File.create(destination)
            try:
                doc.sign(dest, settings)
            finally:
                dest.close()  # release flushes the signed output to disk
    finally:
        settings.close()


def test_pkcs12_key_loads(signing_key_path):
    key = PKCS12Key.create_from_file(signing_key_path, KEY_PASSWORD)
    try:
        signing_key = key.to_signing_key()
        assert isinstance(signing_key, SigningKey)
        signing_key.close()
    finally:
        key.close()


def test_pkcs12_wrong_password_raises(signing_key_path):
    with pytest.raises(PdfError):
        PKCS12Key.create_from_file(signing_key_path, "not-the-password")


def test_sign_document(plain_pdf, signing_key_path, tmp_path):
    """Sign a document and confirm the signed output is a valid, larger PDF."""
    signed = str(tmp_path / "signed.pdf")

    key = PKCS12Key.create_from_file(signing_key_path, KEY_PASSWORD)
    try:
        signing_key = key.to_signing_key()
        try:
            _sign(plain_pdf, signed, signing_key)
        finally:
            signing_key.close()
    finally:
        key.close()

    assert os.path.getsize(signed) > os.path.getsize(plain_pdf)

    # The signed document reopens and is structurally valid.
    with Document(signed) as doc:
        assert len(doc.get_catalog().get_pages()) >= 1


def test_signature_settings_digest_round_trip():
    settings = DocumentSignatureSettings.create()
    try:
        settings.digest = MessageDigestAlgorithm.SHA512
        assert settings.digest == MessageDigestAlgorithm.SHA512
    finally:
        settings.close()
