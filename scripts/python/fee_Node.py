
import hou


import fee_Utils
import fee_HDA

# import fee_Node
# from importlib import reload
# reload(fee_Node)


def convertNodeTuple(node):
    if isinstance(node, tuple):
        if len(node) > 0:
            if isinstance(node[0], hou.Node):
                return node
        else:
            return node
    elif isinstance(node, hou.Node):
        return (node, )
    
    raise TypeError('Only Support Node or Node Tuple Type')

def getUserWantedSelectedNodes(kwargsNode):
    selectedNodes = hou.selectedNodes()
    if kwargsNode in selectedNodes:
        return selectedNodes
    else:
        return (kwargsNode, )


def addSpareInput(kwargsNode):
    selectedNodesTuple = convertNodeTuple(selectedNodes)
    for selectedNode in selectedNodes:
        pass
        #selectedNode.addSpareParmTuple(parm_template, in_folder=(), create_missing_folders=False)









def copyParms_NodetoNode(sourceNode, targetNode, copyNoneExistParms = False):
    if copyNoneExistParms:
        origParmTemplateGroup = sourceNode.parmTemplateGroup()
        #print(origParmTemplateGroup.asDialogScript())
        if 1:
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
            parmVal = parm.evalAsInt()
            #print(parmVal)
            #print(parm.multiParmStartOffset())
            parentFolder = targetNode.parm(parm.name())
            if parentFolder is None:
                continue
            #parentFolder.setFromParm(parm)
            for idx in range(0, parentFolder.evalAsInt()):
                parentFolder.removeMultiParmInstance(0)
            for idx in range(0, parmVal):
                parentFolder.insertMultiParmInstance(idx)
            '''
            try:
                parmVal = parm.evalAsInt()
                #print(parmVal)
                #print(parm.multiParmStartOffset())
                parentFolder = targetNode.parm(parm.name())
                if parentFolder is None:
                    continue
                #parentFolder.setFromParm(parm)
                for idx in range(0, parentFolder.evalAsInt()):
                    parentFolder.removeMultiParmInstance(0)
                for idx in range(0, parmVal):
                    parentFolder.insertMultiParmInstance(idx)
            except:
                print('')
                #print(targetNode)
                print(parm)
            '''

    if 0:
        for parm in targetNode.parms():
            if parm.isMultiParmInstance():
                print(parm)

    for targetparm in targetNode.parms():
        #break
        sourceparm = sourceNode.parm(targetparm.name())
        if sourceparm is None:
            continue
        targetparm.deleteAllKeyframes()
        targetparm.setFromParm(sourceparm)
        '''
        try:
            sourceparm = sourceNode.parm(targetparm.name())
            if sourceparm is None:
                continue
            targetparm.deleteAllKeyframes()
            targetparm.setFromParm(sourceparm)
        except:
            #print(targetNode)
            #print(targetNode.parm(targetparm.name()))
            print(targetparm)
        '''
    '''
    for sourceparm in sourceNode.parms():
        #break
        try:
            targetparm = targetNode.parm(sourceparm.name())
            if targetparm is None:
                continue
            targetparm.deleteAllKeyframes()
            targetparm.setFromParm(sourceparm)
        except:
            #print(targetNode)
            #print(targetNode.parm(sourceparm.name()))
            print(sourceparm)
    '''


def copyRelRef_NodetoNode(sourceNode, targetNode, prefix='', sufix=''):
    for sourceParm in sourceNode.parms():
        parmName = sourceParm.name()
        targetParm = targetNode.parm(prefix + parmName + sufix)
        if targetParm is not None:
            targetParm.deleteAllKeyframes()
            targetParm.set(sourceParm)

