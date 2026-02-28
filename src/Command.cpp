/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: darismen <darismen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/08 18:32:33 by darismen          #+#    #+#             */
/*   Updated: 2026/02/28 20:52:11 by darismen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Command.hpp"
#include "../include/Channel.hpp"
#include "../include/Client.hpp"
#include "../include/Server.hpp"
#include "../include/Response.hpp"
#include <algorithm>

//MARK: Cap
void Cap::execute(Client* client, std::vector<std::string> args)
{
	if (args.empty())
		return;
	
	std::string subcommand = args[0];
	std::transform(subcommand.begin(), subcommand.end(), subcommand.begin(), ::toupper);
	
	if (subcommand == "LS")
	{
		// Respond with no capabilities
		_srv->sendResponse(":server CAP * LS :", client->getSocketFd());
	}
	else if (subcommand == "END")
	{
		// Client finished capability negotiation, don't respond
		// Just acknowledge and continue
	}
	else if (subcommand == "REQ")
	{
		// Client requested capabilities we don't support
		_srv->sendResponse(":server CAP * NAK :" + (args.size() > 1 ? args[1] : ""), client->getSocketFd());
	}
}
//MARK: Servlist
void Servlist::execute(Client* client, std::vector<std::string> args)
{
	(void)args;
	_srv->sendResponse(RPL_SERVLIST, client->getSocketFd());
}
//MARK: Userhost
void Userhost::execute(Client* client, std::vector<std::string> args)
{
	if (args.size() == 0)
		return void(_srv->sendResponse(RPL_USERHOST(client), client->getSocketFd()));

	for (size_t i = 0; i < args.size(); i++)
	{
		Client *target = _srv->getclient(args[i]);
		if (target != NULL)
			_srv->sendResponse(RPL_USERHOST(target), client->getSocketFd());
		else
			_srv->sendResponse(ERR_NOSUCHNICK(args[i]), client->getSocketFd());
	}
}

//MARK: Pass
void Pass::execute(Client* client, std::vector<std::string> args)
{
	if (args.size() == 0)
		return void(_srv->sendResponse(ERR_NEEDMOREPARAMS(client->getNickname(), "PASS"), client->getSocketFd()));
	
	std::string key = args[0];

	if (_srv->getpassword() != key)
		_srv->close_client(client->getSocketFd(), "Wrong Password");
	
}
//MARK: Who
void Who::execute(Client* client, std::vector<std::string> args)
{
	if (args.size() < 1)
		return void(_srv->sendResponse(ERR_NEEDMOREPARAMS(client->getNickname(), "WHO"), client->getSocketFd()));
	
	std::string tgt_channel = args[0];
	
	Channel *channel = _srv->getChannel(tgt_channel);
	if (!channel)
		return void(_srv->sendResponse(ERR_NOSUCHCHANNEL(tgt_channel), client->getSocketFd()));
		
	const std::vector<std::string>& nicknames = channel->getNicknames();
	for (std::vector<std::string>::const_iterator it = nicknames.begin(); it != nicknames.end(); it++)
		_srv->sendResponse(RPL_WHOREPLY(tgt_channel, *it), client->getSocketFd());
	_srv->sendResponse(RPL_ENDOFWHO(client), client->getSocketFd());
}

std::string get_active_modes(Channel *channel);
//MARK: List
void List::execute(Client* client, std::vector<std::string> args)
{
	(void)args;
	std::set<Channel*> channels = _srv->get_all_Channels();
	std::set<Channel *>::const_iterator it = channels.begin();
	for (; it != channels.end(); it++)
	{
		_srv->sendResponse(RPL_LIST((*it)->getName(), get_active_modes(*it), (*it)->getTopic()), client->getSocketFd());
	}
	_srv->sendResponse(RPL_LISTEND, client->getSocketFd());
}

//MARK: Kick
// KICK args: *client, <channel> <target> :[<message>]
void Kick::execute(Client* client, std::vector<std::string> args)
{
	if (args.size() < 2)
		return void(_srv->sendResponse(ERR_NEEDMOREPARAMS(client->getNickname(), "KICK"), client->getSocketFd()));

	std::string tgt_channel = args[0];
	std::string target_user = args[1];
	std::string reason = (args.size() > 2) ? args[2] : "No reason specified!";
	
	Channel* channel = _srv->getChannel(tgt_channel);
	if (!channel)
		return void(_srv->sendResponse(ERR_NOSUCHCHANNEL(tgt_channel), client->getSocketFd()));
	if (!channel->isClientAdmin(client))
		return void(_srv->sendResponse(ERR_CHANOPRIVSNEEDED(client->getNickname(), tgt_channel), client->getSocketFd()));
	
	Client* target = _srv->getclient(target_user);
	if (!target)
		return void(_srv->sendResponse(ERR_NOSUCHNICK(target_user), client->getSocketFd()));
	if (!channel->getClient(target_user))
		return void(_srv->sendResponse(ERR_NOTONCHANNEL(client->getNickname(), tgt_channel), client->getSocketFd()));
	if (channel->isClientAdmin(target))
		if (target == channel->getAdminList()[0])
			return void(_srv->sendResponse("Can't kick og_admin", client->getSocketFd()));

	channel->kick(client, target, reason);
	target->leaveChannel(tgt_channel);
}
// MARK: Invite
// INVITE args: *client, <target> <channel> 
void	Invite::execute(Client* client, std::vector<std::string> args)
{
	if (args.size() < 2)
		return void(_srv->sendResponse(ERR_NEEDMOREPARAMS(client->getNickname(), "Invite"), client->getSocketFd()));
	
	std::string target_user = args[0];
	std::string tgt_channel = args[1];
	std::string key = "";
	if (args.size() > 2)
		key = args[2];

	Client *dest = _srv->getclient(target_user);
	if (!dest)
		return void(_srv->sendResponse(ERR_NOSUCHNICK(target_user), client->getSocketFd()));

	Channel *channel = _srv->getChannel(tgt_channel);
	if (!channel)
		return void(_srv->sendResponse(ERR_NOSUCHCHANNEL(tgt_channel), client->getSocketFd()));
		
	if (!channel->getClient(client->getNickname()))
		return void(_srv->sendResponse(ERR_NOTONCHANNEL(client->getNickname(), tgt_channel), client->getSocketFd()));
	if (channel->getClient(target_user))
		return void(_srv->sendResponse(ERR_ALREADYINCHANNEL(target_user, tgt_channel), client->getSocketFd()));
	
	if (channel->getModeValue('i') && !channel->isClientAdmin(client)) 
		return void(_srv->sendResponse(ERR_CHANOPRIVSNEEDED(client->getNickname(), tgt_channel), client->getSocketFd()));
	if (channel->getModeValue('l') && channel->getNicknames().size() >= channel->getLimit())
		return void(_srv->sendResponse(ERR_CHANNELISFULL(client->getNickname(), tgt_channel), client->getSocketFd()));
	if (channel->getModeValue('k') && key == channel->getKey())
		return void(_srv->sendResponse(ERR_BADCHANNELKEY(client->getNickname(), tgt_channel), client->getSocketFd()));

	if (channel->isClientInvited(dest))
		return void(_srv->sendResponse("Already Invited", client->getSocketFd()));
	channel->add_invited(dest);
	_srv->sendResponse(RPL_INVITING(client->getNickname(), target_user, tgt_channel), client->getSocketFd());
	_srv->sendResponse(RPL_INVITING(client->getNickname(), target_user, tgt_channel), dest->getSocketFd());
}

// MARK: Topic
// syntax: Topic <msgtarget> <message>
void	Topic::execute(Client* client, std::vector<std::string> args)
{
	if (args.size() < 1 || args[0].empty())
		return void(_srv->sendResponse(ERR_NEEDMOREPARAMS(client->getNickname(), "Topic"), client->getSocketFd()));
	
	std::string tgt_channel = args[0];
	std::string message = (args.size() > 1) ? args[1] : "";

	Channel *channel = _srv->getChannel(tgt_channel);

	if (!channel)
		return void(_srv->sendResponse(ERR_NOSUCHCHANNEL(tgt_channel), client->getSocketFd()));
	
	if (channel->getModeValue('t') && !channel->isClientAdmin(client))
		return void(_srv->sendResponse(ERR_CHANOPRIVSNEEDED(client->getNickname(), tgt_channel), client->getSocketFd()));

	if (!message.empty())
	{
		channel->setTopic(message);
		channel->broadcast(RPL_TOPIC(tgt_channel, message));
	}
	else if (message == ":")
		channel->broadcast(RPL_NOTOPIC(tgt_channel));
	else
	{
		_srv->sendResponse(RPL_TOPIC(tgt_channel, channel->getTopic()), client->getSocketFd());
	}
}

std::string get_active_modes(Channel *channel)
{
	std::string active = "+";
	std::map<char, bool>::iterator it = channel->getAllModes().begin();
	for (; it != channel->getAllModes().end(); it++)
		if (channel->getModeValue((*it).first))
			active += (*it).first;
	return active;
}

// MARK: Mode
// MODE <channel> <flags> [<args>]
// modes = iklot
// static const std::map<char, int> MODE_PARAMS = {
// 	{'l', 1}, {'k', 1}, {'o', 1}
// };

// static const std::vector<char> PARAMS = {'l', 'k', 'o'};
static const std::string PARAMS = "klo";

void Mode::execute(Client* client, std::vector<std::string> args)
{
	if (args.size() < 1)
		return void(_srv->sendResponse(ERR_NEEDMOREPARAMS(client->getNickname(), "MODE"), client->getSocketFd()));
	
	std::string tgt_channel = args[0];
		
	Channel* channel = _srv->getChannel(tgt_channel);
	if (!channel)
		return void(_srv->sendResponse(ERR_NOSUCHCHANNEL(tgt_channel), client->getSocketFd()));
	if (!channel->isClientAdmin(client))
		return void(_srv->sendResponse(ERR_CHANOPRIVSNEEDED(client->getNickname(), tgt_channel), client->getSocketFd()));
	
	// If only channel specified, return current modes
	if (args.size() == 1)
		return void(_srv->sendResponse(RPL_CHANNELMODEIS(tgt_channel, get_active_modes(channel)), client->getSocketFd()));

	// Process mode changes
	std::string modes = args[1];
	bool adding = (args[1][0] != '-');
	
	size_t arg_idx = 2;
	for (size_t i = 1; i < modes.length(); ++i)
	{
		char mode_char = modes[i];
		if (!channel->hasMode(mode_char))
			_srv->sendResponse(ERR_INVALIDMODEPARAM(client->getNickname(), channel->getName(), std::string(1, mode_char)), client->getSocketFd());
		else
		{
			channel->setMode(mode_char, adding);
			std::string flag = std::string(1, modes[0]) + mode_char;
			if (PARAMS.find(mode_char) != std::string::npos)
			{
				if (adding || mode_char == 'o')
				{
					if (arg_idx < args.size())
						change_sub(client, channel, flag, args[arg_idx++]); 
					else
						_srv->sendResponse(ERR_NEEDMOREPARAMS(client->getNickname(), "MODE"), client->getSocketFd());
				}
				else
					change_sub(client, channel, flag, "");
			}
			else
				channel->broadcast(RPL_MODE(client, args[0], flag, ""));
		}
	}

	/* for (size_t i = 0; i < args[1].length(); ++i)
	{
		char mode_char = args[1][i];
		if (mode_char == '+') adding = true;
		else if (mode_char == '-') adding = false;
		else if (!channel->hasMode(mode_char))
			_srv->sendResponse(ERR_INVALIDMODEPARAM(client->getNickname(), channel->getName(), std::string(1, mode_char)), client->getSocketFd());
		else 
		{
			std::string flag = std::string(1, adding ? '+' : '-') + mode_char;
			if (MODE_PARAMS.count(mode_char))
			{
				if (arg_idx < args.size())
				{
					channel->setMode(mode_char, adding);
					change_sub(client, channel, flag, args[arg_idx++]); 
					// channel->broadcast(RPL_MODE(client, args[0], flag, args[arg_idx++]));
				}
				else
					_srv->sendResponse(ERR_NEEDMOREPARAMS(client->getNickname(), "MODE"), client->getSocketFd());
			}
			else
			{
				channel->setMode(mode_char, adding);
				channel->broadcast(RPL_MODE(client, args[0], flag, ""));
			}
		}
	} */
}

