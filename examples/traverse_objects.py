"""Resolve a low-level indirect object and print its dictionary entries.

This uses the syntax layer (File / Object) rather than the semantic layer
(Document / Catalog), showing how to reach raw PDF objects by number.

Usage::

    python traverse_objects.py document.pdf [object_number] [generation]
"""

import sys

from vanillapdf import File, ObjectType


def traverse_objects(path: str, object_number: int, generation: int) -> None:
    # A File must be initialized (xref tables parsed) before objects resolve.
    with File(path) as pdf:
        pdf.initialize()
        print(f"{path}: PDF {pdf.get_version_string()}, "
              f"encrypted={pdf.is_encrypted}")

        with pdf.get_indirect_object(object_number, generation) as obj:
            print(f"object {object_number} {generation} -> {obj.object_type.name}")

            # If it is a dictionary, enumerate its keys and value types.
            if obj.object_type == ObjectType.DICTIONARY:
                for key, value in obj.items():
                    print(f"  /{key.value_string()}: {value.object_type.name}")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        raise SystemExit(
            "usage: python traverse_objects.py document.pdf "
            "[object_number] [generation]"
        )
    filename = sys.argv[1]
    number = int(sys.argv[2]) if len(sys.argv) > 2 else 2
    gen = int(sys.argv[3]) if len(sys.argv) > 3 else 0
    traverse_objects(filename, number, gen)