def bake_optypeExp_to_str(targetNode, sourceNode, optype_exp, optype_exp1):
    relativePathTo = targetNode.relativePathTo(sourceNode)
    str_optype0 = r"optype('" + relativePathTo + r"/.')" # r"optype('../.')"
    str_optype1 = r"optype('" + relativePathTo + r"')" # r"optype('..')"
    str_py_optype0 = r"hou.node('" + relativePathTo + r"/.').type().nameComponents()[2]" # r"hou.node('../.').type().nameComponents()[2]"
    str_py_optype1 = r"hou.node('" + relativePathTo + r"').type().nameComponents()[2]" # r"hou.node('..').type().nameComponents()[2]"
    #print(str_py_optype0)

    parms = targetNode.parms()
    for parm in parms:
        rawValue = parm.rawValue()
        try:
            expLang = parm.expressionLanguage()
        except:
            #没有exp
            if parm.parmTemplate().dataType() == hou.parmData.String:
                #是string类型
                if r'`' + str_optype0 + r'`' in rawValue:
                    rawValue = rawValue.replace(r'`' + str_optype0 + r'`', optype_exp)
                    parm.set(rawValue)
                if r'`' + str_optype1 + r'`' in rawValue:
                    rawValue = rawValue.replace(r'`' + str_optype1 + r'`', optype_exp)
                    parm.set(rawValue)
                if str_optype0 in rawValue:
                    rawValue = rawValue.replace(str_optype0, optype_exp1)
                    parm.set(rawValue)
                if str_optype1 in rawValue:
                    rawValue = rawValue.replace(str_optype1, optype_exp1)
                    parm.set(rawValue)
        else:
            #有exp
            if expLang == hou.exprLanguage.Python:
                if str_py_optype0 in rawValue:
                    rawValue = rawValue.replace(str_py_optype0, optype_exp1)
                    parm.deleteAllKeyframes()
                    parm.setExpression(rawValue, hou.exprLanguage.Python, True)
                if str_py_optype1 in rawValue:
                    rawValue = rawValue.replace(str_py_optype1, optype_exp1)
                    parm.deleteAllKeyframes()
                    parm.setExpression(rawValue, hou.exprLanguage.Python, True)

            elif expLang == hou.exprLanguage.Hscript:
                if str_optype0 in rawValue:
                    rawValue = rawValue.replace(str_optype0, optype_exp1)
                    parm.deleteAllKeyframes()
                    parm.setExpression(rawValue, hou.exprLanguage.Hscript, True)
                if str_optype1 in rawValue:
                    rawValue = rawValue.replace(str_optype1, optype_exp1)
                    parm.deleteAllKeyframes()
                    parm.setExpression(rawValue, hou.exprLanguage.Hscript, True)



