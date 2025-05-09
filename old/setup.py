from setuptools import setup, Extension

extensions = [
    Extension(
        "vanillapdf._vanillapdf",
        sources=[
            "src/vanillapdf_c/vanillapdfmodule.c",
            "src/vanillapdf_c/documentmodule.c",
        ],
        include_dirs=["/path/to/vanillapdf/include"],
        libraries=["vanillapdf"],
        library_dirs=["/path/to/vanillapdf/lib"],
    )
]

setup(
    ext_modules=extensions,
)
