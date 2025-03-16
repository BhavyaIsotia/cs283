1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

The client detects the end of a command’s output by checking for the EOF character (0x04) sent by the server. To handle partial reads, it uses a loop with recv(), reading data until EOF is received, ensuring the full response is collected before proceeding.


2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

Commands should be null-terminated (\0), and responses should end with an EOF marker (0x04) to define message boundaries. Without proper handling, issues like command merging, truncation, or indefinite blocking may occur.

3. Describe the general differences between stateful and stateless protocols.

Stateful: Maintains session data (e.g., TCP, HTTPS with cookies).
Stateless: Treats each request independently (e.g., UDP, basic HTTP requests).
In our shell, the server is stateful, maintaining a connection with each client.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

UDP is faster than TCP and is useful when low latency matters more than reliability, such as in video streaming, VoIP, gaming, and DNS, where occasional packet loss is acceptable.


5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

The OS provides sockets (via the Berkeley Sockets API), allowing applications to use socket(), bind(), connect(), send(), and recv() for network communication.