def convertSubnet(node, ignoreUnlock = False, Only_FeEHDA = True, ignore_SideFX_HDA = True, detectName = True, detectPath = False):
    nodeType = node.type()
    
    ##### 检测各种情况
    if nodeType.name() == 'subnet':
        convertSubnet_recurseSubChild(node, node, '', ignoreUnlock, Only_FeEHDA, ignore_SideFX_HDA)

    defi = nodeType.definition()
    if defi is None:
        return

    if not ignoreUnlock and not node.matchesCurrentDefinition():
        #print(node)
        convertSubnet_recurseSubChild(node, node, '', ignoreUnlock, Only_FeEHDA)
        return
    
    if Only_FeEHDA:
        if not fee_HDA.isFeENode(nodeType, detectName, detectPath):
            return

    if ignore_SideFX_HDA:
        # defaultLibPath = hou.getenv('HFS') + r'/houdini/otls/'
        # if defi.libraryFilePath().startswith(defaultLibPath):
        if fee_HDA.isSideFXDefinition(defi):
            convertSubnet_recurseSubChild(node, node, '', ignoreUnlock, Only_FeEHDA)
            return

    node.allowEditingOfContents()

    ##### 记录flag情况
    parent = node.parent()
    #print(parent.childTypeCategory().name())
    if parent.childTypeCategory().name() != 'Sop':
        raise TypeError('error')
    if node.isHardLocked():
        raise TypeError('isHardLocked')
    if node.isSoftLocked():
        raise TypeError('isSoftLocked')
    
    displayFlag = node == parent.displayNode()
    renderFlag = node == parent.renderNode()
    bypass = node.isBypassed()
    isTemplateFlagSet = node.isTemplateFlagSet()
    isHighlightFlagSet = node.isHighlightFlagSet()
    isSelectableTemplateFlagSet = node.isSelectableTemplateFlagSet()
    isUnloadFlagSet = node.isUnloadFlagSet()

    inputConnectors = node.inputConnectors()
    nInputs = len(inputConnectors)

    #### 处理4号以后的输入口
    if nInputs > 4:
        shiftVector2 = hou.Vector2(0.0, -1.0)
        nulls = []

        ###算出nulls列表的所有元素，后面有用
        for idx in range(4, nInputs):
            if len(inputConnectors[idx]) == 0:
                # 说明没有连
                nulls.append(None)
                continue
            
            inputConnection = inputConnectors[idx][0]
            subnetIndirectInput = inputConnection.subnetIndirectInput()

            #### 查找输入的类型
            flag = False
            if subnetIndirectInput is None:
                inputNode = inputConnection.inputNode()
                if inputNode is None:
                    ####说明输入是一个个dot，最终连接到subnet input
                    inputItem = inputConnection.inputItem()
                    while type(inputItem).__name__ == 'NetworkDot':
                        ####寻找dot的最终链接
                        inputItem = inputItem.inputItem()
                    
                    if type(inputItem).__name__ == 'SubnetIndirectInput':
                        subnetIndirectInput = inputItem
                        flag = True
                    else:
                        raise ValueError('fee : 不可能的情况')
                        #nulls.append(inputItem)
                        #flag = False
                else:
                    ####说明输入是一个node
                    nulls.append(inputNode)
                    flag = False
            else:
                ####说明输入是一个subnet input
                flag = True
            
            #### 建立null节点
            if flag:
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
                        null = parent.createNode('null', exact_type_name=True)
                        null.setInput(0, subnetIndirectInput, output_index=0)
                    else:
                        try:
                            null = subnetIndirectInput.createOutputNode('null', exact_type_name=True)
                        except:
                            print(subnetIndirectInput)
                            raise ValueError('')
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


    #### 处理0-3号输入口
    inputItems = []
    inputItem_output_index = []
    for idx in range(0, min(nInputs, 4)):
        if len(inputConnectors[idx]) == 0:
            # 说明没有连
            inputItems.append(None)
            inputItem_output_index.append(None)
            continue
        inputConnection = inputConnectors[idx][0]
        inputItem = inputConnection.inputItem()
        inputItems.append(inputItem)
        #if type(inputItem).__name__ == 'NetworkDot' or type(inputItem).__name__ == 'SubnetIndirectInput':
        if isinstance(inputItem, hou.NetworkDot) or isinstance(inputItem, hou.SubnetIndirectInput):
            inputItem_output_index.append(0)
        else:
            inputItem_output_index.append(inputConnection.outputIndex())


    ############ 修改节点类型 ############
    #origNodeshape = node.userData('nodeshape')
    copyOrigNode = hou.copyNodesTo([node], parent)[0]
    newNode = node.changeNodeType('subnet', keep_parms=False)
    newNode.removeSpareParms()
    for idx in range(0, min(nInputs, 4)):
        if inputItems[idx] is not None:
            try:
                newNode.setInput(idx, inputItems[idx], inputItem_output_index[idx])
            except:
                print(newNode)
                print(inputItems[idx])
                print(inputItem_output_index[idx])
                raise ValueError('setInput Error')
    # newNode.parm('label1').hide(True)
    # newNode.parm('label2').hide(True)
    # newNode.parm('label3').hide(True)
    # newNode.parm('label4').hide(True)

    oldIndirectInputs = copyOrigNode.indirectInputs()
    newIndirectInputs = newNode.indirectInputs()
    for idx in range(0, min(len(oldIndirectInputs), 4)):
        try:
            pos = oldIndirectInputs[idx].position()
            newIndirectInputs[idx].setPosition(pos)
        except:
            print(copyOrigNode)
            print(newNode)
            print(oldIndirectInputs[idx])
            print(newIndirectInputs[idx])
            raise ValueError('fee Error')


    copyParms_NodetoNode(copyOrigNode, newNode, copyNoneExistParms = True)

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

    optype_exp = copyOrigNode.type().nameComponents()[2]
    optype_exp1 = '\'' + optype_exp + '\''
    bake_optypeExp_to_str(newNode, newNode, optype_exp, optype_exp1)
    convertSubnet_recurseSubChild(newNode, newNode, optype_exp, ignoreUnlock, Only_FeEHDA, ignore_SideFX_HDA, detectName, detectPath)
    #print(optype_exp)

    copyOrigNode.destroy()

    



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
        bake_optypeExp_to_str(child, sourceNode, optype_exp, optype_exp1)

        """
        relativePathTo = child.relativePathTo(sourceNode)
        str_optype = r"optype('" + relativePathTo + r"/.')" # r"optype('../.')"
        str_py_optype = r"hou.node('" + relativePathTo + r"/.').type().nameComponents()[2]" # r"hou.node('../.').type().nameComponents()[2]"
        # print(str_py_optype)

        parms = child.parms()
        for parm in parms:
            rawValue = parm.rawValue()
            try:
                expLang = parm.expressionLanguage()
            except:
                #没有exp
                if parm.parmTemplate().dataType() == hou.parmData.String:
                    #是string类型
                    if r'`' + str_optype + r'`' in rawValue:
                        rawValue = rawValue.replace(r'`' + str_optype + r'`', optype_exp)
                        parm.set(rawValue)
                    if str_optype in rawValue:
                        rawValue = rawValue.replace(str_optype, optype_exp1)
                        parm.set(rawValue)
            else:
                #有exp
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
        """

        childNodeType = child.type()
        typename = childNodeType.name()
        #if "fee" in typename.lower() and child.matchesCurrentDefinition():
        
        if typename == 'subnet':
            pass
            convertSubnet_recurseSubChild(sourceNode, child, optype_exp, ignoreUnlock, Only_FeEHDA, ignore_SideFX_HDA, detectName, detectPath)
        else:
            if Only_FeEHDA:
                if fee_HDA.isFeENode(childNodeType, detectName, detectPath):
                    convertSubnet(child, ignoreUnlock, Only_FeEHDA, ignore_SideFX_HDA, detectName, detectPath)
                else:
                    convertSubnet_recurseSubChild(sourceNode, child, optype_exp, ignoreUnlock, Only_FeEHDA, ignore_SideFX_HDA, detectName, detectPath)
            else:
                convertSubnet(child, ignoreUnlock, Only_FeEHDA, ignore_SideFX_HDA, detectName, detectPath)









