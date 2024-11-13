
The objective of this assignment is to study inter-processor 
communication. A small test program is provided, which 
starts two communicating threads: a sender and a receiver. 

The threads use the functions send() and receive(), to send a buffer of 
data from the sender to the receiver. The receiver optionally checks the
validity of the received data (the txtest program), or simply goes to
receive the next buffer.

The threads operate in lock-step: only after the receiver has received the first buffer
does the sender send the next one. This is ensured using barriers. 

There are two versions of the txtest and txbench programs: single and double buffered. The only
difference is that in the single-buffered version, the sender first prepares the message,
then sends the message, then enters the barrier to wait for the next round. In the
double-buffered version, the sender instead sends a message, then prepares the next message before entering
the barrier. 

# Speeding up send/receive

In the original design, the sender copies its data into a shared buffer, then enters a barrier. 
The receiver first enters the barrier, then copies the data from the shared buffer into its receive buffer. 

Your job is to speed up the communication, by making changes to the send and receive functions. Do not make
changes to the other functions.

Here are some example runs using the template code on nodes, with 4096 byte messages:

`./txtest -s 4096
Sender: 1 secs total, 2.801516 GB/sec @ size 4096
Receiver: 4 secs total, 0.766784 GB/sec @ size 4096

./txbench -s 4096
Receiver: 4 secs total, 0.917467 GB/sec @ size 4096
Sender: 2 secs total, 1.775764 GB/sec @ size 4096

./txbench_double -s 4096
Sender: 2 secs total, 1.539654 GB/sec @ size 4096
Receiver: 3 secs total, 1.006641 GB/sec @ size 4096`

Depending on your implementation, the sender or the receiver may end up faster than the other, after which point it simply waits for the other to finish before starting the next round. Thus, use the lower of the two throughput numbers as the performance of your design.

**Investigate:** how does buffer size affect performance? What about double-buffering? Try plotting the throughput vs. size using gnuplot. 

## Eliminate the barriers

The barrier is there in send/receive to make sure that the message is ready before the reader starts reading the message. 
However, the barriers are system-call intensive. 

Using a shared (volatile) global variable, can you think of a different way to coordinate the send and receive functions 
and achieve the same behavior? You may use standard volatile C global for the coordination, but it'll be less error prone and more architecture independent to use C11 atomics from the <stdatomic.h> library. Ask ChatGPT how they work. 

If you simply delete the barrier code without something else to replace it, txtest will fail. Make sure your solution runs txtest correctly.

**Investigate:** How does this impact performance for small buffers, like 64 bytes? What about large buffers, like 64kB? Try putting
adding lines for the barrier-free implementation to your plot. 

## Eliminate the shared buffer

The shared buffer isn't strictly necessary. Redesign send() and receive() to copy directly between the sender buffer to the receiver buffer, instead of the intermediary shared buffer. Have send() share a pointer to its buffer with recv(), and then recv() copy the data directly from the send buffer. Make sure the txtest program still works. 

**Investigate:** At this point, the send() is probably *way* faster than recv(). Add a line for this to your gnuplot script, as well as a line for the double-buffered version without a shared buffer.  

Share your best result with the class on Piazza! 

## BONUS CHALLENGE: cooperative copy

After eliminating the shared buffer, and using double-buffering, here is an example output showing a substantial speedup:

`./txbench_double -s 4096
Receiver: 1 secs total, 2.569597 GB/sec @ size 4096
Sender: 0 secs total, 12.198711 GB/sec @ size 4096`

However, note how the Sender is still a lot faster. 

*First:* this is only true for buffers smaller than 32kB. What's causing the sender to run slower for larger buffers than that?

*Second:* for the smaller buffers, change the design so that send() helps out recv() with the job of copying the data from the send buffer to the receive buffer. Did this improve performance?




