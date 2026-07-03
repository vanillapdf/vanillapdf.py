"""Verify the digital signatures in a PDF.

Usage::

    python verify_signature.py signed.pdf [trusted_certs_dir]

Without a trusted-certs directory, certificate-chain validation is skipped and
only the cryptographic signature and document integrity are checked.
"""

import sys

from vanillapdf import (
    Document,
    SignatureVerificationSettings,
    TrustedCertificateStore,
)


def verify(path: str, certs_dir: str = None) -> None:
    with Document(path) as doc:
        form = doc.get_catalog().get_acro_form()
        if form is None:
            print(f"{path}: no interactive form / signatures")
            return

        with (
            TrustedCertificateStore.create() as store,
            SignatureVerificationSettings.create() as settings,
        ):
            if certs_dir:
                store.load_from_directory(certs_dir)
            else:
                settings.skip_certificate_validation = True

            found = False
            for field in form.fields:
                signature_field = field.as_signature_field()
                if signature_field is None:
                    continue
                found = True
                signature = signature_field.value
                with signature, signature.verify(doc, store, settings) as result:
                    name = result.signer_common_name or "<unknown>"
                    print(
                        f"signature by {name}: status={result.status.name}, "
                        f"valid={result.is_signature_valid}, "
                        f"intact={result.is_document_intact}, "
                        f"trusted={result.is_certificate_trusted}"
                    )

            if not found:
                print(f"{path}: no signature fields")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        raise SystemExit("usage: python verify_signature.py signed.pdf [certs_dir]")
    verify(sys.argv[1], sys.argv[2] if len(sys.argv) > 2 else None)
