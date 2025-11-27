/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjimenez <pjimenez@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 10:39:51 by paromero          #+#    #+#             */
/*   Updated: 2025/11/18 15:37:22 by pjimenez         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"

server::server(char **av) {
	std::string port = av[1];
	this->port_ = atoi(port.c_str());
	this->pswd_ = av[2];
	this->signal_ = false;
}

server::~server() {
}

server::server(const server& other) {
	this->port_ = other.port_;
	this->pswd_ = other.pswd_;
	this->serSocket_ = other.serSocket_;
	this->signal_ = other.signal_;
	this->pollfds_ = other.pollfds_;
	this->fd_to_poll_index_ = other.fd_to_poll_index_;
	this->max_clients_ = other.max_clients_;
	this->next_uid_ = other.next_uid_;
}

server& server::operator=(const server& other) {
	if (this != &other) {
        this->port_ = other.port_;
		this->pswd_ = other.pswd_;
		this->serSocket_ = other.serSocket_;
		this->signal_ = other.signal_;
		this->pollfds_ = other.pollfds_;
		this->fd_to_poll_index_ = other.fd_to_poll_index_;
		this->max_clients_ = other.max_clients_;
		this->next_uid_ = other.next_uid_;
	}
	return *this;
}

void	server::init() {
	struct sockaddr_in add;
	struct pollfd NewPoll;
	
	add.sin_family = AF_INET; //! address set to ipv4
	add.sin_port = htons(this->port_);
	add.sin_addr.s_addr = INADDR_ANY;

	this->serSocket_ = socket(add.sin_family,SOCK_STREAM, 0); //! create socket
	if (this->serSocket_ == -1)
		throw(std::runtime_error("Failed to create a socket"));
	
	int en = 1;
	if (setsockopt(this->serSocket_, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
		throw(std::runtime_error("Failed to set SO_REUSEADDR in the socket"));
	if (fcntl(this->serSocket_, F_SETFL, O_NONBLOCK) == -1)
		throw(std::runtime_error("Failed to set NONBLOCK in the socket"));
	if (bind(this->serSocket_, (struct sockaddr *)&add, sizeof(add)) == -1)
		throw(std::runtime_error("Failed to bind the socket"));
	if (listen(this->serSocket_, SOMAXCONN) == -1)
		throw(std::runtime_error("Failed to listen"));
	
	NewPoll.fd = this->serSocket_;
	NewPoll.events = POLLIN; //! setted to reading data
	NewPoll.revents = 0; //! set the returned events to 0
	this->pollfds_.push_back(NewPoll);
}

void	server::run() {
	while (signal_ == false) {
		if ((poll(&pollfds_[0], pollfds_.size(), -1) == -1 ) && signal_ == false)
			throw std::runtime_error("Poll failed");
		
		
	}
}