void	Mode::change_sub(Client *client, Channel *channel, std::string mode, std::string arg)
{
	char m = mode[1];
	switch(m)
	{
		case 'l':
			{
				if (mode[0] == '-')
					channel->setLimit(100);
				else
				{
					for (char c : arg)
						if (!std::isdigit(c))
							return void(_srv->sendResponse(ERR_INVALIDMODEPARAM(client->getNickname(), channel->getName(), "l"), client->getSocketFd()));
					int limit = std::atoi(arg.c_str());
					if (limit <= 0)
					{
						_srv->sendResponse(ERR_INVALIDMODEPARAM(client->getNickname(), channel->getName(), "l"), client->getSocketFd());
						return;
					}
					channel->setLimit(limit);
				}
				break;
			}
		case 'k':
			if (mode[0] == '-')
				channel->setKey("");
			else
				channel->setKey(arg);
			break;
		case 'o':
			Client *candidate = channel->getClient(arg);
			if (candidate != NULL)
			{
				if (mode[0] == '+')
					channel->add_admin(candidate);
				else
					channel->remove_admin(candidate);
			}
			else
				{_srv->sendResponse(ERR_NOSUCHNICK(arg), client->getSocketFd()); return;}
			break;
	}
	channel->broadcast(RPL_MODE(client, channel->getName(), mode, arg));
}



