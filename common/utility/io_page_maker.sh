#!/bin/bash


#Vitis GitHub IO updater script

#If adding any new examples, first update top level index lists in internal branch and merge into upstream/internal

#pandoc should be installed and working


#arguments : branches upstream/master , your_fork, pandoc location


#md2rst
	
	if [ "$#" -ne 3 ];
	then 
		echo ""
		echo "Invalid arguments"
		echo "Usage : ./io_updator.sh upstream/master_clone_path your_fork_clone_path pandoc_location"
		echo ""
		exit 1
	fi

	#clone master
	git clone $1

	#copy pandoc
	cp -rf $3/pandoc-2.9.2.1/ ./Vitis_Accel_Examples/common
        
	#make a test folder if not there just outside the clone to collect the rst files
	
	if [ -d "test" ];
	then
		rm -rf test	
	fi
		
	mkdir ./test
		
	#run md2rst
	cd Vitis_Accel_Examples/ 
	./common/utility/md2rst/update_md2rst_all.sh
	#check if rst files are updated

#building html pages using sphinx
	
	cd ..

	#make an internal folder
	if [ -d "internal" ]; 
	then
		rm -rf internal
	fi
	
	mkdir internal
	
	#clone internal into a different location
	cd internal
	git clone $2
	cd Vitis_Accel_Examples
        git remote add upstream $1
	git fetch upstream
	git checkout upstream/internal 
        
	#make a new branch temp_internal from internal
	git checkout \-b temp_internal
	
	#copy rst files from test to internal/source
	cp -rf ../../'test'/* ./'source'/
	
	#make html
	make html > ../../sphinx_log
	git add ./'source'
        git commit
	#collect errors/output from "make html" into a log
	
	#check and update internal
	git push --force \-u origin temp_internal

#update to gh-pages

	
	#clone gh-pages/
	cd ../..
	mkdir gh_pages
	cd gh_pages
	git clone $2
	cd Vitis_Accel_Examples
	git remote add upstream $1
	git fetch upstream
	git checkout upstream/gh-pages

	#make a new branch update_gh from upstream/gh_pages
	git checkout \-b update_gh
	
	#copy build/html from internal clone to master/html in gh-pages
	cp -rf ../../internal/Vitis_Accel_Examples/build/html ./master
	
	#add files for commit 
	git add ./master
        git commit
	
	#push to origin
	git push --force \-u origin update_gh

	#update gh-pages
	echo "Update done"
	echo "Please review the changes on git for pull request"
	


#done
