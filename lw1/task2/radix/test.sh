#!/bin/bash

EXEC=./radix
TESTS_DIR=./tests
OUTPUT=output_test.txt

echo "Running tests..."

# Тест 1: Перевод из 16 в 10
echo "Test 1: 16 10 1F"
$EXEC 16 10 1F
if [ $? -eq 0 ]; then
    echo "Expected: 31"
    echo ""
else
    echo "Failed"
    exit 1
fi

# Тест 2: Перевод из 10 в 2
echo "Test 2: 10 2 10"
$EXEC 10 2 10
if [ $? -eq 0 ]; then
    echo "Expected: 1010"
    echo ""
else
    echo "Failed"
    exit 1
fi

# Тест 3: Отрицательное число
echo "Test 3: 10 16 -255"
$EXEC 10 16 -255
if [ $? -eq 0 ]; then
    echo "Expected: -FF"
    echo ""
else
    echo "Failed"
    exit 1
fi

# Тест 4: Ноль
echo "Test 4:  10 8 0"
$EXEC 10 8 0
if [ $? -eq 0 ]; then
    echo "Expected: 0"
    echo ""
else
    echo "Failed"
    exit 1
fi

# Тест 5: Из 2 в 16
echo "Test 5:  2 16 11111111"
$EXEC 2 16 11111111
if [ $? -eq 0 ]; then
    echo "Expected: FF"
    echo ""
else
    echo "Failed"
    exit 1
fi

# Тест 6: Из 36 в 10 (буквы)
echo "Test 6:  36 10 Z"
$EXEC 36 10 Z
if [ $? -eq 0 ]; then
    echo "Expected: 35"
    echo ""
else
    echo "Failed"
    exit 1
fi

# Тест 7: Ошибка - неверная система счисления
echo "Test 7:  1 10 10 (ошибка)"
$EXEC 1 10 10
if [ $? -ne 0 ]; then
    echo "Expected: Error"
    echo ""
else
    echo "Should have failed but didn't"
    exit 1
fi

# Тест 8: Ошибка - недопустимая цифра
echo "Test 8:  10 16 G (ошибка - G не цифра в 10-ричной)"
$EXEC 10 16 G
if [ $? -ne 0 ]; then
    echo "Expected: Error"
    echo ""
else
    echo "Should have failed but didn't"
    exit 1
fi

# Тест 9: Максимальное значение int(вроде от архитектуры зависит)
echo "Test 9:  10 16 2147483647"
$EXEC 10 16 2147483647
if [ $? -eq 0 ]; then
    echo "Expected: 7FFFFFFF"
    echo ""
else
    echo "Failed"
    exit 1
fi

# Тест 10: Минимальное значение int
echo "Test 10:  10 16 -2147483648"
$EXEC 10 16 -2147483648
if [ $? -eq 0 ]; then
    echo "Expected: -80000000"
    echo ""
else
    echo "Failed"
    exit 1
fi

echo "All tests passed!"
exit 0