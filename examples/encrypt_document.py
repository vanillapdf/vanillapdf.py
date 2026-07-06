"""Encrypt a PDF with AES-128 and a user + owner password.

Usage::

    python encrypt_document.py input.pdf encrypted.pdf
"""

import sys

from vanillapdf import (
    Document,
    DocumentEncryptionSettings,
    EncryptionAlgorithm,
    UserAccessPermission,
)


def encrypt(source: str, destination: str, user_pw: str, owner_pw: str) -> None:
    # The source must not already be encrypted; add_encryption refuses to
    # double-encrypt (remove_encryption first if needed).
    with Document(source) as doc:
        settings = DocumentEncryptionSettings.create()
        try:
            settings.algorithm = EncryptionAlgorithm.AES
            settings.key_length = 128
            # Deny extra permissions to anyone opening with the user password.
            settings.user_access_permissions = UserAccessPermission.NONE
            settings.set_owner_password(owner_pw)
            settings.set_user_password(user_pw)

            doc.add_encryption(settings)
            doc.save(destination)  # encryption is applied on save
        finally:
            settings.close()

    print(f"Wrote encrypted document to {destination}")


if __name__ == "__main__":
    if len(sys.argv) != 3:
        raise SystemExit("usage: python encrypt_document.py input.pdf encrypted.pdf")
    encrypt(sys.argv[1], sys.argv[2], user_pw="user-secret", owner_pw="owner-secret")
