GEN_DIR = ./test

platform_test:
	platforminfo -j $(DEVICE) > platform_info.json
	$(ABS_COMMON_REPO)/common/utility/platform_gen.py platform_info.json bandwidth

LDCLFLAGS+= --config platform_bandwidth.ini
PLATFORM_JSON=platform.json
