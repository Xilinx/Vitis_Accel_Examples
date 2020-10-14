ifeq ($(ENABLE_ASYNC_FLOW), yes)
LDFLAGS += -laio
CXXFLAGS += -DASYNC_READ
endif
