import threading
import vanillapdf
from vanillapdf import IntegerObject, Errors, PdfError, ObjectType

THREADS = 8
ITERATIONS = 150


def _run(worker, count=THREADS):
    """Start `count` threads on `worker` (which takes a start barrier) and
    join them."""
    barrier = threading.Barrier(count)
    threads = [threading.Thread(target=worker, args=(barrier,)) for _ in range(count)]
    for t in threads:
        t.start()
    for t in threads:
        t.join()


def test_concurrent_reads_from_shared_file(test_pdf):
    """Many threads reading the SAME open File concurrently must get consistent
    results (shared native handle access, mirroring the upstream threaded test)."""
    failures = []

    with vanillapdf.File(test_pdf) as shared:
        shared.initialize()

        def worker(barrier):
            barrier.wait()  # release all threads together to maximize overlap
            try:
                for _ in range(ITERATIONS):
                    with shared.get_indirect_object(2, 0) as obj:
                        assert obj.object_type == ObjectType.DICTIONARY
                        assert len(obj) > 0
            except Exception as exc:  # noqa: BLE001
                failures.append(repr(exc))

        _run(worker)

    assert failures == []


def test_concurrent_reads_from_shared_document(test_pdf):
    """Many threads traversing the SAME open Document (shared catalog / page
    tree) concurrently must read consistent page data."""
    failures = []

    with vanillapdf.Document(test_pdf) as doc:
        pages = doc.get_catalog().get_pages()

        def worker(barrier):
            barrier.wait()
            try:
                for _ in range(ITERATIONS):
                    media_box = pages[0].media_box
                    assert (media_box.upper_right_x, media_box.upper_right_y) == (612, 792)
            except Exception as exc:  # noqa: BLE001
                failures.append(repr(exc))

        _run(worker)

    assert failures == []


def test_error_state_is_thread_local():
    """A thread hitting an error must not disturb another thread's successful
    operations - the library keeps its last-error state thread-locally, and
    raise_last_error reads it inline on the failing thread."""
    failures = []

    def success_worker(barrier):
        barrier.wait()
        for i in range(ITERATIONS):
            try:
                obj = IntegerObject.create(i)
                if obj.value != i:
                    failures.append(f"value mismatch: {obj.value} != {i}")
                obj.close()
            except PdfError as exc:  # must never happen on the success path
                failures.append(f"unexpected PdfError {exc.error_code}")

    def error_worker(barrier):
        barrier.wait()
        for _ in range(ITERATIONS):
            try:
                Errors.get_printable_error_text(999)  # invalid code -> raises
                failures.append("error path did not raise")
            except PdfError:
                pass

    barrier = threading.Barrier(THREADS)
    workers = ([success_worker] * (THREADS // 2)) + ([error_worker] * (THREADS // 2))
    threads = [threading.Thread(target=w, args=(barrier,)) for w in workers]
    for t in threads:
        t.start()
    for t in threads:
        t.join()

    assert failures == []
