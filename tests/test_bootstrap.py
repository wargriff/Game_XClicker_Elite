"""Test bootstrap ui.py conflict fix."""

import os
import sys
import tempfile


def test_bootstrap_fixes_ui_py_conflict():
    workspace = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    if workspace not in sys.path:
        sys.path.insert(0, workspace)

    with tempfile.TemporaryDirectory() as tmp:
        os.makedirs(os.path.join(tmp, "ui"))
        open(os.path.join(tmp, "ui", "__init__.py"), "w").close()
        open(os.path.join(tmp, "ui.py"), "w").write("# conflict\n")

        from utils.bootstrap import _fix_ui_py_conflict

        _fix_ui_py_conflict(tmp)
        assert not os.path.isfile(os.path.join(tmp, "ui.py"))
        assert any("ui.py.bak" in f for f in os.listdir(tmp))
