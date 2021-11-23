
# import fee_Math
# from importlib import reload
# reload(fee_Math)

def isNumber(strInput):
    try:
        float(strInput)
        return True
    except ValueError:
        pass
    return False


def valueType(strInput):
    try:
        float(strInput)
        if isinstance(eval(strInput), int):
            return 0
        else:
            return 1
    except ValueError:
        pass

    return 2
