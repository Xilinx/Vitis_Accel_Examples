#!/bin/bash
appDir=$(dirname $(dirname $(dirname $(readlink -f $0))))

echo "-----------------------"
echo "--  UPDATING .md to .rst --"
echo "-----------------------"

update_file() {
	ignore=0

	for i in $IGNORE; do
		if [[ $1 =~ ^description.json ]]; then 
			ignore=1
		fi
	done

	if [[ $VERBOSE == "true" ]]; then
		echo -n "Checking $1 ... "
	fi
	if [[ $ignore == 1 ]]; then
		if [[ $VERBOSE == "true" ]]; then
			echo "SKIP"
		fi
	else
		pushd . > /dev/null
                # Migrate to the example directory 
		cd $(dirname $1)
		# Take out the name of the example
		b_name=$(basename $(dirname $1))
		# Run the detailed .md generator
		$appDir/utility/md2rst/md2rst.py description.json #> /dev/null 2>&1
		# Run the .md to .rst file generator using correct path to pandoc
		$appDir/pandoc-2.7.3/bin/pandoc -f markdown D_README.md -t rst -o $b_name.rst
		# move the generated .rst to desired folder
		mv $b_name.rst $appDir/../../test/
		# delete the detailed readme
		rm D_README.md
		# Locate the desired folder and run sphinx to generate html files from
		# rst files. Go live...
		popd >/dev/null
	fi
}

VCS_FILES=$(git ls-files)

for f in $VCS_FILES; do
	if [[ ($f == */description.json) ]]; then
		if grep -q '"match_readme": "false"' $f; then
			echo $f
			echo "Readme Manually Edited:: Autofile Generator Failed"			
		else
			echo $f
			update_file $(readlink -f $f)
		fi
	fi
done
