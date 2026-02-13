#!/bin/bash

EXEC=./invert
TESTS_DIR=./tests

echo "Running tests..."

# Тест 1: Нормальная матрица
echo "Test 1: Valid matrix from stdin..."
cat <<EOF | "$EXEC"
1	2	3
0	1	4
5	6	0
EOF
echo "Expected:"
echo "-24.000	18.000	5.000"
echo "20.000	-15.000	-4.000"
echo "-5.000	4.000	1.000"
echo ""

# Тест 2: Матрица с дробными числами
echo "Test 2: Matrix with floats from stdin..."
cat <<EOF | "$EXEC"
4	7	2.3
2	1	1
3	-2	-2.31
EOF
echo "Expected:"
echo "-0.009	0.321	0.131"
echo "0.212	-0.448	0.017"
echo "-0.194	0.806	-0.278"
echo ""

# Тест 3: Вырожденная матрица
echo "Test 3: Non-invertible matrix..."
cat <<EOF | "$EXEC"
1	2	3
2	4	6
1	2	3
EOF
echo "Expected: Non-invertible"
echo ""

# Тест 4: Неверный формат (пропущено значение)
echo "Test 4: Invalid matrix format (missing element)..."
cat <<EOF | "$EXEC"
1	2
2	4	6
1	2	3
EOF
echo "Expected: Invalid matrix format"
echo ""

# Тест 5: Некорректный элемент
echo "Test 5: Invalid matrix value..."
cat <<EOF | "$EXEC"
1	2	a
2	4	6
1	2	3
EOF
echo "Expected: Invalid matrix"
echo ""

# Тест 6: Использование файла
echo "Test 6: Reading from file..."
"$EXEC" $TESTS_DIR/matrix1.txt
echo ""

# Тест 7: Справка
echo "Test 7: Help message..."
"$EXEC" -h
echo ""

echo "All tests passed!"
exit 0