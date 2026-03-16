#include <pybind11/pybind11.h>
#include <pybind11/stl.h>     // std::vector/map → Python list/dict automatically
#include "../src/dlp_toolkit.h"      // your actual headers from include/

namespace py = pybind11;

// First arg must match the pybind11_add_module() name in root CMakeLists.txt
PYBIND11_MODULE(dlp_toolkit_py, m) {
    m.doc() = "dlp_toolkit Python bindings";
    m.def("mulmod", &mulmod);
    m.def("powmod", &powmod);
    m.def("addmod", &addmod);
    m.def("modinv", &modinv);
    m.def("bsgs", &bsgs);
    m.def("bsgs_naive", &bsgs_naive);
    m.def("gcd", [](u64 a, u64 b) {
		    auto r = gcd(a, b);
		    return py::make_tuple(r.g, r.u, r.v);
		    });

}
