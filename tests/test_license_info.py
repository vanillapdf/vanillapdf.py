import vanillapdf


def test_license_is_valid_returns_bool():
    """is_valid() reports the (optional) license state as a bool. Licensing is
    opt-in and not enforced, so we assert the type, not a specific value."""
    result = vanillapdf.LicenseInfo.is_valid()
    assert isinstance(result, bool)


def test_license_is_temporary_returns_bool():
    result = vanillapdf.LicenseInfo.is_temporary()
    assert isinstance(result, bool)
