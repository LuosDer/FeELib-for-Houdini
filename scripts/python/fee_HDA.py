
import hou
import os

import fee_Utils
# from importlib import reload
# reload(fee_Utils)


# import fee_HDA
# from importlib import reload
# reload(fee_HDA)


def convertDefi(inputNodeType):
    if isinstance(inputNodeType, hou.HDADefinition):
        return inputNodeType
    elif isinstance(inputNodeType, hou.NodeType):
        return inputNodeType.definition()
    elif isinstance(inputNodeType, hou.Node):
        return inputNodeType.type().definition()
    else:
        raise TypeError('Invalid Type')

def convertNodeType(inputNodeType):
    if isinstance(inputNodeType, hou.NodeType):
        return inputNodeType
    elif isinstance(inputNodeType, hou.Node):
        return inputNodeType.type()
    elif isinstance(inputNodeType, hou.HDADefinition):
        return inputNodeType.nodeType()
    else:
        raise TypeError('Invalid Type')


def readDeprecatedNode():
    deprecatedNodeList = []

    pathFeELib = hou.getenv('FeELib')
    if pathFeELib is not None:
        deprecatedSopDefinitionTXT = pathFeELib + '/scripts/deprecatedDefinition/deprecatedSopDefinition.txt'
        if os.path.exists(deprecatedSopDefinitionTXT):
            with open(deprecatedSopDefinitionTXT, "r") as TXT:
                fee_Utils.readTXTAsList(deprecatedNodeList, TXT)

    pathFeELib = hou.getenv('FeEProjectHoudini')
    if pathFeELib is not None:
        deprecatedSopDefinitionTXT = pathFeELib + '/scripts/deprecatedDefinition/deprecatedSopDefinition.txt'
        if os.path.exists(deprecatedSopDefinitionTXT):
            with open(deprecatedSopDefinitionTXT, "r") as TXT:
                fee_Utils.readTXTAsList(deprecatedNodeList, TXT)
    
    pathFeELib = hou.getenv('FeEworkHoudini')
    if pathFeELib is not None:
        deprecatedSopDefinitionTXT = pathFeELib + '/scripts/deprecatedDefinition/deprecatedSopDefinition.txt'
        if os.path.exists(deprecatedSopDefinitionTXT):
            with open(deprecatedSopDefinitionTXT, "r") as TXT:
                fee_Utils.readTXTAsList(deprecatedNodeList, TXT)

    deprecatedNodeList = list(set(deprecatedNodeList))
    return deprecatedNodeList


def readNeedHideNode():
    deprecatedNodeList = []
    pathFeELib = hou.getenv('FeELib')
    if pathFeELib is not None:
        deprecatedSopDefinitionTXT = pathFeELib + '/scripts/deprecatedDefinition/needHideSopNode.txt'
        if os.path.exists(deprecatedSopDefinitionTXT):
            with open(deprecatedSopDefinitionTXT, "r") as TXT:
                fee_Utils.readTXTAsList(deprecatedNodeList, TXT)

    pathFeELib = hou.getenv('FeEProjectHoudini')
    if pathFeELib is not None:
        deprecatedSopDefinitionTXT = pathFeELib + '/scripts/deprecatedDefinition/needHideSopNode.txt'
        if os.path.exists(deprecatedSopDefinitionTXT):
            with open(deprecatedSopDefinitionTXT, "r") as TXT:
                fee_Utils.readTXTAsList(deprecatedNodeList, TXT)

    pathFeELib = hou.getenv('FeEworkHoudini')
    if pathFeELib is not None:
        deprecatedSopDefinitionTXT = pathFeELib + '/scripts/deprecatedDefinition/needHideSopNode.txt'
        if os.path.exists(deprecatedSopDefinitionTXT):
            with open(deprecatedSopDefinitionTXT, "r") as TXT:
                fee_Utils.readTXTAsList(deprecatedNodeList, TXT)

    deprecatedNodeList = list(set(deprecatedNodeList))
    return deprecatedNodeList



def isUnlockedHDA(node):
    return not node.isLockedHDA() and node.type().definition() is not None


