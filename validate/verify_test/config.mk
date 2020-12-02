GEN_DIR = ./test
USR_CLFLAGS:=
USR_LDCLFLAGS:=

platform_test:
	platforminfo -j $(DEVICE) > platform_info.json
	$(ABS_COMMON_REPO)/common/utility/platform_gen.py platform_info.json verify

ifneq ($(USR_CLFLAGS), )	
CLFLAGS += $(USR_CLFLAGS)
endif
ifneq ($(USR_LDCLFLAGS), )
LDCLFLAGS += $(USR_LDCLFLAGS)
endif

PLATFORM_JSON=platform.json
