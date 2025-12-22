/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: darismen <darismen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 16:26:28 by darismen          #+#    #+#             */
/*   Updated: 2025/12/17 19:30:41 by darismen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include <vector>
#include <map>

#include "command.hpp"
#include "Server.hpp"

class Server;
class Command;

class Parser
{
	private:
		Server	*_srv;
		std::map<std::string, Command *>	_commands;
		
		std::string     trim(const std::string& str);

	public:
		Parser(Server* srv);
		~Parser();

		void	invoke(Client* client, const std::string& command, const std::vector<std::string>& params);
};