#!/bin/bash

EXEC=./replace
TESTS_DIR=./tests
OUTPUT=output_test.txt

echo "Running tests..."

# Тест 1: Командная строка - нормальная замена
echo "Test 1: Normal replacement via command line"
echo "hello world" > $TESTS_DIR/input1.txt
$EXEC $TESTS_DIR/input1.txt $OUTPUT "hello" "hi"
if [ $? -eq 0 ] && [ "$(cat $OUTPUT)" == "hi world" ]; then
    echo "[PASS]"
else
    echo "[FAIL]"
fi

# Тест 2: Командная строка - пустая искомая строка
echo -e "\nTest 2: Empty search string via command line"
echo "hello world" > $TESTS_DIR/input2.txt
$EXEC $TESTS_DIR/input2.txt $OUTPUT "" "test"
if [ $? -eq 0 ] && [ "$(cat $OUTPUT)" == "hello world" ]; then
    echo "[PASS]"
else
    echo "[FAIL]"
fi

# Тест 3: Командная строка - ошибка открытия файла
echo -e "\nTest 3: File not found via command line"
$EXEC nonexistent.txt $OUTPUT "search" "replace"
if [ $? -eq 1 ]; then
    echo "[PASS]"
else
    echo "[FAIL]"
fi

# Тест 4: stdin - нормальная замена
echo -e "\nTest 4: Normal replacement via stdin"
printf "ma\nmama\nmama delala pelmeni\n" | $EXEC > $OUTPUT
if [ $? -eq 0 ] && [ "$(cat $OUTPUT)" == "mamamama delala pelmeni" ]; then
    echo "[PASS]"
else
    echo "[FAIL]"
fi

# Тест 5: stdin - ошибка ввода (нет строки замены)
echo -e "\nTest 5: Missing replace string via stdin"
printf "hello\n" | $EXEC > $OUTPUT
if [ $? -eq 1 ]; then
    echo "[PASS]"
else
    echo "[FAIL]"
fi

# Тест 6: Командная строка - сложный случай с частичным совпадением
echo -e "\nTest 6: Complex pattern with partial match"
echo "12312312345" > $TESTS_DIR/input3.txt
$EXEC $TESTS_DIR/input3.txt $OUTPUT "1231234" "XYZ"
if [ $? -eq 0 ] && [ "$(cat $OUTPUT)" == "123XYZ5" ]; then
    echo "[PASS]"
else
    echo "[FAIL]"
fi

# Тест 7: Проверка прав на чтение файла
echo -e "\nTest 7: Read permissions error"
touch $TESTS_DIR/no_read.txt
chmod 000 $TESTS_DIR/no_read.txt
$EXEC $TESTS_DIR/no_read.txt $OUTPUT "search" "replace"
if [ $? -eq 1 ]; then
    echo "[PASS]"
else
    echo "[FAIL]"
fi
chmod 644 $TESTS_DIR/no_read.txt

# Тест 8: Пустые значения строк поиска и замены
echo -e "\nTest 8: Empty search and replace strings"
echo "hello world" > $TESTS_DIR/input4.txt
$EXEC $TESTS_DIR/input4.txt $OUTPUT "" ""
if [ $? -eq 0 ] && [ "$(cat $OUTPUT)" == "hello world" ]; then
    echo "[PASS]"
else
    echo "[FAIL]"
fi

rm -f $OUTPUT

echo "All tests passed!"
exit 0