import threading
from vanillapdf import IntegerObject, Errors, PdfError


def test_concurrent_object_creation():
    """Many threads creating/reading/closing objects concurrently must not
    corrupt each other (capsule + refcount safety under the GIL)."""
    errors = []

    def worker(n):
        try:
            for i in range(300):
                obj = IntegerObject.create(n * 100000 + i)
                assert obj.value == n * 100000 + i
                obj.close()
        except Exception as exc:  # noqa: BLE001
            errors.append(repr(exc))

    threads = [threading.Thread(target=worker, args=(t,)) for t in range(8)]
    for t in threads:
        t.start()
    for t in threads:
        t.join()

    assert errors == []


def test_error_state_is_thread_local():
    """A thread hitting an error must not disturb another thread's successful
    operations - the library keeps its last-error state thread-locally, and
    raise_last_error reads it inline on the failing thread."""
    failures = []

    def success_worker():
        for i in range(300):
            try:
                obj = IntegerObject.create(i)
                if obj.value != i:
                    failures.append(f"value mismatch: {obj.value} != {i}")
                obj.close()
            except PdfError as exc:  # must never happen on the success path
                failures.append(f"unexpected PdfError {exc.error_code}")

    def error_worker():
        for _ in range(300):
            try:
                Errors.get_printable_error_text(999)  # invalid code -> raises
                failures.append("error path did not raise")
            except PdfError:
                pass

    threads = ([threading.Thread(target=success_worker) for _ in range(4)]
               + [threading.Thread(target=error_worker) for _ in range(4)])
    for t in threads:
        t.start()
    for t in threads:
        t.join()

    assert failures == []
