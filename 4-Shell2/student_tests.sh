#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suite in this file

@test "It handles quoted spaces correctly" {
    run ./dsh <<EOF                
echo "hello   world"
EOF

    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')
    expected_output="hello   worlddsh2> dsh2> cmd loop returned 0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Exit command terminates shell" {
    run ./dsh <<EOF
exit
EOF

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$status" -eq 0 ]
}

@test "Runs ls command successfully" {
    run ./dsh <<EOF
ls
EOF

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$status" -eq 0 ]
}

@test "Runs pwd command successfully" {
    run ./dsh <<EOF
pwd
EOF

    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v')

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"

    [[ "$stripped_output" == *$(pwd)* ]]

    [ "$status" -eq 0 ]
}

@test "Handles empty input without errors" {
    run ./dsh <<EOF
EOF

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"

    [ "$status" -eq 0 ]
}
