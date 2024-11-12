
The objective of this assignment is to study inter-processor 
communication. A small test program is provided, which 
starts two communicating threads: a sender and a receiver. 

The threads use the functions send() and receive(), to send a buffer of 
data from the sender to the receiver. The receiver optionally checks the
validity of the received data (the txtest program), or simply goes to
receive the next buffer.

The threads operate in lock-step: only after the receiver has received the first buffer
does the sender send the next one. This is ensured using barriers. Leave this in place.

== Your job

In the original design, the sender copies its data into a shared buffer, then enters a barrier. 
The receiver first enters the barrier, then copies the data from the shared buffer into its receive buffer. 

Your job is to speed up the communication. 