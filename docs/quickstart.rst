Quickstart
==========

Install the package (see :doc:`installation`), then open a document and read it.

Open and read a document
-------------------------

.. code-block:: python

   import vanillapdf

   with vanillapdf.Document("input.pdf") as doc:
       # Document information (each entry may be None if absent).
       info = doc.get_document_info()
       if info is not None and info.title is not None:
           print("Title:", info.title.value_string())

       # Traverse pages (the page tree is a 0-based Python sequence).
       pages = doc.get_catalog().get_pages()
       print(len(pages), "page(s)")
       for page in pages:
           box = page.media_box
           print(box.width, "x", box.height)

Create a document
-----------------

.. literalinclude:: ../examples/create_document.py
   :language: python
   :start-after: from vanillapdf import Document, PageObject, Rectangle
   :end-before: if __name__

**Key patterns to notice:**

- **Context managers** -- ``with`` releases native handles deterministically at
  the end of the block. Every handle-owning class supports it.
- **Errors** -- a failing library call raises
  :class:`~vanillapdf.PdfError` with a numeric ``error_code``; catch it to
  handle library failures.
- **Optional entries are None** -- missing document-info fields and similar
  optional lookups return ``None`` instead of raising.

Low-level object access
-----------------------

To reach raw PDF objects by number, use the syntax layer via
:class:`~vanillapdf.File`:

.. code-block:: python

   from vanillapdf import File, ObjectType

   with File("input.pdf") as pdf:
       pdf.initialize()  # parse xref tables first
       with pdf.get_indirect_object(2, 0) as obj:
           if obj.object_type == ObjectType.DICTIONARY:
               for key, value in obj.items():
                   print(key.value_string(), value.object_type.name)

Next steps
----------

- :doc:`examples` -- more runnable scripts
- :doc:`api/documents` -- the semantic document model
- :doc:`api/objects` -- the syntax object model
