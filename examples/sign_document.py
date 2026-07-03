"""Sign a PDF with a PKCS#12 (.pfx/.p12) key using a SHA-256 digest.

Usage::

    python sign_document.py input.pdf signed.pdf key.pfx password
"""

import sys

from vanillapdf import (
    Document,
    File,
    PKCS12Key,
    DocumentSignatureSettings,
    MessageDigestAlgorithm,
)


def sign(source: str, destination: str, key_path: str, key_password: str) -> None:
    # Every handle here is a context manager, so a single `with` closes them all
    # in reverse order on exit (the destination File is released last-in/first-out,
    # which flushes the signed output). Later items can reference earlier ones.
    with (
        PKCS12Key.create_from_file(key_path, key_password) as key,
        key.to_signing_key() as signing_key,
        DocumentSignatureSettings.create() as settings,
        Document(source) as doc,
        File.create(destination) as dest,  # signing writes here, not via doc.save()
    ):
        settings.digest = MessageDigestAlgorithm.SHA256
        settings.set_signing_key(signing_key)
        doc.sign(dest, settings)

    print(f"Wrote signed document to {destination}")


if __name__ == "__main__":
    if len(sys.argv) != 5:
        raise SystemExit(
            "usage: python sign_document.py input.pdf signed.pdf key.pfx password"
        )
    sign(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])