def isFeENode(inputNodeType, detectName = True, detectPath = False):
    nodeType = convertNodeType(inputNodeType)

    if detectName:
        nameComponents = nodeType.nameComponents()
        if not ( ('FeE' in nameComponents[1] or 'Five elements Elf' in nameComponents[1]) or (nameComponents[2].endswith("_fee") and nodeType.description().startswith("FeE")) ):
            return False
    
    if detectPath:
        defi = nodeType.definition()
        if defi is None:
            libraryFilePath = 'None'
        else:
            libraryFilePath = defi.libraryFilePath()
        
        pathFeELib = hou.getenv('FeELib')
        if pathFeELib is None:
            raise AttributeError('not found env: FeELib')

        if pathFeELib is not None:
            isInPathCondition = pathFeELib in libraryFilePath

        pathFeELib = hou.getenv('FeEProjectHoudini')
        if pathFeELib is not None:
            isInPathCondition = isInPathCondition or pathFeELib in libraryFilePath

        pathFeELib = hou.getenv('FeEworkHoudini')
        if pathFeELib is not None:
            isInPathCondition = isInPathCondition or pathFeELib in libraryFilePath

        if not isInPathCondition:
            return False
    
    return True

def isSideFXLabsNode(inputNodeType, detectName = True, detectPath = False):
    nodeType = convertNodeType(inputNodeType)

    if detectName:
        nameComponents = nodeType.nameComponents()
        if not 'labs' in nameComponents[1]:
            return False
    
    if detectPath:
        defi = nodeType.definition()
        if defi is None:
            libraryFilePath = 'None'
        else:
            libraryFilePath = defi.libraryFilePath()
        
        pathFeELib = hou.getenv('SIDEFXLABS')
        if pathFeELib is None:
            raise AttributeError('not found env: SIDEFXLABS')

        isInPathCondition = pathFeELib in libraryFilePath

        if not isInPathCondition:
            return False
    
    return True


def isSideFXNode(inputNodeType, ignoreDSO = True):
    nodeType = convertNodeType(inputNodeType)
    
    defaultLibPath = hou.getenv('HFS') + r'/houdini/otls/'
    defi = nodeType.definition()
    if ignoreDSO and defi is None:
        return False
    return defi.libraryFilePath().startswith(defaultLibPath)


def isSideFXDefinition(defi):
    defaultLibPath = hou.getenv('HFS') + r'/houdini/otls/'
    return defi.libraryFilePath().startswith(defaultLibPath)



def checkHideFeENode(keepHide = True, detectName = False, detectPath = True):
    ############### keepHide is False 用于切换hide模式
    TEMP_path = hou.getenv('TEMP')
    isHidingTXT_path = TEMP_path + '/isHidingFeENode.txt'

    if os.path.exists(isHidingTXT_path):
        with open(isHidingTXT_path, "r") as isHidingTXT:
            isHiding = int(isHidingTXT.read(1)) == 1
            #print(isHiding)
            
        if not keepHide:
            with open(isHidingTXT_path, "w") as isHidingTXT:
                isHidingTXT.write('0' if isHiding else '1')
    else:
        isHiding = False
        if not keepHide:
            #os.makedirs(isHidingTXT_path)
            with open(isHidingTXT_path, "w") as isHidingTXT:
                isHidingTXT.write('1')


    deprecatedNodeList = readDeprecatedNode()
    deprecatedNodeList.extend(readNeedHideNode())
    
    nodeTypeCategories = hou.nodeTypeCategories()

    for nodeTypeCategoriesKey in nodeTypeCategories:
        # run over all nodeType
        dictNodeTypes = nodeTypeCategories[nodeTypeCategoriesKey].nodeTypes()
        for nodeTypeDictsKey in dictNodeTypes:
            nodeType = dictNodeTypes[nodeTypeDictsKey]

            nodeTypeName = nodeType.name()
            if nodeTypeName in deprecatedNodeList:
                #print(nodeTypeName)
                nodeType.setHidden(True)
                continue
    

    if keepHide:
        if isHiding:
            hideToggle = True
        else:
            return
    else:
        hideToggle = not isHiding
    

    for nodeTypeCategoriesKey in nodeTypeCategories:
        # run over all nodeType
        dictNodeTypes = nodeTypeCategories[nodeTypeCategoriesKey].nodeTypes()
        for nodeTypeDictsKey in dictNodeTypes:
            nodeType = dictNodeTypes[nodeTypeDictsKey]

            '''
            defi = nodeType.definition()
            if not defi:
                continue
            '''
        
            if not isFeENode(nodeType, detectName = detectName, detectPath = detectPath):
                continue

            nodeType.setHidden(hideToggle)
            """
            if 1:
                #if nodeType.hidden():
            else:
                pass
                # operation = 'opunhide' if isHiding else 'ophide'
                # hou.hscript(operation + ' Sop ' + nodeType.name())
            """






