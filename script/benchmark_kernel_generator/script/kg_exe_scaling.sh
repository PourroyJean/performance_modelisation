T=$(date +%y%m%d%H%M%S).$(uname -n)

MD="m"

BENCH="./bin/tool_kernel_generator/kg"

for i in {1..200};do
    echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" >>$T
    echo ./$BENCH  -W 128 -D true -L 10000 -O $MD        >>$T
    echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" >>$T
    ./$BENCH  -W 128 -D true -L 10000 -O $MD | tee -a $T
    MD=$MD"m"
done
