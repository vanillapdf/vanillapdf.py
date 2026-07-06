Documents
=========

The semantic layer: the high-level document model built on top of the syntax
objects.

Document
--------

.. autoclass:: vanillapdf.Document
   :members:

Catalog
-------

.. autoclass:: vanillapdf.Catalog
   :members:

PageTree
--------

.. autoclass:: vanillapdf.PageTree
   :members:
   :special-members: __len__, __getitem__, __setitem__, __delitem__, __iter__

PageObject
----------

.. autoclass:: vanillapdf.PageObject
   :members:

Rectangle
---------

.. autoclass:: vanillapdf.Rectangle
   :members:

DocumentInfo
------------

.. autoclass:: vanillapdf.DocumentInfo
   :members:

Encryption
----------

.. autoclass:: vanillapdf.DocumentEncryptionSettings
   :members:

.. autoclass:: vanillapdf.EncryptionAlgorithm
   :members:
   :undoc-members:

.. autoclass:: vanillapdf.UserAccessPermission
   :members:
   :undoc-members:

Signing
-------

.. autoclass:: vanillapdf.DocumentSignatureSettings
   :members:

.. autoclass:: vanillapdf.MessageDigestAlgorithm
   :members:
   :undoc-members:

See also :class:`vanillapdf.PKCS12Key` and :class:`vanillapdf.SigningKey` in
:doc:`utils`.

Interactive forms & signatures
------------------------------

.. autoclass:: vanillapdf.InteractiveForm
   :members:

.. autoclass:: vanillapdf.FieldCollection
   :members:
   :special-members: __len__, __getitem__, __iter__

.. autoclass:: vanillapdf.Field
   :members:

.. autoclass:: vanillapdf.SignatureField
   :members:

.. autoclass:: vanillapdf.FieldType
   :members:
   :undoc-members:

.. autoclass:: vanillapdf.DigitalSignature
   :members:

See :class:`vanillapdf.TrustedCertificateStore`,
:class:`vanillapdf.SignatureVerificationSettings`, and
:class:`vanillapdf.SignatureVerificationResult` in :doc:`utils` for verification.
