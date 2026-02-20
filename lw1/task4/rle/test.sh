#!/bin/bash

EXEC=./rle
TESTS_DIR=./tests

echo "Running RLE tests..."

# Create test dir
mkdir -p $TESTS_DIR

# Test 1: Empty file
echo "Test 1: Empty file packing"
touch $TESTS_DIR/empty.txt
./$EXEC pack $TESTS_DIR/empty.txt $TESTS_DIR/empty_packed.bin
if [ $? -eq 0 ] && [ ! -s $TESTS_DIR/empty_packed.bin ]; then
    echo "[PASS]"
else
    echo "[FAIL]: Empty file packing failed"
    exit 1
fi

echo "Test 1b: Empty file unpacking"
./$EXEC unpack $TESTS_DIR/empty_packed.bin $TESTS_DIR/empty_unpacked.txt
if [ $? -eq 0 ] && [ ! -s $TESTS_DIR/empty_unpacked.txt ]; then
    echo "[PASS]"
else
    echo "[FAIL]: Empty file unpacking failed"
    exit 1
fi

# Test 2: Simple sequence AAA BBBB C
echo -ne "AAABBBBC" > $TESTS_DIR/simple.txt

echo "Test 2: Simple sequence packing"
./$EXEC pack $TESTS_DIR/simple.txt $TESTS_DIR/simple_packed.bin
if [ $? -eq 0 ]; then
    # Expected: 3,'A', 4,'B', 1,'C'
    expected_len=6
    actual_len=$(stat -f%z $TESTS_DIR/simple_packed.bin 2>/dev/null || stat -c%s $TESTS_DIR/simple_packed.bin)
    if [ "$actual_len" -eq "$expected_len" ]; then
        echo "[PASS]"
    else
        echo "[FAIL]: Expected length $expected_len, got $actual_len"
        exit 1
    fi
else
    echo "[FAIL]: Simple sequence packing failed"
    exit 1
fi

echo "Test 2b: Simple sequence unpacking"
./$EXEC unpack $TESTS_DIR/simple_packed.bin $TESTS_DIR/simple_unpacked.txt
if [ $? -eq 0 ]; then
    cmp -s $TESTS_DIR/simple.txt $TESTS_DIR/simple_unpacked.txt
    if [ $? -eq 0 ]; then
        echo "[PASS]"
    else
        echo "[FAIL]: Unpacked content doesn't match original"
        exit 1
    fi
else
    echo "[FAIL]: Simple sequence unpacking failed"
    exit 1
fi

# Test 3: Sequence of 255 same bytes
python3 -c "print('A' * 255, end='')" > $TESTS_DIR/seq255.txt

echo "Test 3: 255-byte sequence packing"
./$EXEC pack $TESTS_DIR/seq255.txt $TESTS_DIR/seq255_packed.bin
if [ $? -eq 0 ]; then
    # Expected: 255,'A'
    expected_len=2
    actual_len=$(stat -f%z $TESTS_DIR/seq255_packed.bin 2>/dev/null || stat -c%s $TESTS_DIR/seq255_packed.bin)
    if [ "$actual_len" -eq "$expected_len" ]; then
        echo "[PASS]"
    else
        echo "[FAIL]: Expected length $expected_len, got $actual_len"
        exit 1
    fi
else
    echo "[FAIL]: 255-byte sequence packing failed"
    exit 1
fi

echo "Test 3b: 255-byte sequence unpacking"
./$EXEC unpack $TESTS_DIR/seq255_packed.bin $TESTS_DIR/seq255_unpacked.txt
if [ $? -eq 0 ]; then
    cmp -s $TESTS_DIR/seq255.txt $TESTS_DIR/seq255_unpacked.txt
    if [ $? -eq 0 ]; then
        echo "[PASS]"
    else
        echo "[FAIL]: Unpacked 255-byte content doesn't match original"
        exit 1
    fi
else
    echo "[FAIL]: 255-byte sequence unpacking failed"
    exit 1
fi

# Test 4: Sequence of 256 same bytes
python3 -c "print('B' * 256, end='')" > $TESTS_DIR/seq256.txt

echo "Test 4: 256-byte sequence packing"
./$EXEC pack $TESTS_DIR/seq256.txt $TESTS_DIR/seq256_packed.bin
if [ $? -eq 0 ]; then
    # Expected: 255,'B', 1,'B'
    expected_len=4
    actual_len=$(stat -f%z $TESTS_DIR/seq256_packed.bin 2>/dev/null || stat -c%s $TESTS_DIR/seq256_packed.bin)
    if [ "$actual_len" -eq "$expected_len" ]; then
        echo "[PASS]"
    else
        echo "[FAIL]: Expected length $expected_len, got $actual_len"
        exit 1
    fi
else
    echo "[FAIL]: 256-byte sequence packing failed"
    exit 1
