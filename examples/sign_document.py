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
    key = PKCS12Key.create_from_file(key_path, key_password)
    try:
        signing_key = key.to_signing_key()
        try:
            settings = DocumentSignatureSettings.create()
            try:
                settings.digest = MessageDigestAlgorithm.SHA256
                settings.set_signing_key(signing_key)

                with Document(source) as doc:
                    # Signing writes to a destination File (not doc.save()).
                    dest = File.create(destination)
                    try:
                        doc.sign(dest, settings)
                    finally:
                        dest.close()  # release flushes the signed output
            finally:
                settings.close()
        finally:
            signing_key.close()
    finally:
        key.close()

    print(f"Wrote signed document to {destination}")


if __name__ == "__main__":
    if len(sys.argv) != 5:
        raise SystemExit(
            "usage: python sign_document.py input.pdf signed.pdf key.pfx password"
        )
    sign(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])