def findAllSubParmRawValue(subnet, strValue):
    for child in subnet.allSubChildren(recurse_in_locked_nodes=False):
        for parm in child.parms():
            if strValue in parm.rawValue():
                print(child)
                print(parm)




def findAllSubParmRawValue(subnet, strValue):
    for child in subnet.allSubChildren(recurse_in_locked_nodes=False):
        for parm in child.parms():
            if strValue in parm.rawValue():
                print(child)
                print(parm)








def removeCategoryEmbeded(nodeTypeCategory = hou.nodeTypeCategories()['Sop'], displayConfirmation = False):
    if displayConfirmation:
        fee_Utils.displayConfirmation()

    dictNodeTypes = nodeTypeCategory.nodeTypes()

    for nodeTypeDictsKey in dictNodeTypes:
        nodeType = dictNodeTypes[nodeTypeDictsKey]
        defi = nodeType.definition()
        if not defi:
            continue
        if defi.libraryFilePath() == 'Embedded':
            defi.destroy()

def removeEmbeded(displayConfirmation = False):
    if displayConfirmation:
        fee_Utils.displayConfirmation()

    nodeTypeCategories = hou.nodeTypeCategories()

    for nodeTypeCategoryKey in nodeTypeCategories:
        nodeTypeCategory = nodeTypeCategories[nodeTypeCategoryKey]
        removeCategoryEmbeded(nodeTypeCategory = nodeTypeCategory)

def hasEmbeded(netCategories):
    dictNodeTypes = netCategories.nodeTypes()
    for nodeTypeDictsKey in dictNodeTypes:
        secNodeType = dictNodeTypes[nodeTypeDictsKey]
        defi = secNodeType.definition()
        if not defi:
            continue
        if defi.libraryFilePath() == 'Embedded':
            return True
    return False


"""
def TABSubmenuPathfromDefi(defi):
    sections = defi.sections()
    try:
        sectionToolShelf = sections[r'Tools.shelf']
    except:
        print('not found section: Tools Shelf:', end = ' ')
        print(nodeType)
    contents = sectionToolShelf.contents()#hou.compressionType.NoCompression
    #print(contents)

    '''
    try:
        import xml.etree.cElementTree as ET
    except ImportError:
        import xml.etree.ElementTree as ET
    '''
    import xml.etree.ElementTree as ET

    tree = ET.fromstring(str(contents))
    toolSubmenu = tree.find('tool').find('toolSubmenu').text
"""


def TABSubmenuPathfromContents(contents):
    '''
    try:
        import xml.etree.cElementTree as ET
    except ImportError:
        import xml.etree.ElementTree as ET
    '''
    import xml.etree.ElementTree as ET

    tree = ET.fromstring(str(contents))
    toolSubmenu = tree.find('tool').find('toolSubmenu').text

def TABSubmenuPathfromSectionToolShelf(sectionToolShelf):
    contents = sectionToolShelf.contents()#hou.compressionType.NoCompression
    return TABSubmenuPathfromContents(contents)


def TABSubmenuPathfromSections(sections):
    try:
        sectionToolShelf = sections[r'Tools.shelf']
        return TABSubmenuPathfromSectionToolShelf(sectionToolShelf)
    except:
        print('not found section: Tools Shelf:', end = ' ')
        print(nodeType)
        return None
    



