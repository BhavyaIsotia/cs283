1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**: fgets() is a good choice I think because it reads an entire line of input at once so it makes it easy to process user commands. It also prevents buffer overflows since we can specify a maximum size for input. ALso fgets() can detect EOF (End of File) which is useful for handling input redirection in tests.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  We use malloc() to allocate memory for cmd_buff because it allows us to dynamically allocate only the memory we need at runtime instead of using a fixed-size array. This helps prevent wasting memory and avoids stack overflow if SH_CMD_MAX is too large.


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  Trimming spaces ensures that commands are parsed correctly. If we don’t remove the leading spaces the shell might not interpret a valid command as an empty one. Trailing spaces can also cause issues, especially when comparing strings or handling arguments. Without trimming, a user typing " ls " might be stored incorrectly, leading to unexpected behavior when executing commands.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  
    Example 1: ls > output.txt
    Redirects the output of ls to a file instead of printing it on the terminal.
    Challenge: We need to handle file creation and overwriting carefully.

    Example 2: sort < numbers.txt
    Reads input from a file instead of the keyboard.
    Challenge: We must open the file, read it correctly, and pass it as input to the command.

    Example 3: echo "Hello" >> log.txt
    Adds output to the end of a file instead of overwriting it.
    Challenge: We need to open the file in append mode and ensure multiple processes don’t write simultaneously.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**: Redirection (>, <) deals with files—it sends command output to a file or reads input from a file.
    Piping (|) connects the output of one command to the input of another command, like ls | grep ".c".
    Key difference: Redirection works with files, while piping connects commands directly.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**: STDOUT is for normal command output (ls, echo).
    STDERR is for error messages (command not found, file not found).
    If we redirect STDOUT to a file (ls > files.txt), we don’t want error messages mixing with the output.
    In our shell... handling STDERR separately helps users debug issues without corrupting valid output.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  Our shell should check the return status of each command and print an error message if it fails. If a command outputs both STDOUT and STDERR, we should allow the user to merge them using 2>&1, FOR e.g., command 2>&1 > output.txt. This way, users can choose whether to keep them separate or combined.