fi

echo "Test 4b: 256-byte sequence unpacking"
./$EXEC unpack $TESTS_DIR/seq256_packed.bin $TESTS_DIR/seq256_unpacked.txt
if [ $? -eq 0 ]; then
    cmp -s $TESTS_DIR/seq256.txt $TESTS_DIR/seq256_unpacked.txt
    if [ $? -eq 0 ]; then
        echo "[PASS]"
    else
        echo "[FAIL]: Unpacked 256-byte content doesn't match original"
        exit 1
    fi
else
    echo "[FAIL]: 256-byte sequence unpacking failed"
    exit 1
fi

# Test 5: Sequence of 257 same bytes
python3 -c "print('C' * 257, end='')" > $TESTS_DIR/seq257.txt

echo "Test 5: 257-byte sequence packing"
./$EXEC pack $TESTS_DIR/seq257.txt $TESTS_DIR/seq257_packed.bin
if [ $? -eq 0 ]; then
    # Expected: 255,'C', 2,'C'
    expected_len=4
    actual_len=$(stat -f%z $TESTS_DIR/seq257_packed.bin 2>/dev/null || stat -c%s $TESTS_DIR/seq257_packed.bin)
    if [ "$actual_len" -eq "$expected_len" ]; then
        echo "[PASS]"
    else
        echo "[FAIL]: Expected length $expected_len, got $actual_len"
        exit 1
    fi
else
    echo "[FAIL]: 257-byte sequence packing failed"
    exit 1
fi

echo "Test 5b: 257-byte sequence unpacking"
./$EXEC unpack $TESTS_DIR/seq257_packed.bin $TESTS_DIR/seq257_unpacked.txt
if [ $? -eq 0 ]; then
    cmp -s $TESTS_DIR/seq257.txt $TESTS_DIR/seq257_unpacked.txt
    if [ $? -eq 0 ]; then
        echo "[PASS]"
    else
        echo "[FAIL]: Unpacked 257-byte content doesn't match original"
        exit 1
    fi
else
    echo "[FAIL]: 257-byte sequence unpacking failed"
    exit 1
fi

# Test 6: Mixed content
echo -ne "AAAAABBBBBCCCCCDD" > $TESTS_DIR/mixed.txt

echo "Test 6: Mixed content packing"
./$EXEC pack $TESTS_DIR/mixed.txt $TESTS_DIR/mixed_packed.bin
if [ $? -eq 0 ]; then
    echo "[PASS]"
else
    echo "[FAIL]: Mixed content packing failed"
    exit 1
fi

echo "Test 6b: Mixed content unpacking"
./$EXEC unpack $TESTS_DIR/mixed_packed.bin $TESTS_DIR/mixed_unpacked.txt
if [ $? -eq 0 ]; then
    cmp -s $TESTS_DIR/mixed.txt $TESTS_DIR/mixed_unpacked.txt
    if [ $? -eq 0 ]; then
        echo "[PASS]"
    else
        echo "[FAIL]: Unpacked mixed content doesn't match original"
        exit 1
    fi
else
    echo "[FAIL]: Mixed content unpacking failed"
    exit 1
fi

# Test 7: Invalid unpack - odd number of bytes
echo -ne "\x03\x41\x04" > $TESTS_DIR/invalid_odd.bin  # 3,'A', 4 but no following byte

echo "Test 7: Invalid unpack - odd number of bytes"
./$EXEC unpack $TESTS_DIR/invalid_odd.bin $TESTS_DIR/invalid_odd_out.txt
if [ $? -ne 0 ]; then
    echo "[PASS]"
else
    echo "[FAIL]: Should have failed on odd-length packed file"
    exit 1
fi

# Test 8: Invalid unpack - zero count
echo -ne "\x00\x41" > $TESTS_DIR/invalid_zero.bin  # 0,'A' - invalid count

echo "Test 8: Invalid unpack - zero count"
./$EXEC unpack $TESTS_DIR/invalid_zero.bin $TESTS_DIR/invalid_zero_out.txt
if [ $? -ne 0 ]; then
    echo "[PASS]"
else
    echo "[FAIL]: Should have failed on zero count"
    exit 1
fi

# Test 9: File with byte value 255
python3 -c "import sys; sys.stdout.buffer.write(b'\xff' * 5)" > $TESTS_DIR/byte255.txt

echo "Test 9: Byte value 255 packing/unpacking"
./$EXEC pack $TESTS_DIR/byte255.txt $TESTS_DIR/byte255_packed.bin
if [ $? -eq 0 ]; then
    ./$EXEC unpack $TESTS_DIR/byte255_packed.bin $TESTS_DIR/byte255_unpacked.txt
    if [ $? -eq 0 ]; then
        cmp -s $TESTS_DIR/byte255.txt $TESTS_DIR/byte255_unpacked.txt
        if [ $? -eq 0 ]; then
            echo "[PASS]"
        else
            echo "[FAIL]: Unpacked byte 255 content doesn't match original"
            exit 1
        fi
    else
        echo "[FAIL]: Byte 255 unpacking failed"
        exit 1
    fi
