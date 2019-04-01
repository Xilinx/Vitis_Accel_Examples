################################################################################
#K-Means Settings
################################################################################
#Select  the data type INT/FLOAT both supported
DATATYPE:=INT
#Select the number of Compute units. 
COMPUTE_UNITS:=2
#NOTE: Kmeans can give better results with more compute units but for all Devices
#More than 2 compute units are not feasible so it is restricted to 2 compute units.
#User can increase the number of Compute units for bigger Devices and can get better
#Results.

#Select the number of parallel points to execute
PARALLEL_POINTS:=4

#Test Setup and files
NCLUSTERS:=5
TEST_FILE:=./data/100
GOLD_FILE:=$(TEST_FILE).gold_c$(NCLUSTERS)
################################################################################
ifeq ($(DATATYPE), INT)
	DATATYPE_ID = 1
	PARALLEL_FEATURES = 2 
else
	DATATYPE_ID = 0
	PARALLEL_FEATURES = 8 
endif

CLFLAGS += -D "PARALLEL_POINTS=$(PARALLEL_POINTS)" -D "PARALLEL_FEATURES=$(PARALLEL_FEATURES)" -D USE_DATA_TYPE=$(DATATYPE_ID)

CXXFLAGS += -DRECORD_OVERALL_TIME -DUSE_DATA_TYPE=$(DATATYPE_ID) -DPARALLEL_POINTS=$(PARALLEL_POINTS)


