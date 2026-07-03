import pytest
from vanillapdf import (
    Document,
    File,
    PKCS12Key,
    DocumentSignatureSettings,
    MessageDigestAlgorithm,
    FieldType,
    TrustedCertificateStore,
    SignatureVerificationSettings,
)

KEY_PASSWORD = "test"


@pytest.fixture
def signed_pdf(plain_pdf, signing_key_path, tmp_path):
    """Sign the plain PDF and return the signed file path."""
    signed = str(tmp_path / "signed.pdf")
    with (
        PKCS12Key.create_from_file(signing_key_path, KEY_PASSWORD) as key,
        key.to_signing_key() as signing_key,
        DocumentSignatureSettings.create() as settings,
        Document(plain_pdf) as doc,
        File.create(signed) as dest,
    ):
        settings.digest = MessageDigestAlgorithm.SHA256
        settings.set_signing_key(signing_key)
        doc.sign(dest, settings)
    return signed


def _first_signature(doc):
    """Return the first DigitalSignature in the document, or None."""
    form = doc.get_catalog().get_acro_form()
    if form is None:
        return None
    for field in form.fields:
        signature_field = field.as_signature_field()
        if signature_field is not None:
            return signature_field.value
    return None


def test_unsigned_document_has_no_acro_form(plain_pdf):
    with Document(plain_pdf) as doc:
        assert doc.get_catalog().get_acro_form() is None


def test_signed_document_has_signature_field(signed_pdf):
    with Document(signed_pdf) as doc:
        form = doc.get_catalog().get_acro_form()
        assert form is not None
        assert len(form.fields) >= 1
        assert FieldType.SIGNATURE in [field.field_type for field in form.fields]


def test_verify_signature(signed_pdf):
    """A freshly signed, unmodified document verifies as cryptographically valid
    and intact (certificate validation skipped: the test cert is self-signed)."""
    with Document(signed_pdf) as doc:
        signature = _first_signature(doc)
        assert signature is not None
        with (
            signature,
            TrustedCertificateStore.create() as store,
            SignatureVerificationSettings.create() as settings,
        ):
            settings.skip_certificate_validation = True
            with signature.verify(doc, store, settings) as result:
                assert result.is_signature_valid is True
                assert result.is_document_intact is True
