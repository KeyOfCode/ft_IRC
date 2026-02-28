/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paluja <paluja@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 18:39:34 by darismen          #+#    #+#             */
/*   Updated: 2026/02/26 19:36:21 by paluja           ###   ########.fr       */
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

#define RPL_WELCOME(client)								"001: " + " :Welcome to the ft_irc network " + CLIENT_PREFIX(client)
#define RPL_YOURHOST(server)							"002: " + " :Your host is <servername>"
#define RPL_MYINFO(server)								"004: " + " :<servername> <available user modes> <available channel modes>"
#define RPL_AWAY(clientNick)							"301: " + clientNick + " is away"
#define RPL_USERHOST(client)							"302: " + CLIENT_PREFIX(client)

#define RPL_WHOISUSER(client)							"311: " + CLIENT_PREFIX(client)
#define RPL_WHOISOPERATOR(client)						"313: " + client->getNickname() + " :is an IRC operator"

//CMD_JOIN,        // :<nick>!<user>@<host> JOIN #channel

#define RPL_JOIN(client, channel)						":" + CLIENT_PREFIX(client) + " JOIN :" + channel
#define RPL_INVITING(inviter, target, channel)          "341 " + inviter + " " + target + " " + channel
#define RPL_PART(client, channel)                       ":" + CLIENT_PREFIX(client) + " PART :" + channel
#define RPL_PRIVMSG(client, target, message)            ":" + CLIENT_PREFIX(client) + " PRIVMSG " + target + " :" + message
#define RPL_QUIT(client, message)                       ":" + CLIENT_PREFIX(client) + " QUIT : " + message
#define RPL_KICK(client, channel, target, reason)       ":" + CLIENT_PREFIX(client) + " KICK " + channel + " " + target + " :" + reason
#define RPL_PING(client, token)                         ":" + CLIENT_PREFIX(client) + " PONG :" + token
#define RPL_NOTICE(client, target, message)				":" + CLIENT_PREFIX(client) + " NOTICE " + target + " :" + message
#define RPL_NOTOPIC(channel)            				"331: " + channel + " :No topic is set"
#define RPL_TOPIC(channel, message)          			"332: TOPIC " + channel + " :" + message
#define RPL_MODE(client, channel, modes, param)			":" + CLIENT_PREFIX(client) + " MODE " + channel + " " + modes + " " + param
#define RPL_CHANNELMODEIS(channel, modes)				"324: " + channel + " MODES " + modes

#define RPL_WHOREPLY(channel, client)					"352: " + channel + " " + client
#define RPL_ENDOFWHO(client)							"315: " + CLIENT_PREFIX(client) + " :End of WHO list"
#define RPL_LIST(channel, modes, topic)					"322: " + channel + " Modes:" + modes + " Topic:" + topic
#define RPL_LISTEND										"323: End of LIST"
#define RPL_NAMREPLY(channel, client)					"353: = " + channel + " " + client
#define RPL_ENDOFNAMES(channel)							"366: " + channel + " :End of NAMES list"
#define RPL_SERVLIST									"234: Available services are: JOIN, MODE, KICK, PART, QUIT, PRIVMSG, NOTICE."
#define RPL_SERVLISTEND									"235: End of Service Listing"

/* Error Responses */

#define ERR_NOTREGISTERED(source)						"451: " + source + " :You have not registered"
#define ERR_ALREADYREGISTERED(source)					"462: " + source + " :You may not register"
#define ERR_ALREADYINCHANNEL(source, channel)			"???: " + source + " " + channel + " :You are already in the channel"
#define ERR_PASSWDMISMATCH(source)						"464: " + source + " :Password is incorrect"
#define ERR_NONICKNAMEGIVEN(source)						"431: " + source + " :Nickname not given"
#define ERR_NICKNAMEINUSE(source)                       "433: " + source + " " + source  + " :Nickname is already in use"

#define ERR_UNKNOWNCOMMAND(source, command)             "421: " + source + " " + command + " :Unknown command"
#define ERR_NEEDMOREPARAMS(source, command)             "461: " + source + " " + command + " :Not enough parameters"

#define ERR_TOOMANYCHANNELS(source, channel)            "405: " + source + " " + channel + " :You have joined too many channels"
#define ERR_NOTONCHANNEL(source, channel)               "442: " + source + " " + channel + " :You're not on that channel"
#define ERR_BADCHANNELKEY(source, channel)              "475: " + source + " " + channel + " :Cannot join channel (+key)"
#define ERR_CHANNELISFULL(source, channel)              "471: " + source + " " + channel + " :Cannot join channel (+limit)"
#define ERR_INVITEONLYCHAN(source, channel)				"473: " + source + " " + channel + " :Cannot join channel (+i)"
#define ERR_BADCHANMASK(source, channel)				"476: " + source + " " + channel + " :Bad Channel Mask"
#define ERR_CANNOTSENDTOCHAN(source, channel)           "404: " + source + " " + channel + " :Cannot send to channel"
#define ERR_CHANOPRIVSNEEDED(source, channel)           "482: " + source + " " + channel + " :You're not channel operator"
#define ERR_UMODEUNKNOWNFLAG(source, flag)			"501: " + source + " " + flag + " :Unknown MODE flag"
#define ERR_INVALIDMODEPARAM(source, channel, mode)	"696: " + source + " " + channel + " " + mode + " :Invalid mode parameter"

#define ERR_NOSUCHNICK(nickname)						"401: " + nickname + " :No such nick/channel"
#define ERR_NOSUCHCHANNEL(channel)						"403: " + channel + " :No such channel"
#define ERR_USERNOTINCHANNEL(source, nickname, channel) "441: " + source + " " + nickname + " " + channel + " :They aren't on that channel"

/* Log Response */

static inline void log(const std::string& message) 
{
	time_t rawtime = time(NULL);
	struct tm *timeinfo = localtime(&rawtime);
	char buffer[80];

	strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
	std::cout << "\033[0;34m[" << buffer << "]\033[0m " << message << std::endl;
}