COMMON_REPO=../common/

docs: README.rst

README.rst:summary.json
	$(COMMON_REPO)/utility/readme_gen/gs_summary_subdir.py

