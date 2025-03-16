#!/usr/bin/env bats

@test "Simple Command" {
    run "./dsh" <<EOF
echo Hello, World!
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="Hello,World!localmodecmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Pipes with Multiple Commands" {
    run "./dsh" <<EOF
ls | grep dsh | sort
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="localmodedshdsh.dSYMdsh_cli.cdshlib.cdshlib.hlocalmodelocalmodecmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}


@test "Built-in cd Command" {
    run "./dsh" <<EOF
cd ..
pwd
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="$(pwd)localmodecmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Multiple Commands on One Line" {
    run "./dsh" <<EOF
echo first; echo second; echo third
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="first;echosecond;echothirdlocalmodecmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Piping with Multiple Filters" {
    run "./dsh" <<EOF
echo -e "alpha\nbeta\ngamma" | grep a | sort
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="localmode-ealpha\nbeta\ngammalocalmodelocalmodelocalmodecmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}

@test "Exit Command" {
    run "./dsh" <<EOF
exit
EOF

    stripped_output=$(echo "$output" | tr -d '[:space:]')
    expected_output="localmodecmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]
}
