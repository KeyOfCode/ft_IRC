/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paromero <paromero@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 10:39:51 by paromero          #+#    #+#             */
/*   Updated: 2025/11/10 11:08:35 by paromero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"

server::server(char **av) {
	std::string port = av[1];
	this->port_ = atoi(port.c_str());
	this->pswd_ = av[2];
	this->signal_ = true;
}

server::~server() {
}

server::server(const server& other) {
	this->port_ = other.port_;
	this->pswd_ = other.pswd_;
	this->listen_fd_ = other.listen_fd_;
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
		this->listen_fd_ = other.listen_fd_;
		this->signal_ = other.signal_;
		this->pollfds_ = other.pollfds_;
		this->fd_to_poll_index_ = other.fd_to_poll_index_;
		this->max_clients_ = other.max_clients_;
		this->next_uid_ = other.next_uid_;
	}
	return *this;
}

void	server::init() {
	std::cout << "port: " << this->port_ << std::endl;
	std::cout << "pswd: " << this->pswd_ << std::endl;
}