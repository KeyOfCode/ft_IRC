/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: darismen <darismen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 18:32:33 by darismen          #+#    #+#             */
/*   Updated: 2026/01/12 17:30:18 by darismen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/command.hpp"
#include "../include/Channel.hpp"
#include "../include/client.hpp"
#include "../include/Server.hpp"
#include "../include/response.hpp"

void Cap::execute(Client* client, std::vector<std::string> args)
{
	if (args.empty())
		return;
	
	std::string subcommand = args[0];
	std::transform(subcommand.begin(), subcommand.end(), subcommand.begin(), ::toupper);
	
	if (subcommand == "LS")
	{
		// Respond with no capabilities
		_srv->sendResponse(":server CAP * LS :\r\n", client->getSocketFd());
	}
	else if (subcommand == "END")
	{
		// Client finished capability negotiation, don't respond
		// Just acknowledge and continue
	}
	else if (subcommand == "REQ")
	{
		// Client requested capabilities we don't support
		_srv->sendResponse(":server CAP * NAK :" + (args.size() > 1 ? args[1] : "") + "\r\n", client->getSocketFd());
	}
}

//MARK: Pass
void Pass::execute(Client* client, std::vector<std::string> args)
{
	(void)client;
	if (args.size() < 1)
		return;
	// Set client password or validate
	// For now, just accept it
}

//MARK: Kick
// KICK args: *client, <channel> <target> :[<message>]
void Kick::execute(Client* client, std::vector<std::string> args)
{
	if (args.size() < 2)
		return void(_srv->sendResponse(ERR_NEEDMOREPARAMS(client->getNickname(), "KICK"), client->getSocketFd()));

	std::string tgt_channel = args[0];
	std::string target_user = args[1];
	std::string reason = "No reason specified!";
	
	
	if (args.size() >= 3 && (args[2][0] != ':' || args[2].size() > 1))
	{
		reason = "";
		for (size_t i = 2; i < args.size(); i++)
		{
			if (i > 2) reason += " ";
			reason += args[i];
		}
	}
	std::set<std::string> channels = client->getChannels();
	
	if (std::find(channels.begin(), channels.end(), tgt_channel) == channels.end())
		return void(_srv->sendResponse(ERR_NOTONCHANNEL(client->getNickname(), tgt_channel), client->getSocketFd()));
	
	Client *dest = _srv->getclient(target_user);
	if (!dest)
		return void(_srv->sendResponse(ERR_NOSUCHNICK(client->getNickname(), tgt_channel), client->getSocketFd()));

	std::set<std::string> dest_channels = dest->getChannels();
	if (std::find(dest_channels.begin(), dest_channels.end(), tgt_channel) == dest_channels.end())
		return void(_srv->sendResponse(ERR_USERNOTINCHANNEL(client->getNickname(), tgt_channel, target_user), client->getSocketFd()));
	
	Channel *channel = _srv->getChannel(tgt_channel);
	channel->kick(client, dest, reason);
	dest->leaveChannel(tgt_channel);

}
// MARK: Invite
// INVITE args: *client, <channel> <target> 
void	Invite::execute(Client* client, std::vector<std::string> args)
{
	if (args.size() < 2)
		return void(_srv->sendResponse(ERR_NEEDMOREPARAMS(client->getNickname(), "Invite"), client->getSocketFd()));
	
	std::string tgt_channel = args[0];
	std::string target_user = args[1];

	std::set<std::string> channels = client->getChannels();
	if (std::find(channels.begin(), channels.end(), tgt_channel) == channels.end())
		return void(_srv->sendResponse(ERR_NOTONCHANNEL(client->getNickname(), tgt_channel), client->getSocketFd()));
	
	Client *dest = _srv->getclient(target_user);
	if (!dest)
		return void(_srv->sendResponse(ERR_NOSUCHNICK(client->getNickname(), target_user), client->getSocketFd()));

	std::set<std::string> dest_channels = dest->getChannels();
	if (std::find(dest_channels.begin(), dest_channels.end(), tgt_channel) != dest_channels.end())
		return void(_srv->sendResponse(ERR_ALREADYINCHANNEL(target_user, tgt_channel), client->getSocketFd()));
	
	Channel *channel = _srv->getChannel(tgt_channel);
	channel->add_client(dest);
	dest->joinChannel(tgt_channel);
}
// MARK: Topic
// syntax: NOTICE <msgtarget> <message>
void	Topic::execute(Client* client, std::vector<std::string> args)
{
	if (args.size() < 2 || args[0].empty() || args[1].empty())
		return void(_srv->sendResponse(ERR_NEEDMOREPARAMS(client->getNickname(), "Topic"), client->getSocketFd()));
	
	std::string tgt_channel = args[0];
	std::string message = "";
	std::vector<std::string>::iterator it = args.begin() + 1;

	while (it != args.end())
	{
		message.append(*it);
		it++;
		if (it != args.end())
			message.append(" ");
	}

	Channel *channel = _srv->getChannel(tgt_channel);
	if (!channel)
		return void(_srv->sendResponse(ERR_NOSUCHCHANNEL(client->getNickname(), tgt_channel), client->getSocketFd()));
	
	channel->setTopic(message);
	
}

// MARK: Mode
// syntax:  MODE <nickname> <flags> (user)
//          MODE <channel> <flags> [<args>]
void	Mode::execute(Client* client, std::vector<std::string> args)
{
	if (args.size() < 2)
		return void(_srv->sendResponse(ERR_NEEDMOREPARAMS(client->getNickname(), "KICK"), client->getSocketFd()));
	
	std::string target = args[0];
	Channel* channel = _srv->getChannel(target);
	if (!channel)
		return void(_srv->sendResponse(ERR_NOSUCHCHANNEL(client->getNickname(), target), client->getSocketFd()));
	if (!channel->isClientAdmin(client))
		return void(_srv->sendResponse(ERR_CHANOPRIVSNEEDED(client->getNickname(), target), client->getSocketFd()));
		
}

/**
 * MARK: Join
 * @param: JOIN args: *client, <channel>
 */
