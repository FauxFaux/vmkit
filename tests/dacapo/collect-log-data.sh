#!/bin/bash 

dacapo_dir=/home/koutheir/PhD/VMKit/vmkit/tests/dacapo
log_dir=$dacapo_dir/logs

cd "$log_dir"
echo "bench_suite,benchmark,vm,duration_ms"
grep '^===== DaCapo [A-Za-z]\+ PASSED' tmp.* | sed 's/tmp[^_]\+_//g' | sed 's/ msec =====//g' | sed 's/\.log:===== DaCapo [a-z]\+ PASSED in /_/g' | tr '_' ',' | sort
