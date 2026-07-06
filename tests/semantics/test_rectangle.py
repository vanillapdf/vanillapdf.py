import pytest

from vanillapdf import Rectangle


def test_create_and_release():
    obj = Rectangle.create()
    assert obj._handle is not None
    obj.close()
    assert obj._handle is None
    obj.close()  # idempotent


def test_coordinates():
    with Rectangle.create() as rect:
        rect.lower_left_x = 10
        rect.lower_left_y = 20
        rect.upper_right_x = 110
        rect.upper_right_y = 220
        assert rect.lower_left_x == 10
        assert rect.lower_left_y == 20
        assert rect.upper_right_x == 110
        assert rect.upper_right_y == 220
        assert rect.width == 100
        assert rect.height == 200
        assert "Rectangle" in repr(rect)


def test_use_after_close_raises():
    obj = Rectangle.create()
    obj.close()
    with pytest.raises(ValueError):
        _ = obj.lower_left_x
