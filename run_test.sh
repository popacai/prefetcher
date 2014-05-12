test_list="grep.trace g++.trace ls.trace plamap.trace testgen.trace"

for run in $test_list
do
    ./cacheSim $run
done