def convert_All_FeENode_to_Subnet(node, ignoreUnlock = False, ignore_SideFX_HDA = True, detectName = True, detectPath = False, displayConfirmation = False):
    if displayConfirmation:
        fee_Utils.displayConfirmation(prevText = 'plz backup HIP before do this\n建议先备份HIP')

    node.allowEditingOfContents()
    for child in node.children():
        childNodeType = child.type()
        #if fee_HDA.isFeENode(childNodeType, detectName, detectPath):
        convertSubnet(child, ignoreUnlock, Only_FeEHDA = True, ignore_SideFX_HDA = ignore_SideFX_HDA, detectName = detectName, detectPath = detectPath)


def convert_All_HDA_to_Subnet(node, ignoreUnlock = False, ignore_SideFX_HDA = True, displayConfirmation = False):
    if displayConfirmation:
        fee_Utils.displayConfirmation(prevText = 'plz backup HIP before do this\n建议先备份HIP')

    node.allowEditingOfContents()
    for child in node.children():
        convertSubnet(child, ignoreUnlock, Only_FeEHDA = False, ignore_SideFX_HDA = ignore_SideFX_HDA)



def unlock_All_HDA(node, detectName = True, detectPath = False, displayConfirmation = False):
    if displayConfirmation:
        fee_Utils.displayConfirmation(prevText = 'plz backup HIP before do this\n建议先备份HIP')

    node.allowEditingOfContents()
    for child in node.children():
        childNodeType = child.type()
        if fee_HDA.isFeENode(childNodeType, detectName, detectPath):
            # child.allowEditingOfContents()
            unlock_All_FeENode(child, detectName, detectPath)
        elif fee_HDA.isUnlockedHDA(child):
            unlock_All_FeENode(child, detectName, detectPath)


def unlock_All_FeENode(node, detectName = True, detectPath = False, displayConfirmation = False):
    if displayConfirmation:
        fee_Utils.displayConfirmation(prevText = 'plz backup HIP before do this\n建议先备份HIP')

    node.allowEditingOfContents()
    for child in node.children():
        childNodeType = child.type()
        if fee_HDA.isFeENode(childNodeType, detectName, detectPath):
            # child.allowEditingOfContents()
            unlock_All_FeENode(child, detectName, detectPath)
        elif fee_HDA.isUnlockedHDA(child):
            unlock_All_FeENode(child, detectName, detectPath)


