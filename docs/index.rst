VanillaPDF Python Bindings
==========================

Python bindings for `Vanilla.PDF <https://github.com/vanillapdf/vanillapdf>`_, a
high-performance C++17 library for creating, editing, and analyzing PDF
documents.

.. code-block:: python

   import vanillapdf

   with vanillapdf.Document("input.pdf") as doc:
       info = doc.get_document_info()
       print(info.title.value_string())

       for page in doc.get_catalog().get_pages():
           box = page.media_box
           print(box.width, box.height)

.. toctree::
   :maxdepth: 2
   :caption: Getting Started

   overview
   installation
   quickstart

.. toctree::
   :maxdepth: 2
   :caption: Learning

   examples

.. toctree::
   :maxdepth: 2
   :caption: API Reference

   api/documents
   api/files
   api/objects
   api/utils

Indices
-------

* :ref:`genindex`
* :ref:`modindex`