void	Join::execute(Client* client, std::vector<std::string> args)
{
	if (args.size() < 1)
		return void(_srv->sendResponse(ERR_NEEDMOREPARAMS(client->getNickname(), "Join"), client->getSocketFd()));

	std::string tgt_channel = args[0];

	std::set<std::string> channels = client->getChannels();
	if (std::find(channels.begin(), channels.end(), tgt_channel) != channels.end())
		return void(_srv->sendResponse(ERR_ALREADYINCHANNEL(client->getNickname(), tgt_channel), client->getSocketFd()));
	
	
	Channel *channel = _srv->getChannel(tgt_channel);
	// std::cout << channel << std::endl;
	if (channel == 0)
	{
		channel = new Channel(tgt_channel, "", client, _srv);
		_srv->add_Channel(channel);
	}
	channel->add_client(client);
	client->joinChannel(tgt_channel);
	_srv->sendResponse(RPL_JOIN(client, tgt_channel), client->getSocketFd());
}

//MARK: PART
// PART args: *client, <channel>
void	Part::execute(Client* client, std::vector<std::string> args)
{
	if (args.size() < 1)
		return void(_srv->sendResponse(ERR_NEEDMOREPARAMS(client->getNickname(), "Part"), client->getSocketFd()));

	std::string tgt_channel = args[0];

	std::set<std::string> channels = client->getChannels();
	if (std::find(channels.begin(), channels.end(), tgt_channel) == channels.end())
		return void(_srv->sendResponse(ERR_NOTONCHANNEL(client->getNickname(), tgt_channel), client->getSocketFd()));

	Channel *channel = _srv->getChannel(tgt_channel);
	channel->remove_client(client);
	client->leaveChannel(tgt_channel);
	_srv->sendResponse(RPL_PART(client, tgt_channel), client->getSocketFd());
}