# def unlock_All_FeENode_recurseSubChild(node, detectName = True, detectPath = False):
















def nodesBottomRightPosition(selectedNodes):
    selectedNodesTuple = convertNodeTuple(selectedNodes)
    if len(selectedNodesTuple) == 0:
        raise ValueError('No Input Nodes')

    BottomRightPosition = hou.Vector2(1e10, -1e10)
    for selectedNode in selectedNodes:
        nodePosition = selectedNode.position()
        BottomRightPosition[0] = min(BottomRightPosition[0], nodePosition[0])
        BottomRightPosition[1] = max(BottomRightPosition[1], nodePosition[1])
    
    return BottomRightPosition

def createCopyNodeParmsNode(selectedNodes):
    selectedNodesTuple = convertNodeTuple(selectedNodes)
    if len(selectedNodesTuple) == 0:
        raise ValueError('No Input Nodes')
        
    parentNode = selectedNodes[0].parent()

    copyParmsNode = parentNode.node('./copyNodeParms_tempNodeforPython1')
    if copyParmsNode is None:
        copyParmsNode = parentNode.createNode('FeE::copyNodeParms', node_name = 'copyNodeParms_tempNodeforPython1')
    
    sourceNodesStrs = []
    for selectedNode in selectedNodes:
        sourceNodesStrs.append(selectedNode.name())
        
    copyParmsNode.parm('sourceNodes').set('../' + ' ../'.join(sourceNodesStrs))
    
    nwePos = nodesBottomRightPosition(selectedNodesTuple) + hou.Vector2(0, -1)
    copyParmsNode.setPosition(nwePos)

def setCopyNodeParmsNode(selectedNodes):
    selectedNodesTuple = convertNodeTuple(selectedNodes)
    if len(selectedNodesTuple) == 0:
        raise ValueError('No Input Nodes')

    parentNode = selectedNodes[0].parent()

    copyParmsNode = parentNode.node('./copyNodeParms_tempNodeforPython1')
    sourceNodesStrs = []
    for selectedNode in selectedNodes:
        sourceNodesStrs.append(selectedNode.name())
        
    copyParmsNode.parm('targetNodes').set('../' + ' ../'.join(sourceNodesStrs))
    


def extract_LockedNull(selectedNodes, subPath = 'extractLockedGeo', savePath_rel_to_HIP = True, keep_name = True):
    selectedNodesTuple = convertNodeTuple(selectedNodes)
    for selectedNode in selectedNodesTuple:
        nodetype = selectedNode.type()
        if nodetype.name() == 'null' and selectedNode.isGenericFlagSet(hou.nodeFlag.Lock):
            
            filepath = ''.join(['/', subPath, '/', selectedNode.path().lstrip('/').replace('/', '.'), '.bgeo'])

            envName = "HIP" if savePath_rel_to_HIP else "TEMP"
            filepath_abs = hou.getenv(envName) + filepath
            filepath_rel = '$' + envName + filepath
            # print(filepath_abs)
            # print(filepath_rel)
            import os
            pathDir = os.path.split(filepath_abs)[0]
            if not os.path.exists(pathDir):
                os.mkdir(pathDir)
            # print(pathDir)
            selectedNode.geometry().saveToFile(filepath_abs)

            selectedNode.setGenericFlag(hou.nodeFlag.Lock, 0)
            newnode = selectedNode.changeNodeType(new_node_type='file', keep_name = keep_name, keep_parms=False, keep_network_contents=True, force_change_on_node_type_match=True)
            newnode.setParms({"file": filepath_rel})

        elif selectedNode.isNetwork() and nodetype.category().name() == 'Sop' and not selectedNode.isLockedHDA():
            for child in selectedNode.children():
                foreverychildren(child)

