1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  We use fork() before calling execvp() because execvp() replaces the current process image with a new program. If we called execvp() directly in the main shell process... our shell would be replaced, and we would lose the ability to execute more commands. fork() creates a child process that executes the command while the parent process (the shell) continues running and can handle more commands.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If fork() fails it usually means the system is out of process resources (so like e.g., too many processes running). In my implementation, I check the return value of fork(). If it is -1, I print an error message like "error: failed to create process" and return an error code. This prevents the shell from crashing unexpectedly.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  execvp() searches for the command in the directories listed in the PATH environment variable. PATH contains a colon-separated list of directories where executable programs are located. If the command is in one of those directories, execvp() runs it; otherwise, it returns an error.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  wait() ensures that the parent process (the shell) waits for the child process to finish executing before continuing. If we didn’t call wait(), child processes would become "zombie processes" after they finish execution, which could later on use up system resources. Also... the shell would immediately print the prompt again before the command finishes, leading to a messy output.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  WEXITSTATUS(status) extracts the exit status of the child process after it terminates. This is useful because it allows the shell to determine whether a command succeeded or failed. It can also be used for scripting or debugging purposes.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  My implementation of build_cmd_buff() detects quoted strings and treats them as a single argument, even if they contain spaces. This is necessary because many commands require arguments with spaces ( lets for e.g., echo "Hello World" should treat "Hello World" as one argument instead of two). Without this handling, commands with quoted arguments wouldn’t work properly.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  In this assignment, I refactored my parsing logic to use cmd_buff_t instead of command_list_t, which simplified the way commands are stored and processed. I also improved handling of quoted arguments and whitespace. One unexpected challenge was ensuring that multiple spaces between arguments were ignored while still correctly handling quoted text. Debugging edge cases like echo " spaced out " was trickier than expected.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals are used for asynchronous communication between processes and the kernel. They allow processes to handle events like termination (SIGTERM), interrupts (SIGINT), or pauses (SIGSTOP). Unlike other IPC mechanisms (e.g., pipes, message queues), signals don’t involve direct data exchange but instead notify a process to perform some action.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGKILL (9): Immediately terminates a process and cannot be caught or ignored. Used when a process needs to be forcefully stopped. SIGTERM (15): Politely asks a process to terminate, allowing it to clean up before exiting. Used for graceful shutdowns. SIGINT (2): Sent when the user presses Ctrl+C, allowing a process to stop execution unless it explicitly handles the signal.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  SIGSTOP pauses a process, but unlike SIGINT, it cannot be caught or ignored. This is because SIGSTOP is designed to unconditionally stop a process until it is resumed with SIGCONT. The kernel ensures this behavior for process control reasons.
