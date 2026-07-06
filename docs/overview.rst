Overview
========

The VanillaPDF Python bindings wrap the `Vanilla.PDF
<https://github.com/vanillapdf/vanillapdf>`_ C++17 library, exposing its
handle-based C API as idiomatic Python classes with context managers, iteration,
and native exceptions.

Two layers
----------

The API mirrors the library's two-layer structure:

**Syntax layer** -- the raw PDF object model. :class:`~vanillapdf.File` gives
low-level access to a parsed file, and :class:`~vanillapdf.Object` and its
subclasses (:class:`~vanillapdf.IntegerObject`,
:class:`~vanillapdf.DictionaryObject`, :class:`~vanillapdf.ArrayObject`,
:class:`~vanillapdf.StreamObject`, ...) represent PDF objects. Arrays and
dictionaries support Python ``len()``, indexing, and iteration.

**Semantic layer** -- the high-level document model.
:class:`~vanillapdf.Document`, :class:`~vanillapdf.Catalog`,
:class:`~vanillapdf.PageTree`, :class:`~vanillapdf.PageObject`, and
:class:`~vanillapdf.Rectangle` model documents, pages, and their geometry. The
page tree is a 0-based Python sequence.

Resource management
-------------------

Every class that owns a native handle is a context manager and has an idempotent
:meth:`close`. Use ``with`` (or call ``close()``) to release handles
deterministically; a ``__del__`` safety net releases anything you forget.

.. code-block:: python

   with vanillapdf.Document("input.pdf") as doc:
       ...  # handle released at the end of the block

Accessing a closed object raises :class:`ValueError`.

Errors
------

When a library call fails it raises :class:`~vanillapdf.PdfError`, which carries
the library's numeric ``error_code``. Error state is stored thread-locally by
the library, so failures on one thread never disturb another. Missing *optional*
entries (for example an absent document-info field) are returned as ``None``
rather than raising.

Thread safety
-------------

The underlying library is thread-safe and keeps its error state per-thread. The
bindings currently hold the CPython GIL across native calls, so threads are
serialized at the C boundary; true parallelism is tracked in
`issue #40 <https://github.com/vanillapdf/vanillapdf.py/issues/40>`_.
