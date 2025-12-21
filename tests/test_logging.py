import vanillapdf


def test_logging_get_severity():
    severity = vanillapdf.Logging.get_severity()
    assert isinstance(severity, vanillapdf.LoggingSeverity)


def test_logging_set_severity():
    # Save original severity
    original = vanillapdf.Logging.get_severity()

    # Set to DEBUG
    vanillapdf.Logging.set_severity(vanillapdf.LoggingSeverity.DEBUG)
    assert vanillapdf.Logging.get_severity() == vanillapdf.LoggingSeverity.DEBUG

    # Set to WARNING
    vanillapdf.Logging.set_severity(vanillapdf.LoggingSeverity.WARNING)
    assert vanillapdf.Logging.get_severity() == vanillapdf.LoggingSeverity.WARNING

    # Restore original
    vanillapdf.Logging.set_severity(original)


def test_logging_severity_enum():
    assert vanillapdf.LoggingSeverity.TRACE < vanillapdf.LoggingSeverity.DEBUG
    assert vanillapdf.LoggingSeverity.DEBUG < vanillapdf.LoggingSeverity.INFO
    assert vanillapdf.LoggingSeverity.INFO < vanillapdf.LoggingSeverity.WARNING
    assert vanillapdf.LoggingSeverity.WARNING < vanillapdf.LoggingSeverity.ERROR
    assert vanillapdf.LoggingSeverity.ERROR < vanillapdf.LoggingSeverity.CRITICAL
    assert vanillapdf.LoggingSeverity.CRITICAL < vanillapdf.LoggingSeverity.OFF


def test_logging_set_pattern():
    # Just verify it doesn't throw
    vanillapdf.Logging.set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v")
