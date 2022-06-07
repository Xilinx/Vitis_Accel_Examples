#!/bin/bash
# This script regenerates all of the README files in the Vitis example repository
# An example with an auto-generated README file requires a description.json file
# Only examples with a valid description.json file are updated by this script

BASEDIR=$(pwd)

dir_list=( $(git ls-files | grep 'description.json' | sed -r 's|/[^/]+$||' | sort | uniq ))

echo ${dir_list[@]}
echo $BASEDIR

for i in "${dir_list[@]}"
do
    cd $i
    if grep -r '"match_readme" : "false"' .; then
        echo "Ignoring README.rst = $i"
    elif
        [[ "$i" =~ ^system_compilation.* ]]; then
        echo "Updating README for = $i"
        rm README.rst
	../../common/utility/readme_gen/readme_gen.py ./description.json
    else
        rm README.rst
        echo "Updating README for = $i"
        make docs
         
    fi
       cd $BASEDIR
done

summary_list=( $(git ls-files | grep 'summary.json' | sed -r 's|/[^/]+$||' | sort | uniq ))
echo ${summary_list[@]}
echo $BASEDIR
for i in "${summary_list[@]}"
do
    cd $i
    echo "Updating README for = $i"
    rm README.rst
    make docs -f summary.mk
    git add README.rst
    cd $BASEDIR
done
