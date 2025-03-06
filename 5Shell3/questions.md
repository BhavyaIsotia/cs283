1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

_answer here_: My implementation ensures that all child processes complete before accepting new input by calling waitpid() for each forked process in the pipeline. After forking all processes, the parent process iterates through the array of process IDs (pids[i]) and waits for each one to finish execution. This guarantees that the shell does not return to the prompt until all commands in the pipeline have completed.
If I forgot to call waitpid() on all child processes...then these processes would become zombie processes after they complete, as their exit status would not be collected. Over time, collectign zombie processes could lead to resources getting exhausted which woul dmake the system unstable. Also the parent shell might return to the prompt before all child processes finish, leading to unexpected behavior (such as incomplete output or interference with subsequent commands.)



2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

_answer here_: Closing the unused pipe ends after calling dup2() is important so that we can ensure proper data flow and prevent resource leaks. So when a process redirects its input or output using dup2(), the original file descriptors (the pipe ends) become unnecessary and should be closed. If they remain open:
>The reading process might not detect an EOF condition, causing it to hang indefinitely waiting for more input.
>The system might run out of available file descriptors if too many open pipes collect.
>Unused write ends left open can prevent a process from terminating when it expects an EOF from the pipeline.
My implementation ensures that data flows correctly between processes and that resources are efficiently managed.



3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

_answer here_: The cd command is implemented as a built-in command as it modifies the shell’s working directory... which is a property of the shell process itself. If cd is executed as an external command using execvp(),then it would run in a separate child process, so basiclaly the directory change would only apply to that child and not persist when returning to the shell prompt.
Challenges of implementing cd as an external command include:
-The working directory would not change for the main shell, making cd ineffective.
-The only way to achieve the desired effect would be for the shell process itself to execute chdir(), which is only possible if cd is handled internally.
-Spawning a child process for cd would be unnecessary overhead, as it would always exit without affecting the parent process’s state.



4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

_answer here_: To allow an arbitrary number of piped commands, I would dynamically allocate memory for the pipeline instead of using a fixed-size array. Instead of defining command_list_t with a static commands[CMD_MAX], I would use malloc() to allocate space for command structures as needed and realloc() to expand it dynamically when more commands are added.
Trade-offs to consider:
-Memory Management -Using dynamic memory requires careful allocation and deallocation to avoid leaks. I would need to free memory explicitly after executing the pipeline.
-Performance Overhead -Using realloc() frequently may introduce some overhead compared to a statically allocated array. Preallocating memory in chunks (e.g., doubling the size when needed) could help reduce excessive reallocation.
-Complexity: Managing dynamic memory adds complexity to the implementation, requiring additional error handling to ensure memory is properly allocated and freed.

