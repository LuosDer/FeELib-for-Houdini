
import hou
import PySide2

import fee_Utils
# from importlib import reload
# reload(fee_Utils)


def readTXTAsList(outList, txt):
    for line in txt.readlines():
        curline = line.strip()
        if curline == '':
            continue
        outList.append(curline[:])


def setClipboardText(text):
    clipboard = PySide2.QtWidgets.QApplication.clipboard()
    mode = clipboard.Clipboard
    clipboard.clear(mode=mode)
    clipboard.setText(text, mode=mode)
