
# import fee_HIP
# from importlib import reload
# reload(fee_HIP)


import hou

import fee_Math



def checkAllGeoMemoryUsage(MEMORY_THRESHOLD = 10000):
    def checkGeoMemoryUsage(geo, printObject, MEMORY_THRESHOLD):
        if geo is None:
            #continue
            return False
        
        # intrinsicNames = geo.intrinsicNames()
        # print(intrinsicNames)
        memoryUsage = geo.intrinsicValue('memoryusage')
        if memoryUsage > MEMORY_THRESHOLD:
            print(printObject)
            print(memoryUsage)
            return True

        return False

    flag = True
    for node in hou.node('/').allSubChildren(top_down=True, recurse_in_locked_nodes=False):
        if not isinstance(node, hou.SopNode):
            continue

        isHardLocked = node.isHardLocked()
        #isSoftLocked = isSoftLocked()
        if isHardLocked:
            geo = node.geometry()
            if checkGeoMemoryUsage(geo, node.path(), MEMORY_THRESHOLD):
                flag = False
            
        for parm in node.parms():
            parmTemplate = parm.parmTemplate()
            if not isinstance(parmTemplate, hou.DataParmTemplate):
                continue
            if parmTemplate.dataParmType() != hou.dataParmType.Geometry:
                continue
            
            geo = parm.evalAsGeometry()
            if checkGeoMemoryUsage(geo, parm, MEMORY_THRESHOLD):
                flag = False

    if flag:
        print('no found high memory usage node')


def checkAllGeoMemoryUsage_InputMemoryThreshold():
    readInput = hou.ui.readInput('Input Memory Usage Threshold', initial_contents = '100')

    if readInput[0] is None:
        return None
    
    if not fee_Math.isNumber(readInput[1]):
        raise TypeError('input not a number')
    
    checkAllGeoMemoryUsage(float(readInput[1]))

