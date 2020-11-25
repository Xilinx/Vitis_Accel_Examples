GEN_DIR = ./test

platform_test:
	platforminfo -j $(DEVICE) > platform_info.json
	$(ABS_COMMON_REPO)/common/utility/platform_gen.py platform_info.json verify

PLATFORM_JSON=platform.json
