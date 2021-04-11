
import hou

# import fee_HDA
# from importlib import reload
# reload(fee_HDA)

def isFeENode(nodeType, detectName = True, detectPath = False):
    if detectName:
        if not nodeType.nameComponents()[2].endswith("_fee") or not nodeType.description().startswith("FeE"):
            return False
    if detectPath:
        defi = nodeType.definition()
        if defi is None:
            libraryFilePath = 'None'
        else:
            libraryFilePath = defi.libraryFilePath()
        pathFeELib = hou.getenv('FeELib')
        pathFeEProject = hou.getenv('FeEProjectHoudini')
        pathFeEmiHoYo = hou.getenv('FeEmiHoYo')
        if not (pathFeELib in libraryFilePath or pathFeEProject in libraryFilePath or pathFeEmiHoYo in libraryFilePath):
            return False
    
    return True

def copyParms_NodetoNode(sourceNode, targetNode):
    origParmTemplateGroup = sourceNode.parmTemplateGroup()
    if 0:
        targetNode.setParmTemplateGroup(origParmTemplateGroup, rename_conflicting_parms=False)
    else:
        try:
            targetNode.setParmTemplateGroup(origParmTemplateGroup, rename_conflicting_parms=False)
        except:
            '''Parameters don't support MinMax, MaxMin, StartEnd, BeginEnd, or XYWH parmNamingSchemes'''
            print(sourceNode, targetNode)
            print(origParmTemplateGroup.asDialogScript())
        
    for parm in sourceNode.parms():
        if parm.parmTemplate().type() == hou.parmTemplateType.Folder:
            try:
                parmVal = parm.evalAsInt()
                #print(parmVal)
                #print(parm.multiParmStartOffset())
                parentFolder = targetNode.parm(parm.name())
                #parentFolder.setFromParm(parm)
                for idx in range(0, parentFolder.evalAsInt()):
                    parentFolder.removeMultiParmInstance(0)
                for idx in range(0, parmVal):
                    parentFolder.insertMultiParmInstance(idx)
            except:
                print('')
                #print(targetNode)
                print(parm)
                
    if 0:
        for parm in targetNode.parms():
            if parm.isMultiParmInstance():
                print(parm)

    for parm in sourceNode.parms():
        #break
        try:
            targetparm = targetNode.parm(parm.name())
            targetparm.deleteAllKeyframes()
            targetparm.setFromParm(parm)
        except:
            #print(targetNode)
            print(targetNode.parm(parm.name()))
            #print(parm)
    

