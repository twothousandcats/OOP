#!/bin/bash

EXEC=./invert
TESTS_DIR=./tests

echo "Running tests..."

# Create test dir
mkdir -p $TESTS_DIR
# Test 1: Valid matrix
cat > $TESTS_DIR/matrix1.txt << EOF
1	2	3
0	1	4
5	6	0
EOF

echo "Test 1: Valid matrix 1"
output=$(./$EXEC $TESTS_DIR/matrix1.txt)
expected="-24.000	18.000	5.000
20.000	-15.000	-4.000
-5.000	4.000	1.000"

if [ "$output" = "$expected" ]; then
    echo "$output"
    echo "[PASS]"
    echo ""
else
    echo "Failed: got"
    echo "$output"
    echo "expected"
    echo "$expected"
    exit 1
fi

# Test 2: Valid matrix with decimals from example 2
cat > $TESTS_DIR/matrix2.txt << EOF
4	7	2.3
2	1	1
3	-2	-2.31
EOF

echo "Test 2: Valid matrix with decimals"
output=$(./$EXEC $TESTS_DIR/matrix2.txt)
if [[ $output != *"Error"* ]] && [[ $output != *"Non-invertible"* ]] && [[ $output != *"Invalid"* ]]; then
    echo "$output"
    echo "[PASS]"
    echo ""
else
    echo "Failed: got '$output'"
    exit 1
fi

# Test 3: Non-invertible matrix
cat > $TESTS_DIR/matrix3.txt << EOF
1	2	3
2	4	6
1	2	3
EOF

echo "Test 3: Non-invertible matrix"
output=$(./$EXEC $TESTS_DIR/matrix3.txt)
if [ "$output" = "Non-invertible" ]; then
    echo "$output"
    echo "[PASS]"
    echo ""
else
    echo "Failed: got '$output', expected 'Non-invertible'"
    exit 1
fi

# Test 4: Invalid matrix (non-numeric value)
cat > $TESTS_DIR/matrix4.txt << EOF
1	2	a
2	4	6
1	2	3
EOF

echo "Test 4: Invalid matrix (non-numeric)"
output=$(./$EXEC $TESTS_DIR/matrix4.txt)
if [ "$output" = "Invalid matrix format" ]; then
    echo "$output"
    echo "[PASS]"
    echo ""
else
    echo "Failed: got '$output', expected 'Invalid matrix'"
    exit 1
fi

# Test 5: Invalid matrix format
cat > $TESTS_DIR/matrix5.txt << EOF
1	2	3	4
2	4	6	5
1	2	3	7
EOF

echo "Test 5: Invalid matrix format"
output=$(./$EXEC $TESTS_DIR/matrix5.txt)
if [ "$output" = "Invalid matrix format" ]; then
    echo "$output"
    echo "[PASS]"
    echo ""
else
    echo "Failed: got '$output', expected 'Invalid matrix format'"
    exit 1
fi

# Test 6: Invalid matrix format
cat > $TESTS_DIR/matrix6.txt << EOF
1	2	3
2	4	6
EOF

echo "Test 6: Invalid matrix format"
output=$(./$EXEC $TESTS_DIR/matrix6.txt)
if [ "$output" = "Invalid matrix format" ]; then
    echo "$output"
    echo "[PASS]"
    echo ""
else
    echo "Failed: got '$output', expected 'Invalid matrix format'"
    exit 1
fi

# Test 7: Help option
echo "Test 7: Help option"
output=$(./$EXEC -h)
if [[ $output == *"Usage:"* ]]; then
    echo "$output"
    echo "[PASS]"
    echo ""
else
    echo "Failed: help not displayed properly"
    exit 1
fi

# Test 8: Reading from stdin
echo "Test 8: Reading from stdin"
input="1 2 3
0	1	4
5	6	0"
output=$(echo -e "$input" | ./$EXEC)
expected="-24.000	18.000	5.000
20.000	-15.000	-4.000
-5.000	4.000	1.000"

if [ "$output" = "$expected" ]; then
    echo "$output"
    echo "[PASS]"
    echo ""
else
    echo "Failed: got"
    echo "$output"
    echo "expected"
    echo "$expected"
    exit 1
fi

# Test 9: Invalid matrix format (missing values in row)
cat > $TESTS_DIR/matrix9.txt << EOF
1	2
2	4	6
1	2	3
EOF

echo "Test 9: Invalid matrix format (missing values)"
output=$(./$EXEC $TESTS_DIR/matrix9.txt)
if [ "$output" = "Invalid matrix format" ]; then
    echo "$output"
    echo "[PASS]"
    echo ""
else
    echo "Failed: got '$output', expected 'Invalid matrix format'"
    exit 1
fi

# Test 10: Another valid matrix
cat > $TESTS_DIR/matrix10.txt << EOF
2	0	0
0	2	0
0	0	2
EOF

echo "Test 10: Identity-like matrix"
output=$(./$EXEC $TESTS_DIR/matrix10.txt)

if [[ $output != *"Error"* ]] && [[ $output != *"Non-invertible"* ]] && [[ $output != *"Invalid"* ]]; then
    echo "$output"
    echo "[PASS]"
    echo ""
else
    echo "Failed: got '$output'"
    exit 1
fi

# Test 11: invalid
cat > $TESTS_DIR/matrix10.txt << EOF
0	0	0
0	0	0
0	0	0
EOF

echo "Test 11: invalid"
output=$(./$EXEC $TESTS_DIR/matrix11.txt)

if [ $? -ne 0 ]; then
    echo "[PASS]"
    echo ""
else
    echo "Failed: got '$output'"
    exit 1
fi

echo "All tests passed!"
rm -rf $TESTS_DIR
exit 0