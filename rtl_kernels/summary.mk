COMMON_REPO=../common/

docs: README.md

README.md:summary.json
	$(COMMON_REPO)/utility/readme_gen/gs_summary_subdir.py

