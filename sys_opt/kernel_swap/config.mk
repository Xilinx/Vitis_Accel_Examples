ifeq ($(findstring dfx, $(DEVICE)), dfx)
CXXFLAGS += -Ddfx_device
endif
