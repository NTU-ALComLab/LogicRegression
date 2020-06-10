count_gate=`grep -E "and|or|nand|nor|xor|xnor" $1 | wc -l`
echo "Number of gates = $count_gate"