
import hou

# import fee_Node
# from importlib import reload
# reload(fee_Node)

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

