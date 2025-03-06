#!/usr/bin/env bats

# Test if 'ls' runs successfully
@test "Check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF
    [ "$status" -eq 0 ]
}

# Test if a built-in 'cd' command executes correctly
@test "Check cd changes directory" {
    run ./dsh <<EOF
cd /
pwd
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == "/"* ]]
}

# Test if a single command runs successfully
@test "Check echo runs successfully" {
    run ./dsh <<EOF
echo "Hello, world!"
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"Hello, world!"* ]]
}

# Test a command with arguments
@test "Check grep finds word in a file" {
    echo "Hello BATS test" > testfile.txt
    run ./dsh <<EOF
grep "BATS" testfile.txt
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"Hello BATS test"* ]]
    rm testfile.txt
}

# Test multiple pipes in a pipeline
@test "Check complex pipeline ls -1 | grep txt | wc -l" {
    touch file1.txt file2.txt file3.log
    run ./dsh <<EOF
ls -1 | grep txt | wc -l
EOF
    [ "$status" -eq 0 ]
    [[ "$output" -eq 2 ]]
    rm file1.txt file2.txt file3.log
}

# Test executing exit command
@test "Check exit command terminates shell" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
}

# Test if 'mkdir' creates a new directory
@test "Check mkdir creates a directory" {
    run ./dsh <<EOF
mkdir testdir
EOF
    [ "$status" -eq 0 ]
    [ -d "testdir" ]
    rmdir testdir
}

# Test if 'rmdir' removes a directory
@test "Check rmdir removes a directory" {
    mkdir testdir
    run ./dsh <<EOF
rmdir testdir
EOF
    [ "$status" -eq 0 ]
    [ ! -d "testdir" ]
}

# Test if 'touch' creates a file
@test "Check touch creates a file" {
    run ./dsh <<EOF
touch testfile
EOF
    [ "$status" -eq 0 ]
    [ -f "testfile" ]
    rm testfile
}

# Test if 'cat' reads a file correctly
@test "Check cat displays file contents" {
    echo "Hello World" > testfile.txt
    run ./dsh <<EOF
cat testfile.txt
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == "Hello World" ]]
    rm testfile.txt
}

# Test a background process execution
@test "Check background process execution" {
    run ./dsh <<EOF
sleep 1 &
EOF
    [ "$status" -eq 0 ]
}

# Test 'rm' deletes a file
@test "Check rm deletes a file" {
    touch testfile.txt
    run ./dsh <<EOF
rm testfile.txt
EOF
    [ "$status" -eq 0 ]
    [ ! -f "testfile.txt" ]
}

# Test command substitution
@test "Check command substitution works" {
    run ./dsh <<EOF
echo "Today is \$(date +%A)"
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == "Today is "* ]]
}