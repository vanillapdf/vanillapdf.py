import vanillapdf


def test_initialize_openssl_runs():
    """Initializing OpenSSL should succeed and return None.

    Note: we intentionally do not call cleanup_openssl() here, since tearing
    down the crypto backend can affect other tests (e.g. encrypted document
    handling) depending on execution order.
    """
    assert vanillapdf.MiscUtils.initialize_openssl() is None
