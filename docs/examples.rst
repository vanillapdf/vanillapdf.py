Examples
========

Runnable scripts live in the `examples/
<https://github.com/vanillapdf/vanillapdf.py/tree/main/examples>`_ directory.
Each is self-contained; the source is embedded below.

Create a document
-----------------

Create a document, add a blank page with a media box, and save it.

.. literalinclude:: ../examples/create_document.py
   :language: python

Read document metadata
----------------------

Print the document information dictionary, skipping entries that are absent.

.. literalinclude:: ../examples/read_metadata.py
   :language: python

Inspect pages
-------------

Traverse the page tree and print each page's media box.

.. literalinclude:: ../examples/inspect_pages.py
   :language: python

Traverse low-level objects
--------------------------

Resolve an indirect object through the syntax layer and walk its dictionary.

.. literalinclude:: ../examples/traverse_objects.py
   :language: python

Encrypt a document
------------------

Encrypt a document with AES-128 and a user + owner password. To decrypt, open
the file, ``initialize()``, call :meth:`~vanillapdf.File.set_encryption_password`
with the password, then :meth:`~vanillapdf.Document.remove_encryption` and save.

.. literalinclude:: ../examples/encrypt_document.py
   :language: python

Sign a document
---------------

Sign a document with a PKCS#12 key. Signing writes to a destination
:class:`~vanillapdf.File` created with :meth:`~vanillapdf.File.create`, rather
than calling :meth:`~vanillapdf.Document.save`.

.. literalinclude:: ../examples/sign_document.py
   :language: python

Verify a signature
------------------

Walk a document's signature fields and verify each signature. Navigation goes
Catalog → AcroForm → fields → signature field → digital signature.

.. literalinclude:: ../examples/verify_signature.py
   :language: python
