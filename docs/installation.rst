Installation
============

From PyPI
---------

Pre-built wheels are published for Windows, Linux, and macOS:

.. code-block:: bash

   pip install vanillapdf

Requires Python 3.10 or later. If a wheel is available for your platform and
interpreter, no C++ toolchain is needed.

From source
-----------

Building from source compiles the native ``_vanillapdf`` extension, which pulls
the Vanilla.PDF C++ library and its dependencies through vcpkg. You need CMake, a
C++17 compiler, and Ninja.

**Windows:**

.. code-block:: bat

   python -m venv venv
   venv\Scripts\activate
   pip install -e .

**Linux:**

.. code-block:: bash

   sudo apt install -y build-essential cmake ninja-build python3-dev \
       git curl unzip pkg-config
   python3 -m venv venv
   source venv/bin/activate
   pip install -e .

Install the test extras and run the suite to verify the build:

.. code-block:: bash

   pip install -e .[test]
   pytest
