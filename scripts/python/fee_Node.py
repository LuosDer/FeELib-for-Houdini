
import hou


# import fee_Node
# from importlib import reload
# reload(fee_Node)

def getUserWantedSelectedNodes(kwargsNode):
    selectedNodes = hou.selectedNodes()
    if kwargsNode in selectedNodes:
        return selectedNodes
    else:
        return (kwargsNode, )

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
    if isinstance(selectedNodes, hou.Node):
        selectedNodesTuple = (selectedNodes, )
    else:
        selectedNodesTuple = selectedNodes

    subNodeCountSum = len(selectedNodesTuple)
    for selectedNode in selectedNodesTuple:
        allSubChildren = selectedNode.allSubChildren()
        subNodeCountSum += len(allSubChildren)
        
    return subNodeCountSum

def bakeAllParms(selectedNodes):
    if isinstance(selectedNodes, hou.Node):
        selectedNodesTuple = (selectedNodes, )
    else:
        selectedNodesTuple = selectedNodes

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


