# Bitcoin Server
Project 2 for Computer Systems, Semester 1, 2017 at the University of Melbourne.
The server is essentially used for checking the validity of given proof-of-works and
can also compute valid proof-of-works. Note that the protocol used by the server
to connect to its clients is SSTP.

Run the server using:
```
prompt: ./server [port_number]
```
where `[port_numer]` is a valid port number.

The server uses the following message protocols:
* `PING`, after which the server replies with a `PONG`
* `ERRO <reason>: BYTE[40]`, which indicates an error and a cause for the error
* `SOLN difficulty:uint32 seed:BYTE[64] solution:uint64`, here the server checks
   if the concatenation of `seed` and the `solution` does in fact produce the
   `target` requirement derived from the `difficulty` value
* `WORK difficulty:uint32 seed:BYTE[64] start:uint64 worker_count:uint8`, here
  `start` in the initial `nonce` value. The server puts this work in the work
  queue. Once a proof-of-work solution is found, it replies with the corresponding
  `SOLN` message
* `ABRT`, on receiving this, the server traverses the work queue and removes all
  the pending work for that client and prematurely kills all active computations
  for that client
