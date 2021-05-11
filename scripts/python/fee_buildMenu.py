
import hou



def strisDetail(argString):
    if argString.find('detail')==0 or argString.find('global')==0:
        return True
    else:
        return False

def strisPrim(argString):
    if argString.find('prim')==0:
        return True
    else:
        return False

def strisPoint(argString):
    if argString.find('point')==0:
        return True
    else:
        return False

def strisEdge(argString):
    if argString.find('edge')==0:
        return True
    else:
        return False

def strisVertex(argString):
    if argString.find('vertex')==0 or argString=='vertices':
        return True
    else:
        return False


def buildAttribsMenu_sub(node, input_num=0, attribClass='point', attribType='all'):
    try:
        input = node.inputs()[input_num]
        geo = input.geometry()
        if strisPrim(attribClass):
            allAttribs = geo.primAttribs()
        elif strisPoint(attribClass):
            allAttribs = geo.pointAttribs()
        elif strisVertex(attribClass):
            allAttribs = geo.vertexAttribs()
        else:
            allAttribs = geo.globalAttribs()
        menu = []
        for attrib in allAttribs:
            if attribType=='int':
                if attrib.dataType()!=hou.attribData.Int:
                    continue
            elif attribType=='float':
                if attrib.dataType()!=hou.attribData.Float:
                    continue
            elif attribType=='string':
                if attrib.dataType()!=hou.attribData.String:
                    continue
            menu += [attrib.name()]
            menu += [attrib.name()]
    except:
        return []
    
    return menu


def buildMultiClassAttribsMenu(node, input_num=0, attribClasses=['point'], attribType='all'):
    menu = []
    for attribClass in attribClasses:
        try:
            menu += buildAttribsMenu_sub(node, input_num, attribClass, attribType)
        except:
            continue
    
    return menu


def buildAttribsMenu(node, input_num=0, attribClass='point', attribType='all'):
    if isinstance(attribClass, list) or isinstance(attribClass, tuple):
        return buildMultiClassAttribsMenu(node, input_num, attribClass, attribType)
    else:
        return buildAttribsMenu_sub(node, input_num, attribClass, attribType)



def buildAllClassAttribsMenu(node, input_num=0, attribType='all'):
    menu = []
    for attribClass in ['detail', 'prim', 'point', 'vertex']:
        try:
            menu += buildAttribsMenu(node, input_num, attribClass, attribType)
        except:
            continue
    
    return menu



def buildGroupsMenu(node, input_num=0, groupClass='point'):
    try:
        input = node.inputs()[input_num]
        geo = input.geometry()
        if strisPrim(groupClass):
            allGroups = geo.primGroups()
        elif strisPoint(groupClass):
            allGroups = geo.pointGroups()
        elif strisEdge(groupClass):
            allGroups = geo.edgeGroups()
        elif strisVertex(groupClass):
            allGroups = geo.vertexGroups()
        else:
            allGroups = geo.globalGroups()
        menu = []
        for group in allGroups:
            menu += [group.name()]
            menu += [group.name()]
    except:
        return []
    
    return menu


def buildMultiClassGroupsMenu(node, input_num=0, groupClasses=['point']):
    menu = []
    for groupClass in groupClasses:
        try:
            menu += buildGroupsMenu(node, input_num, groupClass)
        except:
            continue
    
    return menu


def buildAllClassGroupsMenu(node, input_num=0):
    menu = []
    for groupClass in ['detail', 'prim', 'point', 'edge', 'vertex']:
        try:
            menu += buildGroupsMenu(node, input_num, groupClass)
        except:
            continue
    
    return menu
