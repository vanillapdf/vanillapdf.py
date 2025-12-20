import vanillapdf


def test_library_info_version_components():
    major = vanillapdf.LibraryInfo.get_version_major()
    minor = vanillapdf.LibraryInfo.get_version_minor()
    patch = vanillapdf.LibraryInfo.get_version_patch()
    build = vanillapdf.LibraryInfo.get_version_build()

    assert isinstance(major, int)
    assert isinstance(minor, int)
    assert isinstance(patch, int)
    assert isinstance(build, int)
    assert major >= 0
    assert minor >= 0
    assert patch >= 0
    assert build >= 0


def test_library_info_version_string():
    version = vanillapdf.LibraryInfo.get_version()
    assert isinstance(version, str)
    parts = version.split(".")
    assert len(parts) == 4


def test_library_info_author():
    author = vanillapdf.LibraryInfo.get_author()
    assert isinstance(author, str)
    assert len(author) > 0


def test_library_info_build_date():
    from datetime import date

    build_date = vanillapdf.LibraryInfo.get_build_date()
    assert isinstance(build_date, date)
    assert build_date.year >= 2020


def test_library_info_build_components():
    day = vanillapdf.LibraryInfo.get_build_day()
    month = vanillapdf.LibraryInfo.get_build_month()
    year = vanillapdf.LibraryInfo.get_build_year()

    assert 1 <= day <= 31
    assert 1 <= month <= 12
    assert year >= 2020


test_library_info_version_components()
test_library_info_version_string()
test_library_info_author()
test_library_info_build_date()
test_library_info_build_components()
