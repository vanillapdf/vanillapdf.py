from . import _vanillapdf

class Document:
    def __init__(self, filename: str = None):
        if filename:
            self._handle = _vanillapdf.document_open(filename)
            if self._handle is None:
                raise RuntimeError(f"Failed to open document '{filename}'")
        else:
            raise NotImplementedError("Currently only opening existing documents is supported.")

    def save(self, filename: str):
        _vanillapdf.document_save(self._handle, filename)

    def close(self):
        if self._handle:
            _vanillapdf.document_release(self._handle)
            self._handle = None

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()

    def __del__(self):
        self.close()