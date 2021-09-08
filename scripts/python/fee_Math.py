
# import fee_Math
# from importlib import reload
# reload(fee_Math)

def isNumber(s):
    try:
        float(s)
        return True
    except ValueError:
        pass
    return False
