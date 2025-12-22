/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: darismen <darismen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 16:52:12 by darismen          #+#    #+#             */
/*   Updated: 2025/12/19 18:02:49 by darismen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/parser.hpp"

Parser::Parser(Server* srv) : _srv(srv)
{
	_commands["KICK"] = new Kick(_srv);
	_commands["INVITE"] = new Invite(_srv);
	_commands["TOPIC"] = new Topic(_srv);
	// _commands["MODE"] = new Mode(_srv);
	
	_commands["CAP"] = new Cap(_srv);
	_commands["PASS"] = new Pass(_srv);
	
	_commands["JOIN"] = new Join(_srv);
	_commands["PART"] = new Part(_srv);
	_commands["NICK"] = new Nick(_srv);
	_commands["USER"] = new User(_srv);
	_commands["PRIVMSG"] = new PrivMsg(_srv);
	_commands["NOTICE"] = new PrivMsg(_srv);

	_commands["PING"] = new Ping(_srv);
	_commands["PONG"] = new Pong(_srv);
}

Parser::~Parser()
{
	for (std::map<std::string, Command*>::iterator it = _commands.begin(); it != _commands.end(); ++it)
		delete it->second;
	_commands.clear();
}

/* std::string     Parser::trim(const std::string& str)
{
	
} */

//:Nickname!Username@Servername Cualquier cosa
//:Nickname!Username@Servername CMD <params>

void	Parser::invoke(Client* client, const std::string& command, const std::vector<std::string>& params)
{
		Command *cmd = _commands.at(command);
		if (cmd == NULL)
			return void(_srv->sendResponse("Unknown Command", client->getSocketFd()));

		cmd->execute(client, params);
}