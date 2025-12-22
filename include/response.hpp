/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: darismen <darismen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 18:39:34 by darismen          #+#    #+#             */
/*   Updated: 2025/12/22 13:48:12 by darismen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>

#include <cstdlib>
#include <cstdio>
#include <time.h>


#define CLIENT_PREFIX(client) (client->getNickname() + "!" + client->getUsername() + "@" + client->getHostname())

/* Numeric Responses */

#define RPL_WELCOME(source)								"001 " + source + " :Welcome " + source + " to the ft_irc network"
#define RPL_NAMREPLY(source, channel, users)            "353 " + source + " = " + channel + " :" + users
#define RPL_ENDOFNAMES(source, channel)                 "366 " + source + " " + channel + " :End of /NAMES list."

//CMD_JOIN,        // :<nick>!<user>@<host> JOIN #channel

/* Command Responses */
/* 
#define RPL_JOIN(source, channel)                       ":" + source + " JOIN :" + channel
#define RPL_PART(source, channel)                       ":" + source + " PART :" + channel
#define RPL_PING(source, token)                         ":" + source + " PONG :" + token
#define RPL_PRIVMSG(source, target, message)            ":" + source + " PRIVMSG " + target + " :" + message
#define RPL_NOTICE(source, target, message)             ":" + source + " NOTICE " + target + " :" + message
#define RPL_QUIT(source, message)                       ":" + source + " QUIT :Quit: " + message
#define RPL_KICK(source, channel, target, reason)       ":" + source + " KICK " + channel + " " + target + " :" + reason
#define RPL_MODE(source, channel, modes, args)          ":" + source + " MODE " + channel + " " + modes + " " + args
 */
#define RPL_JOIN(client, channel)                       ":" + CLIENT_PREFIX(client) + " JOIN :" + channel
#define RPL_PART(client, channel)                       ":" + CLIENT_PREFIX(client) + " PART :" + channel
#define RPL_PRIVMSG(client, target, message)            ":" + CLIENT_PREFIX(client) + " PRIVMSG " + target + " :" + message
#define RPL_QUIT(client, message)                       ":" + CLIENT_PREFIX(client) + " QUIT : " + message
#define RPL_KICK(client, channel, target, reason)       ":" + CLIENT_PREFIX(client) + " KICK " + channel + " " + target + " :" + reason
#define RPL_MODE(client, channel, modes, args)          ":" + CLIENT_PREFIX(client) + " MODE " + channel + " " + modes + " " + args
#define RPL_PING(client, token)                         ":" + CLIENT_PREFIX(client) + " PONG :" + token
#define RPL_NOTICE(client, target, message)             ":" + CLIENT_PREFIX(client) + " NOTICE " + target + " :" + message
/* Error Responses */

#define ERR_NOTREGISTERED(source)                       "451 " + source + " :You have not registered"
#define ERR_ALREADYREGISTERED(source)                   "462 " + source + " :You may not register"
#define ERR_ALREADYINCHANNEL(source, channel)                    "??? " + source + " " + channel + " :You are already in the channel"
#define ERR_PASSWDMISMATCH(source)                      "464 " + source + " :Password is incorrect"
#define ERR_NONICKNAMEGIVEN(source)                     "431 " + source + " :Nickname not given"
#define ERR_NICKNAMEINUSE(source)                       "433 " + source + " " + source  + " :Nickname is already in use"

#define ERR_UNKNOWNCOMMAND(source, command)             "421 " + source + " " + command + " :Unknown command"
#define ERR_NEEDMOREPARAMS(source, command)             "461 " + source + " " + command + " :Not enough parameters"

#define ERR_TOOMANYCHANNELS(source, channel)            "405 " + source + " " + channel + " :You have joined too many channels"
#define ERR_NOTONCHANNEL(source, channel)               "442 " + source + " " + channel + " :You're not on that channel"
#define ERR_NOSUCHCHANNEL(source, channel)              "403 " + source + " " + channel + " :No such channel"
#define ERR_BADCHANNELKEY(source, channel)              "475 " + source + " " + channel + " :Cannot join channel (+key)"
#define ERR_CHANNELISFULL(source, channel)              "471 " + source + " " + channel + " :Cannot join channel (+limit)"
#define ERR_CANNOTSENDTOCHAN(source, channel)           "404 " + source + " " + channel + " :Cannot send to channel"
#define ERR_CHANOPRIVSNEEDED(source, channel)           "482 " + source + " " + channel + " :You're not channel operator"

#define ERR_NOSUCHNICK(source, nickname)                "401 " + source + " " + nickname + " :No such nick/channel"
#define ERR_USERNOTINCHANNEL(source, nickname, channel) "441 " + source + " " + nickname + " " + channel + " :They aren't on that channel"

/* Log Response */

static inline void log(const std::string& message) 
{
	time_t rawtime = time(NULL);
	struct tm *timeinfo = localtime(&rawtime);
	char buffer[80];

	strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
	std::cout << "\033[0;34m[" << buffer << "]\033[0m " << message << std::endl;
}