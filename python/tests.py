import sys, os
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

import dlp_toolkit_py as dlp

def test_powmod():
    assert dlp.powmod(3, 218, 1000) == 489, f"expected 489, got {dlp.powmod(3, 218, 1000)}"
    print("powmod(3, 218, 1000) =", dlp.powmod(3, 218, 1000))

def test_gcd():
    a = (1 << 63) - 1
    b = (1 << 63) - 2
    g, u, v = dlp.gcd(a, b)
    assert g == 1, f"expected g=1, got {g}"
    assert u == 1, f"expected u=1, got {u}"
    assert v == -1, f"expected v=-1, got {v}"
    print(f"gcd({a}, {b}) = {g}, u={u}, v={v}")

def test_modinv():
    assert dlp.modinv(78467, 104801) == 1763, f"expected 1763, got {dlp.modinv(78467, 104801)}"
    print("78467^-1 (mod 104801) =", dlp.modinv(78467, 104801))

def test_bsgs():
    x = dlp.bsgs(5, 49999999966, 6839120451, 49999999967)
    assert x == 16666666655, f"expected 16666666655, got {x}"
    print("bsgs(5, 49999999966, 6839120451, 49999999967) =", x)

def test_bsgs_naive():
    x = dlp.bsgs_naive(5, 49999999966, 6839120451, 49999999967)
    assert x == 16666666655, f"expected 16666666655, got {x}"
    print("bsgs_naive(5, 49999999966, 6839120451, 49999999967) =", x)

if __name__ == "__main__":
    test_powmod()
    test_gcd()
    test_modinv()
    test_bsgs()
    test_bsgs_naive()
    print("All tests passed!")
