> *This project has been created as part of the 42 curriculum by darismen, paromero, pjimenez*

# *FT_IRC*


## Description

This project implements the core functionality of an IRC server, allowing multiple clients to connect, create channels, and communicate in real-time.

## Instructions

```bash
	make
	./ircserv <port> <password>
```

* `IP`: In Linux, use hostname -I
* `port`: The port number on which your IRC server will be listening for incoming IRC connections.
* `password`: The connection password required by any IRC client trying to connect to your server.

### Registration Commands

* `PASS`: Requires password
* `NICK`: Set nickname
* `USER`: Set Username

### Basic Commands

* `PRIVMSG`: Sends a message to recipient
* `NOTICE`: Sends a message to recipient (slightly different behavior)
* `LIST`: Lists all the channels
* `WHO` : Lists all the users in a channel
* `TOPIC`: Show the topic of a channel
* `JOIN`: Join a channel
* `PART`: Leave a channel
* `QUIT`: Leave the server

### Channel Operator Commands

* `KICK`: Eject a client from the channel
* `INVITE`: Invite a client to a channel
* `TOPIC`: Change or view the channel topic
* `MODE`: Change the channel's mode
  * `i`: Set/remove Invite-only channel
  * `t`: Set/remove the restrictions of the TOPIC command to channel operators
  * `k`: Set/remove the channel key (password)
  * `o`: Give/take channel operator privilege
  * `l`: Set/remove the user limit to the channel

## Resources

### 🏷️ RFC documentation

Contains all numeric replies, explains the syntax and behaviour of each command.

[RFC 1459](https://datatracker.ietf.org/doc/html/rfc1459)
[RFC 2810](https://datatracker.ietf.org/doc/html/rfc2810)
[RFC 2811](https://datatracker.ietf.org/doc/html/rfc2811)
[RFC 2812](https://datatracker.ietf.org/doc/html/rfc2812)
[RFC 2813](https://datatracker.ietf.org/doc/html/rfc2813)
[RFC 7194](https://datatracker.ietf.org/doc/html/rfc7194)

