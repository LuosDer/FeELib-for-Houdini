
import hou

def copyParms_NodetoNode(sourceNode, targetNode):
    origParmTemplateGroup = sourceNode.parmTemplateGroup()
    targetNode.setParmTemplateGroup(origParmTemplateGroup, rename_conflicting_parms=False)
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
    


def convertSubnet(node):
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


    copyOrigNode = hou.copyNodesTo([node], parent)[0]
    newNode = node.changeNodeType('subnet', keep_parms=False)
    newNode.removeSpareParms()
    copyParms_NodetoNode(copyOrigNode, newNode)

    newNode.setDisplayFlag(displayFlag)
    newNode.setRenderFlag(renderFlag)
    newNode.bypass(bypass)
    newNode.setTemplateFlag(isTemplateFlagSet)
    newNode.setHighlightFlag(isHighlightFlagSet)
    newNode.setSelectableTemplateFlag(isSelectableTemplateFlagSet)
    newNode.setUnloadFlag(isUnloadFlagSet)


    optype_exp = copyOrigNode.type().nameComponents()[2]

    convertSubnet_recurseSubChild(newNode, newNode, optype_exp)
    
    copyOrigNode.destroy()



def convertSubnet_recurseSubChild(sourceNode, recurseNode, optype_exp):
    optype_exp1 = '\'' + optype_exp + '\''
    
    recurseNode.allowEditingOfContents()
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
        
        typename = child.type().name()
        #if "fee" in typename.lower() and child.matchesCurrentDefinition():
        if "fee" in typename.lower():
            pass
            convertSubnet(child)
        elif ( not child.matchesCurrentDefinition() ) or ( typename == 'subnet' ):
            pass
            convertSubnet_recurseSubChild(sourceNode, child, optype_exp)

