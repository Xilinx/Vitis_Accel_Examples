VERBOSE := no

# Generate Verbose content
ifeq ($(VERBOSE),yes)
CXXFLAGS += -DVERBOSE
endif
