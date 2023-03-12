ifdef CONFIG_ITRACE
CXXSRC = src/utils/disasm.cc
CXXFLAGS += $(shell llvm-config-14 --cxxflags) -fPIE
LIBS += $(shell llvm-config-14 --libs)
endif