def TABSubmenuPathfromDefi(defi):
    sections = defi.sections()
    return TABSubmenuPathfromSections(sections)



def extractAllUsedDefiToEmbeded(ignoreNodeTypeNames = (), ignoreFeEHDA = False):
    fee_Utils.displayConfirmation(prevText = "This tool can copy all used HDA under all selectedNodes's allSubChildren to otls/ folder by the HIP path and also can convert those HDA to Black Box \n这个工具可以把所有 selectedNodes(选择的节点) 的 allSubChildren(子节点) 内用到的HDA复制到HIP路径下的otls/文件夹内 并可选转为Black Box")

    toBlackBox = hou.ui.displayMessage('Convert to Black Box?\n转为黑盒？', buttons = ("yes", "no"), default_choice = 0, close_choice = 0)
    toBlackBox = toBlackBox == 0

    hdaFileType = hou.ui.displayMessage('HDA File Type\nHDA类型？', buttons=('Seperated', 'All in One'), default_choice = 1, close_choice = 1)
    hdaFileType = hdaFileType == 1

    HIPPath = hou.hipFile.path()
    save_OTLFolderPath = os.path.split(HIPPath)[0] + '/otls/'
    if os.path.exists(save_OTLFolderPath):
        #print(save_OTLFolderPath)
        #print(os.listdir(save_OTLFolderPath))
        for subDir in os.listdir(save_OTLFolderPath):
            subFullDir = save_OTLFolderPath + subDir
            if not os.path.isfile(subFullDir):
                continue
            #print(os.path.splitext(subDir))
            if os.path.splitext(subDir)[1].lower() != '.hda':
                continue
            try:
                hou.hda.uninstallFile(subFullDir)
            except:
                pass
            os.remove(subFullDir)
    else:
        os.mkdir(save_OTLFolderPath)
    
    save_OTLPath = save_OTLFolderPath
    if toBlackBox:
        save_OTLPath += '/allUsedHDAs_blackBox.hda'
    else:
        save_OTLPath += '/allUsedHDAs.hda'

    if not os.path.exists(save_OTLFolderPath):
        os.makedirs(save_OTLFolderPath)

    convertedDefi = []
    if hdaFileType:
        newHDAFilePaths = [save_OTLPath]
    else:
        newHDAFilePaths = []

    
    allowEditingOfContentsNodes = []

    selectedNodes = hou.selectedNodes()
    for selectedNode in selectedNodes:
        for allSubChild in selectedNode.allSubChildren():
            if not allSubChild.matchesCurrentDefinition():
                continue
            nodeType = allSubChild.type()
            defi = nodeType.definition()
            if defi is None:
                continue
            if defi in convertedDefi:
                continue
            #if fee_HDA.isSideFXDefinition(defi):
            if isSideFXDefinition(defi):
                continue
            if ignoreFeEHDA:
                #if fee_HDA.isFeENode(nodeType):
                if isFeENode(nodeType):
                    continue
            nodeTypeName = allSubChild.type().name()
            if nodeTypeName in ignoreNodeTypeNames:
                continue

            if not hdaFileType:
                defiPath = defi.libraryFilePath()
                save_OTLPath = save_OTLFolderPath + '/' + os.path.split(defiPath)[1]

            allSubChild.allowEditingOfContents(propagate=True)
            allowEditingOfContentsNodes.append(allSubChild)
            if toBlackBox:
                defi.save(save_OTLPath, template_node = allSubChild, compile_contents=True, black_box=True, create_backup=False)
                #defi.save(save_OTLPath, compile_contents=False, black_box=True, create_backup=False)
            else:
                defi.save(save_OTLPath, template_node = allSubChild, create_backup=False)
                
            convertedDefi.append(defi)
            
            if not hdaFileType and save_OTLPath not in newHDAFilePaths:
                newHDAFilePaths.append(save_OTLPath)

    for node in allowEditingOfContentsNodes:
        node.matchCurrentDefinition()
    
    for newHDAFilePath in newHDAFilePaths:
        hou.hda.installFile(newHDAFilePath)
    

    #hou.hda.uninstallFile(defiPath)
    #convertedDefi = list(set(convertedDefi)) #去重

