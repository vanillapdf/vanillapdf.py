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
