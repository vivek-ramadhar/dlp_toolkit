BUILD     := build
TARGET    := dlp_toolkit# must match project() in CMakeLists.txt
PY_MODULE := dlp_toolkit_py  # what you import in Python

JOBS := $(shell nproc 2>/dev/null || sysctl -n hw.logicalcpu)
MAKEFLAGS += --jobs=$(JOBS)

# ── Flags ─────────────────────────────────────────────────────────────────────
BINDINGS  ?= OFF   # make bindings BINDINGS=ON
TRACY     ?= OFF   # make tracy    TRACY=ON
TYPE      ?= Debug # make release  TYPE=Release
NATIVE	  ?= OFF

CMAKE_FLAGS := \
    -DCMAKE_BUILD_TYPE=$(TYPE)  \
    -DBUILD_PYTHON_BINDINGS=$(BINDINGS) \
    -DENABLE_TRACY=$(TRACY)  \
    -DNATIVE_OPTIMZATIONS=$(NATIVE) \
    -Wno-dev

.PHONY: all run bindings tracy release clean install-py help

# Default: plain C++ exe, no extras
all:
	@cmake -B $(BUILD) -S . $(CMAKE_FLAGS) 2>/dev/null
	@cmake --build $(BUILD) --parallel
	@echo "✓  $(TARGET) built ($(JOBS) jobs)"

# run: all
# 	@./$(BUILD)/$(TARGET)

# Build with Python bindings
bindings:
	@$(MAKE) all BINDINGS=ON
	@$(MAKE) install-py

debug:
	@$(MAKE) all TRACY=ON TYPE=Debug

# Build with Tracy profiler
tracy:
	@$(MAKE) all TRACY=ON TYPE=Release NATIVE=On

# Build everything together
full:
	@$(MAKE) all BINDINGS=ON TRACY=ON

release:
	@$(MAKE) all TYPE=Release NATIVE=On

# Copy the .so to the project root so `import MyProject_py` just works
install-py:
	@cmake --install $(BUILD) --prefix .
	@echo "$(PY_MODULE).so installed — you can now: python -c 'import $(PY_MODULE)'"

clean:
	@rm -rf $(BUILD)
	@find . -name "$(PY_MODULE)*.so" -delete 2>/dev/null; true
	@echo "Cleaned"

help:
	@echo ""
	@echo "  make            →  build C++ exe"
	# @echo "  make run        →  build + run exe"
	@echo "  make bindings   →  build Python .so  (BINDINGS=ON)"
	@echo "  make tracy      →  build with Tracy  (TRACY=ON)"
	@echo "  make full       →  build with both"
	@echo "  make release    →  optimized build"
	@echo "  make clean      →  wipe build/"
	@echo ""
