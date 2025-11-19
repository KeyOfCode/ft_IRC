/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: darismen <darismen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 16:52:12 by darismen          #+#    #+#             */
/*   Updated: 2025/11/19 17:26:48 by darismen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/parser.hpp"

Parser::Parser(Server* srv) : _srv(srv)
{
	_commands["KICK"] = new Kick(_srv);
	_commands["INVITE"] = new Invite(_srv);
	_commands["TOPIC"] = new Topic(_srv);
	_commands["MODE"] = new Mode(_srv);
	
	_commands["JOIN"] = new Join(_srv);
	_commands["PART"] = new Part(_srv);
	_commands["NICK"] = new Topic(_srv);
	_commands["USER"] = new Mode(_srv);
	_commands["PRIVMSG"] = new Mode(_srv);

	_commands["PING"] = new Ping(_srv);
    _commands["PONG"] = new Pong(_srv);
}

Parser::~Parser() {}

