
MEMORY_THRESHOLD = 10000


# import fee_HIP
# from importlib import reload
# reload(fee_HIP)


import hou



def checkAllGeoMemoryUsage(MEMORY_THRESHOLD = MEMORY_THRESHOLD):

    def checkGeoMemoryUsage(geo, printObject, MEMORY_THRESHOLD):
        if geo is None:
            #continue
            return
        
        # intrinsicNames = geo.intrinsicNames()
        # print(intrinsicNames)
        memoryUsage = geo.intrinsicValue('memoryusage')
        if memoryUsage > MEMORY_THRESHOLD:
            print(printObject)
            print(memoryUsage)


    for node in hou.node('/').allSubChildren(top_down=True, recurse_in_locked_nodes=False):
        if not isinstance(node, hou.SopNode):
            continue

        isHardLocked = node.isHardLocked()
        #isSoftLocked = isSoftLocked()
        if isHardLocked:
            geo = node.geometry()
            checkGeoMemoryUsage(geo, node.path(), MEMORY_THRESHOLD)
            
        for parm in node.parms():
            parmTemplate = parm.parmTemplate()
            if not isinstance(parmTemplate, hou.DataParmTemplate):
                continue
            if parmTemplate.dataParmType() != hou.dataParmType.Geometry:
                continue
            
            geo = parm.evalAsGeometry()
            checkGeoMemoryUsage(geo, parm, MEMORY_THRESHOLD)

