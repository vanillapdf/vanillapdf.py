Objects
=======

The syntax layer: PDF object types. Every object is a :class:`~vanillapdf.Object`
subclass; container objects support Python ``len()``, indexing, and iteration.

Enumerations
------------

.. autoclass:: vanillapdf.ObjectType
   :members:
   :undoc-members:

.. autoclass:: vanillapdf.StringType
   :members:
   :undoc-members:

Base object
-----------

.. autoclass:: vanillapdf.Object
   :members:

Scalar objects
--------------

.. autoclass:: vanillapdf.NullObject
   :members:

.. autoclass:: vanillapdf.BooleanObject
   :members:

.. autoclass:: vanillapdf.IntegerObject
   :members:

.. autoclass:: vanillapdf.RealObject
   :members:

.. autoclass:: vanillapdf.NameObject
   :members:

String objects
--------------

.. autoclass:: vanillapdf.StringObject
   :members:

.. autoclass:: vanillapdf.LiteralStringObject
   :members:

.. autoclass:: vanillapdf.HexadecimalStringObject
   :members:

Container objects
-----------------

.. autoclass:: vanillapdf.ArrayObject
   :members:
   :special-members: __len__, __getitem__, __setitem__, __delitem__, __iter__

.. autoclass:: vanillapdf.DictionaryObject
   :members:
   :special-members: __len__, __getitem__, __setitem__, __delitem__, __contains__, __iter__

.. autoclass:: vanillapdf.StreamObject
   :members:

.. autoclass:: vanillapdf.IndirectReferenceObject
   :members:
