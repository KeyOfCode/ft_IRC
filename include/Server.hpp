/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: darismen <darismen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 08:59:25 by paromero          #+#    #+#             */
/*   Updated: 2026/02/10 12:17:57 by darismen         ###   ########.fr       */
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
#include "Client.hpp"
#include "Channel.hpp"
#include "Parser.hpp"

class Channel;

class Client;

class Parser;

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
		int		find_poll_index(int fd);
		void	accept_new_clients();	
		void	handle_read(int fd);	
		void	handle_write(int fd);	
		
		
		public:
		Server(char **av);
		~Server();
		
		Client*			getclient(int fd);
		Client*			getclient(std::string name);
		
		Channel*				getChannel(std::string name);
		std::set<Channel *>		get_all_Channels();
		
		void			add_Channel(Channel * new_channel);
		void			del_Channel(Channel * channel);
		
		static 	void	handle_signal(int sig);							
		void			init();											
		void			run();											
		void			stop();											
		void			close_client(int fd, std::string reason);		
		void			sendResponse(const std::string& response, int fd);  
		
	public:
		std::string		getpassword();
		
};

