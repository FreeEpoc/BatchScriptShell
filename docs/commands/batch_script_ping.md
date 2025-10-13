# Batch Script - PING

---



---

This batch command sends ICMP/IP "echo" packets over the network to the designated address.

## Syntax

```
PING [address]
```

Where address is the IP address or hostname of the destination system.

## Example

```
@echo off 
Ping 127.0.0.1
```

## Output

The above command will send ICMP/IP "echo" packets to the destination address 192.168.0.1. Following is an example of the output.

```
Pinging 127.0.0.1 with 32 bytes of data:
Reply from 127.0.0.1: bytes = 32 time<1ms TTL = 128
Reply from 127.0.0.1: bytes = 32 time<1ms TTL = 128
Reply from 127.0.0.1: bytes = 32 time<1ms TTL = 128
Reply from 127.0.0.1: bytes = 32 time<1ms TTL = 128

Ping statistics for 127.0.0.1:
   Packets: Sent = 4, Received = 4, Lost = 0 (0% loss),
Approximate round trip times in milli-seconds:
   Minimum = 0ms, Maximum = 0ms, Average = 0ms
```

batch\_script\_commands.htm