//MARK: NICK
// NICK args: <nickname>
void	Nick::execute(Client* client, std::vector<std::string> args)
{
	if (args.empty() || args[0].empty())
		return void(_srv->sendResponse(ERR_NONICKNAMEGIVEN(client->getNickname()), client->getSocketFd()));
	
	std::string nickname = args[0];
	if (_srv->getclient(nickname))
		return void(_srv->sendResponse(ERR_NICKNAMEINUSE(client->getNickname()), client->getSocketFd()));
	client->setNickname(nickname);
}

//MARK: USER
// USER args: <username> <hostname> <Servername> <realname>
void    User::execute(Client* client, std::vector<std::string> args)
{
	if (client->isFullyRegistered())
		return void(_srv->sendResponse(ERR_ALREADYREGISTERED(client->getNickname()), client->getSocketFd()));

	if (args.size() < 4) 
		return void(_srv->sendResponse(ERR_NEEDMOREPARAMS(client->getNickname(), "User"), client->getSocketFd()));
	
	client->setUsername(args[0]);
	client->setRealname(args[3]);
	client->setUserOK();
}

//MARK: PRIVMSG
// syntax: PRIVMSG <msgtarget> :<message>
void	PrivMsg::execute(Client* client, std::vector<std::string> args)
{
	if (args.size() < 2 || args[0].empty() || args[1].empty())
		return void(_srv->sendResponse(ERR_NEEDMOREPARAMS(client->getNickname(), "PrivMsg"), client->getSocketFd()));
	
	std::string target = args[0];
	std::string message = "";
	std::vector<std::string>::iterator it = args.begin() + 1;

	while (it != args.end())
	{
		message.append(*it);
		it++;
		if (it != args.end())
			message.append(" ");
	}

	if (!message.empty() && message[0] == ':')
		message = message.substr(1);

	Client *dest = _srv->getclient(target);
	if (!dest)
		return void(_srv->sendResponse(ERR_NOSUCHNICK(client->getNickname(), target), client->getSocketFd()));
	
	_srv->sendResponse(RPL_PRIVMSG(client, target, message), dest->getSocketFd());
}

//MARK: Notice
void	Notice::execute(Client* client, std::vector<std::string> args)
{
	if (args.size() < 2 || args[0].empty() || args[1].empty())
		return void(_srv->sendResponse(ERR_NEEDMOREPARAMS(client->getNickname(), "Notice"), client->getSocketFd()));
	
	std::string tgt_channel;
	std::string message = "";
	std::vector<std::string>::iterator it = args.begin();
	if (args[0][0] == '#')
		{tgt_channel = args[0]; it++;}
	else
	{
		std::set<std::string> channels = client->getChannels();
		if (channels.empty())
			return void(_srv->sendResponse(ERR_NOTONCHANNEL(client->getNickname(), ""), client->getSocketFd()));
		tgt_channel =  *channels.rbegin();
	}
	
	while (it != args.end())
	{
		message.append(*it);
		it++;
		if (it != args.end())
			message.append(" ");
	}
	
	Channel *channel = _srv->getChannel(tgt_channel);
	if (!channel)
	return void(_srv->sendResponse(ERR_NOSUCHCHANNEL(client->getNickname(), tgt_channel), client->getSocketFd()));

	channel->broadcast(RPL_NOTICE(client, tgt_channel, message));
}

//MARK: PING
// syntax: PING <server1> [<server2>]
void	Ping::execute(Client* client, std::vector<std::string> args)
{
	if (args.size() < 1)
		return void(_srv->sendResponse(ERR_NEEDMOREPARAMS(client->getNickname(), "Ping"), client->getSocketFd()));
	
	_srv->sendResponse(RPL_PING(client, args[0]), client->getSocketFd());
}

//MARK: PONG
// syntax: PONG <server1> [<server2>]
void	Pong::execute(Client* client, std::vector<std::string> args)
{
	if (args.size() < 1)
		return void(_srv->sendResponse(ERR_NEEDMOREPARAMS(client->getNickname(), "Pong"), client->getSocketFd()));
	
	_srv->sendResponse(RPL_PING(client, args[0]), client->getSocketFd());
}