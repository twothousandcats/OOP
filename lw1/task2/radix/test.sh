#!/bin/bash

EXEC=./radix
TESTS_DIR=./tests

echo "Running tests..."

# Тест 1: Перевод из 16 в 10
echo "Test 1: 16 10 1F"
output=$($EXEC 16 10 1F)
if [ $? -eq 0 ] && [ "$output" = "31" ]; then
    echo $output
    echo "[PASS]"
    echo ""
else
    echo "Failed: got '$output', expected '31'"
    exit 1
fi

# Тест 2: Перевод из 10 в 2
echo "Test 2: 10 2 10"
output=$($EXEC 10 2 10)
if [ $? -eq 0 ] && [ "$output" = "1010" ]; then
    echo $output
    echo "[PASS]"
    echo ""
else
    echo "Failed: got '$output', expected '1010'"
    exit 1
fi

# Тест 3: Отрицательное число
echo "Test 3: 10 16 -255"
output=$($EXEC 10 16 -255)
if [ $? -eq 0 ] && [ "$output" = "-FF" ]; then
    echo $output
    echo "[PASS]"
    echo ""
else
    echo "Failed: got '$output', expected '-FF'"
    exit 1
fi

# Тест 4: Ноль
echo "Test 4: 10 8 0"
output=$($EXEC 10 8 0)
if [ $? -eq 0 ] && [ "$output" = "0" ]; then
    echo $output
    echo "[PASS]"
    echo ""
else
    echo "Failed: got '$output', expected '0'"
    exit 1
fi

# Тест 5: Из 2 в 16
echo "Test 5: 2 16 11111111"
output=$($EXEC 2 16 11111111)
if [ $? -eq 0 ] && [ "$output" = "FF" ]; then
    echo $output
    echo "[PASS]"
    echo ""
else
    echo "Failed: got '$output', expected 'FF'"
    exit 1
fi

# Тест 6: Из 36 в 10 (буквы)
echo "Test 6: 36 10 Z"
output=$($EXEC 36 10 Z)
if [ $? -eq 0 ] && [ "$output" = "35" ]; then
    echo $output
    echo "[PASS]"
    echo ""
else
    echo "Failed: got '$output', expected '35'"
    exit 1
fi

# Тест 7: Ошибка - неверная система счисления
echo "Test 7: 1 10 10 (ошибка)"
output=$($EXEC 1 10 10)
if [ $? -ne 0 ]; then
    echo "(error as expected)"
    echo "[PASS]"
    echo ""
else
    echo "Failed: should have failed but didn't"
    exit 1
fi

# Тест 8: Ошибка - недопустимая цифра
echo "Test 8: 10 16 G (ошибка - G не цифра в 10-ричной)"
output=$($EXEC 10 16 G)
if [ $? -ne 0 ]; then
    echo "(error as expected)"
    echo "[PASS]"
    echo ""
else
    echo "Failed: should have failed but didn't"
    exit 1
fi

# Тест 9: Максимальное значение int
echo "Test 9: 10 16 2147483647"
output=$($EXEC 10 16 2147483647)
if [ $? -eq 0 ] && [ "$output" = "7FFFFFFF" ]; then
    echo $output
    echo "[PASS]"
    echo ""
else
    echo "Failed: got '$output', expected '7FFFFFFF'"
    exit 1
fi

# Тест 10: Минимальное значение int
echo "Test 10: 10 16 -2147483648"
output=$($EXEC 10 16 -2147483648)
if [ $? -eq 0 ] && [ "$output" = "-80000000" ]; then
    echo $output
    echo "[PASS]"
    echo ""
else
    echo "Failed: got '$output', expected '-80000000'"
    exit 1
fi

echo "All tests passed!"
exit 0