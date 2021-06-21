
import hou
import os
import fee_HDA

fee_HDA.checkHideFeENode(keepHide = True, detectName = False, detectPath = True)









sopNodeTypeNameTuple = (
'packintrinsics_fee',
'groupbybearing_fee',
'matchsize_fee',
'uniquenames_fee',
'groupbyname_fee',
'sortnamebyattrib_fee',
'keepname_betweenframe_fee',
'numpieces_fee',
'polyframe_cubic_fee',
'polyframe_cubic_fee::1.0',
'meshlayout_fee',
'meshlayout_multilayer_fee',
'copytopoints_fee',
'matchpolywinding_fast_fee::0.1',

'separatepoly2d_fee',
'uniformnormaldir_fee::1.0',
'uniformnormaldir_fee::2.0',

'spreadout_fee',
'spreadout_fee::0.9',
'spreadout_fee::1.0',
'spreadout_multiinput_fee',
'is2dswitch_fee',

'matchbbox_fee',
'correctgeowinding_wn_fee',
'correctgeowinding_volume_fee',
'normalizedirpoly_fee::1.0',
'normalizedirpoly_fee::2.0',
'animationloop_fee',
'restdir2d_fee',
'restdir_fee',
'matchdir2d_fee',
'normalizedir2d_fee',
'matchdir_fee',
'matchtransform_bound_fee',
'polycenterextrude_fee',
'groupbydir_fee',
'groupbydir_fast_fee',
'groupedgebydir_fee',
'cameraproject_fee',
'frontfaceborder_fee',
'attribdirnoise_fee',
'extractintrinsic_fee',
'resttransform_fee',
'attribaccumulate_fee',
'intrinsicpromote_fee',
'matchtransform_bb_fee',
'groupfromenum_fee',
'matchdirorient_fee::1.0',

'extractallpiecestransform_fee',
'extractsimilarpiecestransform_fee',
'uvtile_fee::0.1',
'uvtile_fee::1.0',
'import_fbx_fee',
'chineseeaves_fee',

)

for sopNodeTypeName in sopNodeTypeNameTuple:
    nodetype = hou.nodeType(hou.sopNodeTypeCategory(), sopNodeTypeName)
    if nodetype is not None:
        nodetype.setHidden(True)








__FeE_FeELib_PATH = hou.getenv('FeELib')#### 复制脚本需要修改这个
# print(__FeE_FeELib_PATH)
__FeE_FeELib_PATH = os.path.normpath(__FeE_FeELib_PATH)
__FeE_FeELib_PATH = __FeE_FeELib_PATH.replace('\\', '/')
# print(__FeE_FeELib_PATH)

__FeE_HOUDINI_SCRIPT_PATH = hou.getenv('HOUDINI_SCRIPT_PATH')
__FeE_HOUDINI_SCRIPT_PATH_split = __FeE_HOUDINI_SCRIPT_PATH.split(';')

try:
    __FeE_startScriptNum = 1 + __FeE_HOUDINI_SCRIPT_PATH_split.index(__FeE_FeELib_PATH + '/scripts')
except:
    __FeE_startScriptNum = 9999999

for idx in range(__FeE_startScriptNum, len(__FeE_HOUDINI_SCRIPT_PATH_split)):
    __FeE_nextStartupPy = __FeE_HOUDINI_SCRIPT_PATH_split[idx] + '/456.py'
    if os.path.exists(__FeE_nextStartupPy):
        with open(__FeE_nextStartupPy, "r") as __FeE_open:
            __FeE_code = __FeE_open.read()
            exec(__FeE_code)
        break

