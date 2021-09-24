
# import fee_Math
# from importlib import reload
# reload(fee_Math)

def isNumber(input):
    try:
        float(input)
        return True
    except ValueError:
        pass
    return False
