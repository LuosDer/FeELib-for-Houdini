
import hou


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


