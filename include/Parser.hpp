/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: darismen <darismen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 16:26:28 by darismen          #+#    #+#             */
/*   Updated: 2026/02/10 12:40:11 by darismen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include <vector>
#include <map>

#include "Command.hpp"
#include "Server.hpp"

class Server;
class Command;

class Parser
{
	private:
		Server	*_srv;
		std::map<std::string, Command *>	_commands;
		
	public:
		Parser(Server* srv);
		~Parser();
		const std::map<std::string, Command *>& getCommands() const;

};
void	invoke(Server *srv, Parser *prs, Client* client,
				std::string command, std::vector<std::string> params);