def convertSubnet(node, ignoreUnlock = False, Only_FeEHDA = True, ignore_SideFX_HDA = True, detectName = True, detectPath = False):
    nodeType = node.type()
    
    if nodeType.name() == 'subnet':
        convertSubnet_recurseSubChild(node, node, '', ignoreUnlock, Only_FeEHDA, ignore_SideFX_HDA)

    definition = nodeType.definition()
    if definition is None:
        return

    if ignoreUnlock==0 and not node.matchesCurrentDefinition():
        #print(node)
        convertSubnet_recurseSubChild(node, node, '', ignoreUnlock, Only_FeEHDA)
        return
    
    if Only_FeEHDA:
        if not isFeENode(nodeType, detectName, detectPath):
            return

    if ignore_SideFX_HDA:
        defaultLibPath = hou.getenv('HFS') + r'/houdini/otls/'
        if definition.libraryFilePath().startswith(defaultLibPath):
            convertSubnet_recurseSubChild(node, node, '', ignoreUnlock, Only_FeEHDA)
            return

    node.allowEditingOfContents()

    parent = node.parent()
    #print(parent.childTypeCategory().name())
    if parent.childTypeCategory().name() != 'Sop':
        raise('error')
    if node.isHardLocked():
        raise('isHardLocked')
    if node.isSoftLocked():
        raise('isSoftLocked')
    
    displayFlag = node == parent.displayNode()
    renderFlag = node == parent.renderNode()
    bypass = node.isBypassed()
    isTemplateFlagSet = node.isTemplateFlagSet()
    isHighlightFlagSet = node.isHighlightFlagSet()
    isSelectableTemplateFlagSet = node.isSelectableTemplateFlagSet()
    isUnloadFlagSet = node.isUnloadFlagSet()

    inputConnectors = node.inputConnectors()
    nInputs = len(inputConnectors)
    
    if nInputs > 4:
        shiftVector2 = hou.Vector2(0.0, -1.0)
        nulls = []
        for idx in range(4, nInputs):#算出nulls列表的所有元素，后面有用
            if len(inputConnectors[idx]) == 0:
                nulls.append(None) #没有连
                continue
            inputConnection = inputConnectors[idx][0]
            #inputItem = inputConnection.inputItem()

            subnetIndirectInput = inputConnection.subnetIndirectInput()
            if subnetIndirectInput is None:
                nulls.append(inputConnection.inputNode())
            else:
                null = None
                for outputConnection in subnetIndirectInput.outputConnections():
                    outputNode = outputConnection.outputNode()
                    if outputNode.type().name() == 'null':
                        if outputNode.parm('copyinput').evalAsInt() == 1:
                            null = outputNode
                            nulls.append(null)
                            break

                if null is None:
                    if 1:
                        null = subnetIndirectInput.createOutputNode('null', exact_type_name=True)
                    else:
                        null = parent.createNode('null', exact_type_name=True)
                        null.setInput(0, subnetIndirectInput, output_index=0)
                    # null.setPosition(subnetIndirectInput.position().__add__(shiftVector2))
                    null.setPosition(subnetIndirectInput.position() + shiftVector2)
                    nulls.append(null)
            
        indirectInputs = node.indirectInputs()
        for idx in range(4, nInputs):
            if nulls[idx-4] is None: #没有连
                continue
            objectMerge = node.createNode('object_merge', exact_type_name=True)
            objectMerge.parm('objpath1').set('../../' + nulls[idx-4].name() )
            objectMerge.setPosition(indirectInputs[idx].position())
            for outputConnection in indirectInputs[idx].outputConnections():
                outputNode = outputConnection.outputNode()
                outputNode.setInput(outputConnection.inputIndex(), objectMerge)


    #origNodeshape = node.userData('nodeshape')

    copyOrigNode = hou.copyNodesTo([node], parent)[0]
    newNode = node.changeNodeType('subnet', keep_parms=False)
    newNode.removeSpareParms()
    # newNode.parm('label1').hide(True)
    # newNode.parm('label2').hide(True)
    # newNode.parm('label3').hide(True)
    # newNode.parm('label4').hide(True)

    copyParms_NodetoNode(copyOrigNode, newNode)

    #if origNodeshape is not None:
        #这个是自动的啦
        #pass
        #newNode.setUserData('nodeshape', origNodeshape)
    
    newNodeParmTemplateGroup = newNode.parmTemplateGroup()
    # folder = newNodeParmTemplateGroup.findFolder('Standard')
    # folder.endsTabGroup()
    #print(folder)
    #newNodeParmTemplateGroup.hideFolder('Standard', True)
    #newNode.setParmTemplateGroup(newNodeParmTemplateGroup, rename_conflicting_parms=False)

    #newNode.removeSpareParmFolder(('Standard', ))
    #newNode.removeSpareParmTuple(newNode.parmTuple('Standard'))

    newNode.setDisplayFlag(displayFlag)
    newNode.setRenderFlag(renderFlag)
    newNode.bypass(bypass)
    newNode.setTemplateFlag(isTemplateFlagSet)
    newNode.setHighlightFlag(isHighlightFlagSet)
    newNode.setSelectableTemplateFlag(isSelectableTemplateFlagSet)
    newNode.setUnloadFlag(isUnloadFlagSet)


    copyOrigNode.destroy()

    convertSubnet_recurseSubChild(newNode, newNode, '', ignoreUnlock, Only_FeEHDA, ignore_SideFX_HDA, detectName, detectPath)
    



