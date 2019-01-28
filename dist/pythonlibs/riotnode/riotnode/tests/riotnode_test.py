"""riotnode.__init__ tests"""
import riotnode


def test_version():
    """Test there is an `__version__` attriubte.

    Goal is to have a test to run the test environment.
    """
    assert getattr(riotnode, '__version__', None) is not None