def setNodeAsOutput(selectedNode):
    #selectedNode = hou.selectedNodes()[0]
    selectedParent = selectedNode.parent()

    outputRenderNodes = []
    #outputRenderNodesIdx = []
    for child in selectedParent.children():
        if child.type().name() != 'output':
            continue
        outputRenderNodes.append(child)
        #outputRenderNodesIdx.append(child.parm('outputidx').evalAsInt())

    if outputRenderNodes:
        if 1:
            outputRenderNodes.sort(key = lambda node: node.parm('outputidx').evalAsInt())
        else:
            outputRenderNodes = [i[0] for i in sorted(zip(outputRenderNodes, outputRenderNodesIdx), key=lambda n: n[1])]

        # for node in outputRenderNodes:
        #     print(node.parm('outputidx').evalAsInt())
        
        flag = 1
        
        for outputNode in selectedNode.outputs():
            if outputNode not in outputRenderNodes:
                continue
            outputNode.setInput(0, None)
            outputRenderNodes[(outputRenderNodes.index(outputNode) + 1) % len(outputRenderNodes)].setInput(0, selectedNode)
            flag = 0
            break

        if flag:
            outputRenderNodes[0].setInput(0, selectedNode)

    else:
        newOutputNode = selectedNode.createOutputNode('output')
        newOutputNode.setPosition(selectedNode.position() + hou.Vector2(0.0, -1.0))


def subNodeCount(selectedNodes):
    selectedNodesTuple = convertNodeTuple(selectedNodes)

    subNodeCountSum = len(selectedNodesTuple)
    for selectedNode in selectedNodesTuple:
        allSubChildren = selectedNode.allSubChildren()
        subNodeCountSum += len(allSubChildren)
        
    return subNodeCountSum

def bakeAllParms(selectedNodes):
    selectedNodesTuple = convertNodeTuple(selectedNodes)

    for selectedNode in selectedNodesTuple:
        if 0:
            import fee_Parm
            fee_Parm.bakeParm(selectedNode.parms())
        else:
            for parm in selectedNode.parms():
                value = parm.eval()
                parm.deleteAllKeyframes()
                parm.set(value)
            
def setToDefaultPosition(selectedItems):
    for selectedItem in selectedItems:
        if selectedItem.networkItemType() == hou.networkItemType.SubnetIndirectInput:
            idx = int(selectedItem.name()) - 1
            selectedItem.setPosition(hou.Vector2(0, 6) + hou.Vector2(3, 0.5) * idx)


def setToDefaultPositionWithNode(selectedItems):
    shiftedPosition = hou.Vector2(0, 0)

    for selectedItem in selectedItems:
        if selectedItem.networkItemType() == hou.networkItemType.SubnetIndirectInput:
            if selectedItem.name() == '1':
                shiftedPosition = hou.Vector2(0, 6) - selectedItem.position()
                break
            

    for selectedItem in selectedItems:
        selectedItem.move(shiftedPosition)


def selectSameNodes(selectedNodes):
    def select(node):
        if node.type() == nodeType:
            node.setSelected(1)

    for selectedNode in selectedNodes:
        siblingNodes = selectedNode.parent().children()
        nodeType = selectedNode.type()
        map(select, sibling)

def createOutputNode_allItemVer(inputItem, node_type_name, node_name=None, run_init_scripts=True, load_contents=True, exact_type_name=False):
    #if isinstance(inputItem, hou.SubnetIndirectInput):
    if isinstance(inputItem, hou.SubnetIndirectInput):
        #shiftPos = hou.Vector2(0, -1)
        if node_type_name == 'null':
            shiftPos = hou.Vector2(0, -1.1692999999999998)
            #shiftPos[1] = -1.1692999999999998
        else:
            shiftPos = hou.Vector2(0, -1)


        parent = inputItem.parent()
        upstreamPos = inputItem.position()
        newNode = parent.createNode(node_type_name, node_name, run_init_scripts, load_contents, exact_type_name)
        
        #print(upstreamPos + shiftPos)
        newNode.setPosition(upstreamPos + shiftPos)

        outputConnections = inputItem.outputConnections()
        for outputConnection in outputConnections:
            outputConnectionNode = outputConnection.outputNode()
            outputConnectionNode.setInput(outputConnection.inputIndex(), newNode, output_index=0)

        newNode.setInput(0, inputItem, output_index=0)

        
    elif isinstance(inputItem, hou.Node):
        newNode = inputItem.createOutputNode(node_type_name, node_name, run_init_scripts, load_contents, exact_type_name)
    else:
        raise TypeError('invalid input type')
    return newNode