/**
 * MARK: Join
 * @param: JOIN args: *client, <channel>
 * @param: JOIN args: *client, <channel> <key>
 */
void	Join::execute(Client* client, std::vector<std::string> args)
{
	if (args.size() < 1)
		return void(_srv->sendResponse(ERR_NEEDMOREPARAMS(client->getNickname(), "Join"), client->getSocketFd()));
	
	std::string tgt_channel = args[0];
	if (args[0][0] != '#' || (args[0].size() == 1 && args[0][0] == '#'))	
		return void(_srv->sendResponse(ERR_BADCHANMASK(client->getNickname(), tgt_channel), client->getSocketFd()));
	
	std::string key = (args.size() > 1 && !args[1].empty()) ? args[1] : "";
	
	Channel *channel = _srv->getChannel(tgt_channel);
	
	if (channel == 0)
	{
		channel = new Channel(tgt_channel, key, client, _srv);
		_srv->add_Channel(channel);
		client->joinChannel(tgt_channel);
		if (key != "")
			channel->setMode('k', true);
	}
	else
	{
		if (channel->getModeValue('i') && !channel->isClientInvited(client))
			return void(_srv->sendResponse(ERR_INVITEONLYCHAN(client->getNickname(), tgt_channel), client->getSocketFd()));
		if (channel->getModeValue('k') && key != channel->getKey())
			return void(_srv->sendResponse(ERR_BADCHANNELKEY(client->getNickname(), tgt_channel), client->getSocketFd()));
		if (channel->getModeValue('l') && channel->getNicknames().size() >= channel->getLimit())
			return void(_srv->sendResponse(ERR_CHANNELISFULL(client->getNickname(), tgt_channel), client->getSocketFd()));
		if (channel->isClientMember(client))
			return void(_srv->sendResponse(ERR_ALREADYINCHANNEL(client->getNickname(), channel->getName()), client->getSocketFd()));
		channel->remove_invited(client);
		channel->add_client(client);
		client->joinChannel(tgt_channel);
	}

	_srv->sendResponse(RPL_JOIN(client, tgt_channel), client->getSocketFd());
	_srv->sendResponse(RPL_TOPIC(tgt_channel, channel->getTopic()), client->getSocketFd());
	
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
	if (!channel)
		return void(_srv->sendResponse(ERR_NOSUCHCHANNEL(tgt_channel), client->getSocketFd()));
	channel->remove_admin(client);
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
	std::string tgt_channel;
	std::string message = (args.size() > 1) ? args[1] : "";

	if (args[0][0] == '#')
	{
		tgt_channel = args[0];
		Channel *dest_channel = _srv->getChannel(tgt_channel);
		if (!dest_channel)
			return void(_srv->sendResponse(ERR_NOSUCHCHANNEL(target), client->getSocketFd()));
		if (!dest_channel->isClientMember(client))
			return void(_srv->sendResponse(ERR_NOTONCHANNEL(client->getNickname(), target), client->getSocketFd()));
		dest_channel->broadcast(RPL_PRIVMSG(client, tgt_channel, message), client);
	}
	else
	{
		Client *dest = _srv->getclient(target);
		if (!dest)
			return void(_srv->sendResponse(ERR_NOSUCHNICK(target), client->getSocketFd()));
		
		_srv->sendResponse(RPL_PRIVMSG(client, target, message), dest->getSocketFd());
	}
}

//MARK: Notice
// args: <param> <:trailing>
void	Notice::execute(Client* client, std::vector<std::string> args)
{
	std::string tgt_channel;
	std::string message;
	try
	{
		if (args.size() < 1 || args[0].empty())
			throw ERR_NEEDMOREPARAMS(client->getNickname(), "Notice");
	
		if (args[0][0] == '#')
		{
			tgt_channel = args[0];
			message = (args.size() > 1) ? args[1] : "";
		}
		else
		{
			std::set<std::string> channels = client->getChannels();
			if (channels.empty())
				throw ERR_NOTONCHANNEL(client->getNickname(), tgt_channel);
			tgt_channel =  *channels.rbegin();
			message = args[0];
		}
		Channel *channel = _srv->getChannel(tgt_channel);
		if (!channel)
			throw ERR_NOSUCHCHANNEL(tgt_channel);

		channel->broadcast(RPL_NOTICE(client, tgt_channel, message));
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
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