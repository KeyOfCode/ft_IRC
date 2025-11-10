/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paromero <paromero@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 08:59:25 by paromero          #+#    #+#             */
/*   Updated: 2025/11/10 11:08:44 by paromero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <poll.h>
#include <map>
#include <stdlib.h>

class server
{
	private:
		//! start params
		int port_;
		std::string pswd_;
		//! listeing socket
		int listen_fd_;
		//! signal bool
		bool signal_;
		//! poll management
		std::vector<struct pollfd> pollfds_;
		std::map<int, size_t> fd_to_poll_index_; //! get the direct index to the poll, faster than going 1 by 1 in pollfds
		//! Clients
		//* std::map<int, std::unique_ptr<Client>> clients_;
		int max_clients_;
		//! DEBUGGING
		int next_uid_; //* Variable que se incrementa cada vez que se conecta un cliente
		
		//? methods
		void	accept_new_clients();					//todo create new clients and assigns to client y pollfd
		void	handle_poll_events();					//todo process polls
		void	handle_read(int fd); 					//todo read data of a client
		void	handle_write(int fd);					//todo send data from a cliente queque
		void	add_fd_toPoll(int fd, short events);	//todo insert a poll to the struct and update poll_index
		void	remove_fd_fromPoll(int fd);				//todo search the fd in poll_index and deletes from index and pollfd
		void	handle_signal();						//todo handle the shutdown from a signal
		void	set_nonblocking(int fd);				//todo set the socket to nonblocking
		void	check_timeouts();						//todo checks every certain time for clients without activity and disconnect them
		
		
	public:
		server(char **av);
		~server();
		server(const server& other);
		server& operator=(const server& other);
		
		void	init();											//todo init the viariables
		void	run();											//todo main loop
		void	stop();											//todo clean de server
		void	close_client(int fd, std::string &reason);		//todo close the client
		
};