def setToDefaultColorShape(selectedItems):
    defaultColorItemValue = 0.7
    defaultColorItem = hou.Color(defaultColorItemValue, defaultColorItemValue, defaultColorItemValue)

    defaultColorFile = hou.Color(0.9, 0.8, 0.55)
    defaultColorRop = hou.Color(0.65, 0.4, 0.5)
    defaultColorLoopBlock = hou.Color(0.75, 0.4, 0)

    #defaultColorNodeValue = 0.8
    #defaultColorNode = hou.Color(defaultColorNodeValue, defaultColorNodeValue, defaultColorNodeValue)

    for selectedItem in selectedItems:
        networkItemType = selectedItem.networkItemType()
        if networkItemType == hou.networkItemType.Node:
            nodeTypeName = selectedItem.type().name()
            
            if nodeTypeName == 'file_fbx_fee':
                selectedItem.setColor(defaultColorFile)
            elif nodeTypeName == 'exportUE4_fbx_fee':
                selectedItem.setColor(defaultColorRop)
            elif nodeTypeName == 'block_begin':
                selectedItem.setColor(defaultColorLoopBlock)
            elif nodeTypeName == 'block_end':
                selectedItem.setColor(defaultColorLoopBlock)
                
            else:
                selectedItem.setColor(selectedItem.type().defaultColor())
                
            
            print(selectedItem.userData('nodeshape'))
            if nodeTypeName == 'convertline_fee::2.0':
                selectedItem.setUserData('nodeshape', 'trapezoid_up')
            elif nodeTypeName == 'exportUE4_fbx_fee':
                pass
                
            '''
            defaultShape = selectedItem.type().defaultShape()
            print(selectedItem.type())
            print(selectedItem.userDataDict())
            print(defaultShape)
            selectedItem.setUserData('nodeshape', defaultShape)
            '''
            
        elif networkItemType == hou.networkItemType.SubnetIndirectInput:
            selectedItem.setColor(defaultColorItem)

def correctWrangleParmeter(selectedNodes):
    import re
    for node in selectedNodes:
        nodeType = node.type()
        nodeTypeName = nodeType.name()
        if nodeTypeName != 'attribwrangle':
            continue
        
        snippet = node.evalParm('snippet')
        
        ######################### vex_inplace ##########################
        vex_inplace = node.parm('vex_inplace')
        vex_inplaceVal = True
        for substr in ('attrib(0,', 'prim(0,', 'point(0,', 'vertex(0,', 'detail(0,'):
            if substr in snippet:
                vex_inplaceVal = False
                break
        if vex_inplaceVal:
            classParmVal = node.parm('vex_inplace').evalAsString()
            print("findattribval(0, '" + classParmVal + "'")
            if "findattribval(0, '" + classParmVal + "'" in snippet:
                vex_inplaceVal = False
        
        vex_inplace.set(vex_inplaceVal)
        
        #print(usedAttribNames)
        ############################ autobind ######################
        if node.evalParm('autobind')==1 or node.evalParm('groupautobind')==1:
            node.parm('autobind').set(False)
            node.parm('groupautobind').set(False)
            usedAttribNames = []
            pattern = re.compile(r'@[a-zA-Z0-9]*')
            usedAttribNames = pattern.findall(snippet)
            usedAttribNames = list(set(usedAttribNames))

            for idx in range(len(usedAttribNames)):
                usedAttribNames[idx] = usedAttribNames[idx][1:]
            
            for topStr in ('elemnum', 'ptnum', 'primnum', 'vtxnum', 'numelem', 'numprim', 'numpt', 'numvtx'):
                if topStr in usedAttribNames:
                    usedAttribNames.remove(topStr)

            for topStr in ('color', 'Alpha', 'Cd', 'N', 'P'):
                if topStr not in usedAttribNames:
                    continue
                usedAttribNames.remove(topStr)
                usedAttribNames.insert(0, topStr)
                

            node.parm('bindings').set(len(usedAttribNames))
            for idx in range(len(usedAttribNames)):
                stridx = str(idx+1)
                node.parm('bindname' + stridx).set(usedAttribNames[idx])
                node.parm('bindparm' + stridx).set(usedAttribNames[idx])

