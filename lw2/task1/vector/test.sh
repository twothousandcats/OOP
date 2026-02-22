#!/bin/bash

EXEC=./vector

echo "Running tests..."

# Тест 1: Обычный случай
echo "Test 1: Mixed numbers"
output=$(echo "1.0 2 3.659512" | $EXEC)
if [ $? -eq 0 ] && [ "$output" = "3.220 4.220 5.879 " ]; then
    echo "$output"
    echo "[PASS]"
    echo ""
else
    echo "Failed: got '$output', expected '3.220 4.220 5.879 '"
    exit 1
fi

# Тест 2: Отрицательные и положительные
echo "Test 2: Negative and positive"
output=$(echo "4 16 -30 10" | $EXEC)
if [ $? -eq 0 ] && [ "$output" = "-20.000 14.000 20.000 26.000 " ]; then
    echo "$output"
    echo "[PASS]"
    echo ""
else
    echo "Failed: got '$output', expected '-20.000 14.000 20.000 26.000 '"
    exit 1
fi

# Тест 3: Только отрицательные (только сортировка)
echo "Test 3: Only negative numbers"
output=$(echo "-1.0004000 -703 -3.659512 -11" | $EXEC)
if [ $? -eq 0 ] && [ "$output" = "-703.000 -11.000 -3.660 -1.000 " ]; then
    echo "$output"
    echo "[PASS]"
    echo ""
else
    echo "Failed: got '$output', expected '-703.000 -11.000 -3.660 -1.000 '"
    exit 1
fi

# Тест 4: Ошибка ввода
echo "Test 4: Invalid input (symbol '-')"
output=$(echo "- 2 3" | $EXEC)
if [ $? -eq 0 ] && [ "$output" = "ERROR" ]; then
    echo "$output"
    echo "[PASS]"
    echo ""
else
    echo "Failed: got '$output', expected 'ERROR'"
    exit 1
fi

# Тест 5: Пустой ввод
echo "Test 5: Empty input"
output=$(echo "" | $EXEC)
if [ $? -eq 0 ] && [ "$output" = "" ]; then
    echo "(empty output)"
    echo "[PASS]"
    echo ""
else
    echo "Failed: got '$output', expected empty output"
    exit 1
fi

# Тест 6: Одно положительное число
echo "Test 6: Single positive number"
output=$(echo "5.5" | $EXEC)
if [ $? -eq 0 ] && [ "$output" = "11.000 " ]; then
    echo "$output"
    echo "[PASS]"
    echo ""
else
    echo "Failed: got '$output', expected '11.000 '"
    exit 1
fi

# Тест 7: Одно отрицательное число
echo "Test 7: Single negative number"
output=$(echo "-5.5" | $EXEC)
# Положительных нет, среднее = 0. Результат -5.5
if [ $? -eq 0 ] && [ "$output" = "-5.500 " ]; then
    echo "$output"
    echo "[PASS]"
    echo ""
else
    echo "Failed: got '$output', expected '-5.500 '"
    exit 1
fi

# Тест 8: Некорректный ввод (буквы)
echo "Test 8: Invalid input (letters)"
output=$(echo "1.0 abc 3.0" | $EXEC)
if [ $? -eq 0 ] && [ "$output" = "ERROR" ]; then
    echo "$output"
    echo "[PASS]"
    echo ""
else
    echo "Failed: got '$output', expected 'ERROR'"
    exit 1
fi

# Тест 9: Табуляции и переносы строк вместо пробелов
echo "Test 9: Tabs and newlines"
output=$(printf "1.0\t2.0\n3.0" | $EXEC)
if [ $? -eq 0 ] && [ "$output" = "3.000 4.000 5.000 " ]; then
    echo "$output"
    echo "[PASS]"
    echo ""
else
    echo "Failed: got '$output', expected '3.000 4.000 5.000 '"
    exit 1
fi

# Тест 10: Граничные значения (большие числа)
echo "Test 10: Large numbers"
output=$(echo "1000000.123456 -1000000.123456" | $EXEC)
# Положительные: 1000000.123456 -> среднее = оно же.
# Элементы:
# 1000000.123456 + 1000000.123456 = 2000000.246912 -> 2000000.247
# -1000000.123456 + 1000000.123456 = 0.000000 -> 0.000
# Сортировка: 0.000 2000000.247
if [ $? -eq 0 ] && [ "$output" = "0.000 2000000.247 " ]; then
    echo "$output"
    echo "[PASS]"
    echo ""
else
    echo "Failed: got '$output', expected '0.000 2000000.247 '"
    exit 1
fi

echo "All tests passed!"
exit 0