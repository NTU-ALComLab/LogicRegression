sed -i -E "s/^zero \( ([A-Za-z0-9_]+) \);/buf  \( \1, 1'b0 \);/g" circuits/circuit_$1.v
sed -i -E "s/^one  \( ([A-Za-z0-9_]+) \);/buf  \( \1, 1'b1 \);/g" circuits/circuit_$1.v
./test testcases/case$1/io_info.txt testcases/case$1/iogen circuits/circuit_$1.v
