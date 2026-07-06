Utilities
=========

Buffers, error handling, logging, and library metadata.

Buffer
------

.. autoclass:: vanillapdf.Buffer
   :members:

Errors
------

.. py:exception:: vanillapdf.PdfError

   Raised when a VanillaPDF library call fails. Instances carry the numeric
   library error code in the ``error_code`` attribute. Error state is recorded
   thread-locally by the library, so failures on one thread do not affect
   another.

.. autoclass:: vanillapdf.Errors
   :members:

Logging
-------

.. autoclass:: vanillapdf.Logging
   :members:

.. autoclass:: vanillapdf.LoggingSeverity
   :members:
   :undoc-members:

Library information
-------------------

.. autoclass:: vanillapdf.LibraryInfo
   :members:

License
-------

.. autoclass:: vanillapdf.LicenseInfo
   :members:

Signing keys
------------

.. autoclass:: vanillapdf.PKCS12Key
   :members:

.. autoclass:: vanillapdf.SigningKey
   :members:

Signature verification
----------------------

.. autoclass:: vanillapdf.TrustedCertificateStore
   :members:

.. autoclass:: vanillapdf.SignatureVerificationSettings
   :members:

.. autoclass:: vanillapdf.SignatureVerificationResult
   :members:

.. autoclass:: vanillapdf.SignatureVerificationStatus
   :members:
   :undoc-members:

Miscellaneous
-------------

.. autoclass:: vanillapdf.MiscUtils
   :members:
