/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: darismen <darismen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 16:26:28 by darismen          #+#    #+#             */
/*   Updated: 2026/01/12 17:39:46 by darismen         ###   ########.fr       */
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
		const std::map<std::string, Command *>& getCommands() const;

};
void	invoke(Server *srv, Parser *prs, Client* client,
				std::string command, std::vector<std::string> params);