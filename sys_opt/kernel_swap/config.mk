ifeq ($(findstring dfx, $(PLATFORM)), dfx)
CXXFLAGS += -Ddfx_device
endif
