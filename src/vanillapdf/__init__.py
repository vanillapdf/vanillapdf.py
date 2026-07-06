from importlib.metadata import PackageNotFoundError
from importlib.metadata import version as _metadata_version

from .semantics.catalog import Catalog
from .semantics.digital_signature import DigitalSignature
from .semantics.document import Document
from .semantics.document_encryption_settings import (
    DocumentEncryptionSettings,
    EncryptionAlgorithm,
    UserAccessPermission,
)
from .semantics.document_info import DocumentInfo
from .semantics.document_signature_settings import (
    DocumentSignatureSettings,
    MessageDigestAlgorithm,
)
from .semantics.fields import Field, FieldCollection, FieldType, SignatureField
from .semantics.interactive_form import InteractiveForm
from .semantics.page_object import PageObject
from .semantics.page_tree import PageTree
from .semantics.rectangle import Rectangle
from .syntax.file import File, PDFVersion
from .syntax.objects import (
    ArrayObject,
    BooleanObject,
    DictionaryObject,
    HexadecimalStringObject,
    IndirectReferenceObject,
    IntegerObject,
    LiteralStringObject,
    NameObject,
    NullObject,
    Object,
    ObjectType,
    RealObject,
    StreamObject,
    StringObject,
    StringType,
)
from .utils.buffer import Buffer
from .utils.errors import Errors, PdfError
from .utils.library_info import LibraryInfo
from .utils.license_info import LicenseInfo
from .utils.logging import Logging, LoggingSeverity
from .utils.misc_utils import MiscUtils
from .utils.pkcs12_key import PKCS12Key
from .utils.signature_verifier import (
    SignatureVerificationResult,
    SignatureVerificationSettings,
    SignatureVerificationStatus,
    TrustedCertificateStore,
)
from .utils.signing_key import SigningKey

# Read the installed distribution metadata rather than importing the generated
# _version.py (which is absent from a source checkout). This is setuptools-scm's
# recommended runtime approach and keeps the type checker happy.
try:
    __version__ = _metadata_version("vanillapdf")
except PackageNotFoundError:
    __version__ = "0.0.0.dev"

__all__ = [
    "Document",
    "Buffer",
    "File",
    "PDFVersion",
    "LibraryInfo",
    "Logging",
    "LoggingSeverity",
    "PdfError",
    "Errors",
    "LicenseInfo",
    "MiscUtils",
    "SigningKey",
    "PKCS12Key",
    "TrustedCertificateStore",
    "SignatureVerificationSettings",
    "SignatureVerificationResult",
    "SignatureVerificationStatus",
    "ObjectType",
    "StringType",
    "Object",
    "NullObject",
    "BooleanObject",
    "IntegerObject",
    "RealObject",
    "NameObject",
    "StringObject",
    "LiteralStringObject",
    "HexadecimalStringObject",
    "ArrayObject",
    "DictionaryObject",
    "StreamObject",
    "IndirectReferenceObject",
    "Catalog",
    "PageTree",
    "PageObject",
    "Rectangle",
    "DocumentInfo",
    "DocumentEncryptionSettings",
    "EncryptionAlgorithm",
    "UserAccessPermission",
    "DocumentSignatureSettings",
    "MessageDigestAlgorithm",
    "InteractiveForm",
    "FieldCollection",
    "Field",
    "SignatureField",
    "FieldType",
    "DigitalSignature",
    "__version__",
]