else
    echo "[FAIL]: Byte 255 packing failed"
    exit 1
fi

# Test 10: Help option
echo "Test 10: Help option"
if ./$EXEC -h > /dev/null 2>&1; then
    echo "[PASS]"
else
    echo "[FAIL]: Help option failed"
    exit 1
fi

# Test 11: Single byte file
echo -ne "X" > $TESTS_DIR/single.txt
echo "Test 11: Single byte packing/unpacking"
./$EXEC pack $TESTS_DIR/single.txt $TESTS_DIR/single_packed.bin
if [ $? -eq 0 ]; then
    # Expected: 1, 'X' -> 2 bytes
    actual_len=$(stat -f%z $TESTS_DIR/single_packed.bin 2>/dev/null || stat -c%s $TESTS_DIR/single_packed.bin)
    if [ "$actual_len" -eq 2 ]; then
        ./$EXEC unpack $TESTS_DIR/single_packed.bin $TESTS_DIR/single_unpacked.txt
        cmp -s $TESTS_DIR/single.txt $TESTS_DIR/single_unpacked.txt && echo "[PASS]" || { echo "[FAIL]: Content mismatch"; exit 1; }
    else
        echo "[FAIL]: Expected length 2, got $actual_len"
        exit 1
    fi
else
    echo "[FAIL]: Single byte packing failed"
    exit 1
fi

# Test 12: Invalid operation argument
echo "Test 12: Invalid operation argument"
./$EXEC compress input.txt output.txt 2>/dev/null
if [ $? -ne 0 ]; then
    echo "[PASS]"
else
    echo "[FAIL]: Should fail on invalid operation"
    exit 1
fi

# Test 13: Missing arguments
echo "Test 13: Missing arguments"
./$EXEC pack input.txt 2>/dev/null
if [ $? -ne 0 ]; then
    echo "[PASS]"
else
    echo "[FAIL]: Should fail on missing arguments"
    exit 1
fi

# Test 14: Non-existent input file
echo "Test 14: Non-existent input file"
./$EXEC pack /non/existent/file.txt $TESTS_DIR/out.bin 2>/dev/null
if [ $? -ne 0 ]; then
    echo "[PASS]"
else
    echo "[FAIL]: Should fail on non-existent file"
    exit 1
fi

# Test 15: All possible byte values (0-255)
python3 -c "import sys; sys.stdout.buffer.write(bytes(range(256)))" > $TESTS_DIR/all_bytes.bin
echo "Test 15: All byte values (0-255) roundtrip"
./$EXEC pack $TESTS_DIR/all_bytes.bin $TESTS_DIR/all_bytes_packed.bin
if [ $? -eq 0 ]; then
    ./$EXEC unpack $TESTS_DIR/all_bytes_packed.bin $TESTS_DIR/all_bytes_unpacked.bin
    if [ $? -eq 0 ]; then
        cmp -s $TESTS_DIR/all_bytes.bin $TESTS_DIR/all_bytes_unpacked.bin && echo "[PASS]" || { echo "[FAIL]: All bytes content mismatch"; exit 1; }
    else
        echo "[FAIL]: Unpacking all bytes failed"
        exit 1
    fi
else
    echo "[FAIL]: Packing all bytes failed"
    exit 1
fi

# Test 16: Alternating pattern (expansion test)
echo -ne "ABABABABABABABAB" > $TESTS_DIR/alternating.txt
echo "Test 16: Alternating pattern (size expansion)"
./$EXEC pack $TESTS_DIR/alternating.txt $TESTS_DIR/alternating_packed.bin
if [ $? -eq 0 ]; then
    # Original 16 bytes. Packed should be 16 * 2 = 32 bytes (1,'A', 1,'B'...)
    actual_len=$(stat -f%z $TESTS_DIR/alternating_packed.bin 2>/dev/null || stat -c%s $TESTS_DIR/alternating_packed.bin)
    if [ "$actual_len" -eq 32 ]; then
        ./$EXEC unpack $TESTS_DIR/alternating_packed.bin $TESTS_DIR/alternating_unpacked.txt
        cmp -s $TESTS_DIR/alternating.txt $TESTS_DIR/alternating_unpacked.txt && echo "[PASS]" || { echo "[FAIL]: Alternating content mismatch"; exit 1; }
    else
        echo "[FAIL]: Expected expanded length 32, got $actual_len"
        exit 1
    fi
else
    echo "[FAIL]: Alternating pattern packing failed"
    exit 1
fi

echo "All tests passed!"
rm -rf $TESTS_DIR
exit 0