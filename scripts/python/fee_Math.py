
def isNumber(s):
    try:
        float(s)
        return True
    except ValueError:
        pass
    return False
