
import hou

# import fee_Parm
# from importlib import reload
# reload(fee_Parm)

def bakeParm(parms):
    if isinstance(parms, hou.Parm):
        parmsTuple = (parms, )
    else:
        parmsTuple = parms
    
    for parm in parmsTuple:
        value = parm.eval()
        parm.deleteAllKeyframes()
        parm.set(value)

hou.Parm.bakeParm = bakeParm
