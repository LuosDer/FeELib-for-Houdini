
import os
# import subprocess

#cmake -G "Visual Studio 16 2019" .. -DCMAKE_PREFIX_PATH="C:\Program Files\Side Effects Software\Houdini 19.0.498\toolkit\cmake"

doCreate = True

inputVSVersion = input('plz input your Visual Studio Version like 19 or 20 (default is 19)')
inputVSVersion = inputVSVersion.strip()

try:
    inputVSVersion = int(inputVSVersion)
    if inputVSVersion == 18:
        parm_VSVersion = "Visual Studio 15 2018"
    elif inputVSVersion == 19:
        parm_VSVersion = "Visual Studio 16 2019"
    elif inputVSVersion == 20:
        parm_VSVersion = "Visual Studio 17 2020"
    else:
        parm_VSVersion = "Visual Studio 16 2019"
except:
    parm_VSVersion = "Visual Studio 16 2019"





devenv_envKey = "devenv"

if devenv_envKey in os.environ:
    devenvPath = os.environ[devenv_envKey]
    # devenvPath = os.path.realpath(devenvPath)
else:
    devenvPath = "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/Common7/IDE/"


# print(devenvPath)
if not os.path.exists(devenvPath):
    doCreate = False

devenvPath = os.path.realpath(devenvPath)


publishMode = "Debug"
publishMode = "Release"

if doCreate:

    fileRootPath = "./../SOP/"
    fileRootPath = os.path.realpath(fileRootPath) + '/'

    for folderName in os.listdir(fileRootPath):
        relFilePath = fileRootPath + folderName
        absFilePath = os.path.realpath(relFilePath)
        if os.path.isfile(absFilePath):
            continue
                

        absBuildPath = "{0}/build".format(absFilePath)
        absBuildPath = absBuildPath.replace('\\', '/')

        if not os.path.exists(absBuildPath):
            continue

        for slnFileName in os.listdir(absBuildPath):
            _fileName, extName = os.path.splitext(slnFileName)
            # print(extName)
            if extName != '.sln':
                continue
            slnAbsFilePath = absBuildPath + '\\' + slnFileName
            slnAbsFilePath = os.path.realpath(slnAbsFilePath)

            # print(slnAbsFilePath)
            # continue
            os.chdir(devenvPath)
            command = "devenv.exe \"{0}\" /rebuild \"{1}|x64\"".format(slnAbsFilePath, publishMode)
            print(command)
            os.system(command)
            # os.system("pause")


    # command = ''
    # for folderName in os.listdir(fileRootPath):
    #     absFilePath = fileRootPath + folderName
    #     if os.path.isfile(absFilePath):
    #         continue

    #     # print(absFilePath)
    #     command += "\n"
    #     command += "cd \".\\SOP\\{0}\\build\"".format(folderName)
    #     command += "\n"
    #     command += "cmake -G {0} .. -DCMAKE_PREFIX_PATH={1}".format(parm_VSVersion, parm_DCMAKE_PREFIX_PATH)


    # command += '\npause'

    # print(command)
    # p = subprocess.Popen("cmd.exe /c" + "d:/start.bat", stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    # os.system(command)
