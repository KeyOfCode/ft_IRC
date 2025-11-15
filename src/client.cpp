#include "client.hpp"

Client::Client(std::string nickname_, int socket_fd)
    : socket_fd_(socket_fd),
      registered_(false),
      isOperator_(false),
      loggedin_(false),
      recv_buffer_(""),
      send_buffer_(""),
      nickname_(nickname_),
      last_activity_(time(NULL))
{}

Client::~Client() {}

// Getters
int Client::getSocketFd() const {
    return socket_fd_;
}

const std::string& Client::getRecvBuffer() const {
    return recv_buffer_;
}


const std::string& Client::getSendBuffer() const {
    return send_buffer_;
}

const std::string& Client::getNickname() const {
    return nickname_;
}

bool Client::isRegistered() const {
    return registered_;
}

time_t Client::getLastActivity() const {
    return last_activity_;
}

