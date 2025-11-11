/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pjimenez <pjimenez@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 10:39:51 by paromero          #+#    #+#             */
/*   Updated: 2025/11/28 01:50:02 by pjimenez         ###   ########.fr       */
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
		if (this->pollfds_.empty())
			continue;
		
		int ret =  poll(&pollfds_[0], pollfds_.size(), -1);
		if (ret == -1) {
			if (errno == EINTR) { //! Interrumped by a signal, try again
				if (signal_)
					break;
				continue;
			}
			else
				throw std::runtime_error("Poll failed");
		}

		std::vector<std::pair<int, short> > ready; //! we keep the fd that has information ready
		ready.reserve(ret);
		for (size_t i = 0; i < pollfds_.size(); i++)
		{
			if (pollfds_[i].revents != 0)
				ready.emplace_back(pollfds_[i].fd, pollfds_[i].revents);
		}
		
		for (size_t i = 0; i < ready.size(); i++) {
			int fd = ready[i].first;
			short event = ready[i].second;
			
			if (event & (POLLERR | POLLHUP | POLLNVAL)) {
				std::string reason = "Socket error";
				std::cout << "close" << std::endl;
				//close_client(fd, reason);
				continue;
			}
			if ((event & POLLIN) && fd == this->serSocket_)
			{
				std::cout << "nuevo cliente" << std::endl;
				//accept_new_clients();
			}
			
			if (event & POLLIN)
			{
				std::cout << "nueva info" << std::endl;
				//handle_read(fd);
			}
			
			if (event & POLLOUT)
			{
				std::cout << "write" << std::endl;
				//handle_write(fd);		
			}
			signal_ = true;
		}
	}
}

/*
todo handle_read - recv en bucle manejar recv==0, EAGAIN, EINTR; volcar bytes a buffer
todo handle_write - test mensaje debug (requiere persona B)
todo close_clients(fd, reason) - cierra socket, borra de pollfds, logs, etc
todo stop() - cierra clientes, socket de server y pollfds
todo accept_new_clients
*/