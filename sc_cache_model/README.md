# sc_cache_model


```

cd diploma_work/sc_cache_model/run
module load systemc-3.0.1.tcl
make -j
make run

or use 

make run VERBOSITY_LEVEL="SC_HIGH"

# cmd line args/default args

# VERBOSITY_LEVEL    		 == "SC_MEDIUM"
# TEST_NAME          		 == "cache_bulk_read_write_check"
# CACHE_SIZE         		 == "32768"
# LINE_SIZE         		 == "64"
# SET_ASSOC_COUNT    		 == "4"
# WORD_SIZE          		 == "4"
# MEMORY_READ_DELAY  		 == "40"
# MEMORY_WRITE_DELAY 		 == "60"
# CACHE_READ_DELAY   		 == "4"
# CACHE_WRITE_DELAY  		 == "6"
# CACHE_INVALIDATE_DELAY     == "3"
# CONNECT_TO_EXTERNAL_MEMORY == "false"

```
