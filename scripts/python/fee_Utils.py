
import hou
import os
import PySide2

# import fee_Utils
# from importlib import reload
# reload(fee_Utils)

def displayConfirmation(prevText='', postText=''):
    sureToRunThis = hou.ui.displayConfirmation(prevText + '\n\nAre you sure you wantS to run this\n你是否确定要运行这个工具\n\n' + postText)
    if not sureToRunThis:
        raise SystemExit('Stop Run', sureToRunThis)

def readTXTAsList(outList, txt):
    for line in txt.readlines():
        curline = line.strip()
        if curline == '':
            continue
        outList.append(curline[:])


def isFloat(inputString):
    if not isinstance(inputString, str):
        raise ValueError()
    
    try:
        returnVal = float(inputString)
        return True
    except:
        return False
    return None







def setClipboardText(text):
    '''
    import win32clipboard
    win32clipboard.SetClipboardText(path)
    win32clipboard.CloseClipboard()
    '''
    clipboard = PySide2.QtWidgets.QApplication.clipboard()
    mode = clipboard.Clipboard
    clipboard.clear(mode=mode)
    clipboard.setText(text, mode=mode)


def gitPullByBat(repositoryPath, reloadAllFiles = False):
    if repositoryPath is None:
        raise TypeError('invalid input', repositoryPath)

    TEMP_path = hou.getenv('TEMP')


    command = repositoryPath[:2] + '\n'
    command += 'cd ' + repositoryPath + '\n'
    command += 'git pull'
    #print(command)
    
    batName = 'feE_Utils_GitPull.bat'
    batPath = TEMP_path + '/' + batName
    with open(batPath, 'w') as BAT:
        BAT.write(command)
    
    os.system(batPath)

    if reloadAllFiles:
        hou.hda.reloadAllFiles(rescan = True)
        #hou.hda.reloadNamespaceOrder()

    '''
    try:
        from git.repo import Repo
        repo = Repo(repositoryPath)
        repo.git.pull(repositoryPath)
    except:
        pass
    '''
    #os.system('D:/Houdini/FeEProjectHoudini/otls/gitpull.bat')
    #os.system('D:; cd D:/Git/houdini_toolkit; git pull')
        
