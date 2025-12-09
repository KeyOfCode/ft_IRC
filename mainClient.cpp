#include "include/client.hpp"
#include <csignal>
#include <cstdio>
#include <string>
#include <iostream>
int foo = 1;
void handle_sig(int sig)
{
    printf("Señal: %d atrapada!\n", sig);
    foo = 0;
}


void printClient(Client *c)
{
    std::cout << "Client FD=" << c->getSocketFd() << "\n";
    std::cout << " Nick: " << c->getNickname() << "\n";
    std::cout << " User: " << c->getUsername() << "\n";
    std::cout << " Host: " << c->getHostname() << "\n";
    std::cout << " RecvBuffer: " << c->getRecvBuffer() << "\n";
    std::cout << " LastActivity: " << c->getLastActivity() << "\n";
    std::cout << " Channels: ";
    for (std::set<std::string>::const_iterator it = c->getChannels().begin(); it != c->getChannels().end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n";
    std::cout << " Disconnected: " << (c->isDisconnected() ? "yes" : "no") << "\n";
}


void handleCommandFromServer(Client &c, const IRCMessage& msg)
{
    if (msg.command == "PING")
    {
        c.enqueueMessage("PONG " + msg.params[0]);
    }
    else if (msg.command == "NICK")
    {
        c.setNickname(msg.params[0]);
    }
    else if (msg.command == "USER")
    {
        c.setUsername(msg.params[0]);
    }
    else if (msg.command == "JOIN")
    {
        std::string channel = msg.params[0];
        c.joinChannel(channel);

        // Respuestas simuladas del servidor
        c.enqueueMessage(":" + c.prefix() + " JOIN " + channel);
        c.enqueueMessage(":server 353 " + c.getNickname() + " = " + channel + " :@" + c.getNickname());
        c.enqueueMessage(":server 366 " + channel + " :End of /NAMES list.");
        printClient(&c);
    }
    else if (msg.command == "PRIVMSG")
    {
        c.enqueueMessage(":" + c.prefix() + " PRIVMSG " + msg.params[0] + " :" + msg.trailing);
        printClient(&c);
    }
    else if (msg.command == "QUIT")
    {
        c.enqueueMessage("ERROR :Closing Link");
        printClient(&c);
        c.handleDisconnect();
        printClient(&c);
    }
}





int main() {
    signal(SIGINT,handle_sig);
    std::string input;
    Client c(1);
    while (foo) {
        std::getline(std::cin,input);
        
        if (input.empty()) {
            continue;
        }
        
        c.appendToRecvBuffer(input + "\r\n");
        c.processBuffer(handleCommandFromServer);

        while (c.hasPendingSend()) {
            std::string toSend = c.peekSend(512);
            std::cout << "[SEND] " << toSend;
            c.markSent(toSend.size());
        }

    }
    return 0;
}