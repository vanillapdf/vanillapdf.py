import vanillapdf


def test_license_is_valid_returns_bool():
    """Without a license set, is_valid() is a bool (typically False)."""
    result = vanillapdf.LicenseInfo.is_valid()
    assert isinstance(result, bool)


def test_license_is_temporary_returns_bool():
    result = vanillapdf.LicenseInfo.is_temporary()
    assert isinstance(result, bool)
