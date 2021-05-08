#!/bin/bash

# Trigger all your test cases with this script

printf "1. NO FUNC LABEL 0 TEST\nIn objdump_x2017: \n" > tests/test_actual.out
args="./objdump_x2017 `sed -n 1p tests/test.args`"
eval $args >> tests/test_actual.out 2>&1
printf "In vm_x2017: " >> tests/test_actual.out
args="./vm_x2017 `sed -n 1p tests/test.args`"
eval $args >> tests/test_actual.out 2>&1

printf "\n2. STK NOT EXIST TEST\nIn objdump_x2017: " >> tests/test_actual.out
args="./objdump_x2017 `sed -n 2p tests/test.args`"
eval $args >> tests/test_actual.out 2>&1
printf "In vm_x2017: " >> tests/test_actual.out
args="./vm_x2017 `sed -n 2p tests/test.args`"
eval $args >> tests/test_actual.out 2>&1

printf "\n3. CAL FUNC NOT EXIST TEST\nIn objdump_x2017: \n" >> tests/test_actual.out
args="./objdump_x2017 `sed -n 3p tests/test.args`"
eval $args >> tests/test_actual.out 2>&1
printf "In vm_x2017: " >> tests/test_actual.out
args="./vm_x2017 `sed -n 3p tests/test.args`"
eval $args >> tests/test_actual.out 2>&1

printf "\n4. INCOMPATIBLE TYPE FOR EQU TEST\nIn objdump_x2017: \n" >> tests/test_actual.out
args="./objdump_x2017 `sed -n 4p tests/test.args`"
eval $args >> tests/test_actual.out 2>&1
printf "In vm_x2017: " >> tests/test_actual.out
args="./vm_x2017 `sed -n 4p tests/test.args`"
eval $args >> tests/test_actual.out 2>&1

printf "\n5. INCOMPATIBLE TYPE FOR REF TEST\nIn objdump_x2017: \n" >> tests/test_actual.out
args="./objdump_x2017 `sed -n 5p tests/test.args`"
eval $args >> tests/test_actual.out 2>&1
printf "In vm_x2017: " >> tests/test_actual.out
args="./vm_x2017 `sed -n 5p tests/test.args`"
eval $args >> tests/test_actual.out 2>&1

printf "\n6. INFINITE CAL TEST\nIn objdump_x2017: \n" >> tests/test_actual.out
args="./objdump_x2017 `sed -n 6p tests/test.args`"
eval $args >> tests/test_actual.out 2>&1
printf "In vm_x2017: " >> tests/test_actual.out
args="./vm_x2017 `sed -n 6p tests/test.args`"
eval $args >> tests/test_actual.out 2>&1

printf "\n7. FILE NOT FOUND TEST\nIn objdump_x2017: " >> tests/test_actual.out
args="./objdump_x2017 `sed -n 7p tests/test.args`"
eval $args >> tests/test_actual.out 2>&1
printf "In vm_x2017: " >> tests/test_actual.out
args="./vm_x2017 `sed -n 7p tests/test.args`"
eval $args >> tests/test_actual.out 2>&1

diff tests/test_actual.out tests/test_expected.out


if [ $? -eq 0 ]; then
    echo "all tests passed"
else
    echo "tests failed, please check output files"
fi