def convertSubnet_recurseSubChild(sourceNode, recurseNode, optype_exp = '', ignoreUnlock = False, Only_FeEHDA = True, ignore_SideFX_HDA = True, detectName = True, detectPath = False):
    nodeType = recurseNode.type()
    isNotSubnet = nodeType.name() != 'subnet'
    if recurseNode.matchesCurrentDefinition() and isNotSubnet:
        return
    
    if nodeType.definition() is None and isNotSubnet:
        return
    
    if optype_exp == '':
        optype_exp = sourceNode.type().nameComponents()[2]

    optype_exp1 = '\'' + optype_exp + '\''
    
    for child in recurseNode.children():
        relativePathTo = child.relativePathTo(sourceNode)
        str_optype = r"optype('" + relativePathTo + r"/.')" # r"optype('../.')"
        str_py_optype = r"hou.node('" + relativePathTo + r"/.').type().nameComponents()[2]" # r"hou.node('../.').type().nameComponents()[2]"
        
        parms = child.parms()
        for parm in parms:
            rawValue = parm.rawValue()
            try:
                expLang = parm.expressionLanguage()
            except:
                if parm.parmTemplate().dataType() == hou.parmData.String:
                    if r'`' + str_optype + r'`' in rawValue:
                        rawValue = rawValue.replace(r'`' + str_optype + r'`', optype_exp)
                        parm.set(rawValue)
                    if str_optype in rawValue:
                        rawValue = rawValue.replace(str_optype, optype_exp1)
                        parm.set(rawValue)
            else:
                if expLang == hou.exprLanguage.Python:
                    if str_py_optype in rawValue:
                        rawValue = rawValue.replace(str_py_optype, optype_exp1)
                        parm.deleteAllKeyframes()
                        parm.setExpression(rawValue, hou.exprLanguage.Python, True)
                elif expLang == hou.exprLanguage.Hscript:
                    if str_optype in rawValue:
                        rawValue = rawValue.replace(str_optype, optype_exp1)
                        parm.deleteAllKeyframes()
                        parm.setExpression(rawValue, hou.exprLanguage.Hscript, True)
                
                #print(rawValue)
        
        childNodeType = child.type()
        typename = childNodeType.name()
        #if "fee" in typename.lower() and child.matchesCurrentDefinition():
        
        if typename == 'subnet':
            pass
            convertSubnet_recurseSubChild(sourceNode, child, optype_exp, ignoreUnlock, Only_FeEHDA, ignore_SideFX_HDA, detectName, detectPath)
        else:
            if Only_FeEHDA:
                if isFeENode(childNodeType, detectName, detectPath):
                    convertSubnet(child, ignoreUnlock, Only_FeEHDA, ignore_SideFX_HDA, detectName, detectPath)
                else:
                    convertSubnet_recurseSubChild(sourceNode, child, optype_exp, ignoreUnlock, Only_FeEHDA, ignore_SideFX_HDA, detectName, detectPath)
            else:
                convertSubnet(child, ignoreUnlock, Only_FeEHDA, ignore_SideFX_HDA, detectName, detectPath)



def convert_All_FeENode_to_Subnet(node, ignoreUnlock = False, ignore_SideFX_HDA = True, detectName = True, detectPath = False):
    node.allowEditingOfContents()
    children = node.children()
    for child in children:
        childNodeType = child.type()
        #if isFeENode(childNodeType, detectName, detectPath):
        convertSubnet(child, ignoreUnlock, Only_FeEHDA = True, ignore_SideFX_HDA = ignore_SideFX_HDA, detectName = detectName, detectPath = detectPath)


def convert_All_HDA_to_Subnet(node, ignoreUnlock = False, ignore_SideFX_HDA = True):
    node.allowEditingOfContents()
    children = node.children()
    for child in children:
        convertSubnet(child, ignoreUnlock, Only_FeEHDA = False, ignore_SideFX_HDA = ignore_SideFX_HDA)






def findAllSubParmRawValue(subnet, strValue):
    for child in subnet.allSubChildren(recurse_in_locked_nodes=False):
        for parm in child.parms():
            if strValue in parm.rawValue():
                print(child)
                print(parm)



