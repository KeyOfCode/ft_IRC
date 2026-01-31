/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: darismen <darismen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 08:59:25 by paromero          #+#    #+#             */
/*   Updated: 2026/01/31 15:06:26 by darismen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <csignal>
#include <iostream>
#include <string>
#include <vector>
#include <poll.h>
#include <map>
#include <stdlib.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <cstring>
#include <netdb.h>
#include <set>
#include "client.hpp"
#include "Channel.hpp"
#include "parser.hpp"

class Channel;

class Client;

class Parser;

// struct IRCMessage {
//     std::string prefix;
//     std::string command;
//     std::vector<std::string> params;
//     std::string trailing;
// };

class Server
{
	
	private:
		//! start params
		int port_;
		std::string pswd_;
		//! listeing socket
		int serSocket_;
		//! signal bool
		static bool signal_;
		//! poll management
		std::vector<struct pollfd> pollfds_;
		//! Clients
		std::map<int, Client*> clients_;
		std::set<Channel *> channels_;
		Parser*				parser_;
		
		//? methods
		int		find_poll_index(int fd);				//done find index of fd in pollfds_ vector
		void	accept_new_clients();					//todo create new clients and assigns to client y pollfd
		void	handle_read(int fd);					//todo read data of a client
		void	handle_write(int fd);					//todo 
		
		
		public:
		Server(char **av);
		~Server();
		// Server(const Server& other);
		// Server& operator=(const Server& other);
		
		Client*			getclient(int fd);
		Client*			getclient(std::string name);
		
		Channel*				getChannel(std::string name);
		std::set<Channel *>		get_all_Channels();
		
		void			add_Channel(Channel * new_channel);
		
		static 	void	handle_signal(int sig);							//done handle the shutdown from a signal
		void			init();											//done init the viariables
		void			run();											//done main loop
		void			stop();											//done clean de Server
		void			close_client(int fd, std::string reason);		//done close the client
		void			sendResponse(const std::string& response, int fd);  //done send messages.
		
	public:
		std::string		getpassword();
		
};

