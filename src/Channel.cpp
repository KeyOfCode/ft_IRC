/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: darismen <darismen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 14:05:47 by darismen          #+#    #+#             */
/*   Updated: 2026/01/31 14:11:12 by darismen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Channel.hpp"
#include "../include/Server.hpp"
#include "../include/client.hpp"

Channel::Channel(const std::string &name, const std::string &key, Client* admin, Server *srv) :
	_name(name),
	_topic("*"),
	_srv(srv),
	_key(key)
{
	_clients.push_back(admin);
	_admins.push_back(admin);
	_modes = {
		{'i', false}, //Invite-only
		{'t', false}, //Topic restricted (only ops can change)
		{'k', false}, //Keyword/key required to join
		//{'o', false}, //Operator
		{'l', false}, //Limit (max users)
	};
	_limit = 10;
}

Channel::~Channel() {}

/* Getters */

std::string		Channel::getName() const { return this->_name;}

std::vector<Client *>		Channel::getAdminList() const { return this->_admins;}
std::vector<Client *>		Channel::getClientList() const { return this->_clients;}

std::string		Channel::getKey() const { return this->_key;}
size_t			Channel::getSize() const { return this->_clients.size();}
size_t			Channel::getLimit() const { return this->_limit;}
std::string		Channel::getTopic() const { return this->_topic;}
bool			Channel::getModeValue(char c) const { return this->_modes.at(c);}

std::vector<std::string>    Channel::getNicknames()
{
	std::vector<std::string> nicknames;
	client_it it = this->_clients.begin();
	
	for (; it != this->_clients.end(); it++)
		nicknames.push_back((*it)->getNickname());
		
	return nicknames;
}

/* Setters */

void	Channel::setKey(std::string key) {this->_key = key;}
void	Channel::setLimit(size_t limit) {this->_limit = limit;}
void	Channel::setTopic(std::string topic) {this->_topic = topic;}

void	Channel::setMode(char mode, bool val) {this->_modes[mode] = val;}

/* Channel Actions */
//MARK: broacast
void	Channel::broadcast(const std::string& message)
{
	client_it it = this->_clients.begin();
	for (; it != this->_clients.end(); it++)
	{
		_srv->sendResponse(message, (*it)->getSocketFd());
	}
}
void	Channel::broadcast(const std::string& message, Client* exclude)
{
	client_it it = this->_clients.begin();
	for (; it != this->_clients.end(); it++)
	{
		if ((*it) != exclude)
			_srv->sendResponse(message, (*it)->getSocketFd());
	}
}

//MARK: add_client
void	Channel::add_client(Client* client)
{	
	if (this->isClientMember(client))
		return void(_srv->sendResponse(ERR_ALREADYINCHANNEL(client->getNickname(), this->getName()), client->getSocketFd()));
	if (this->getSize() >= this->getLimit())
		return void(_srv->sendResponse(ERR_CHANNELISFULL(client->getNickname(), this->getName()), client->getSocketFd()));
	_clients.push_back(client);
}

void	Channel::remove_client(Client* client) 
{
	// isClientMember should be handled in the command, not here.
	_clients.erase(std::remove(_clients.begin(), _clients.end(), client), _clients.end());
}

bool	Channel::isClientMember(Client* client)
{
	if (std::find(this->_clients.begin(), this->_clients.end(), client) != this->_clients.end()) 
		return true;
	
	return false;
}

Client*	Channel::getClient(std::string client)
{
	client_it it = this->_clients.begin();
	for (; it != this->_clients.end(); it++)
		if ((*it)->getNickname() == client)
			return *it;

	std::cout << "Client is not member" << std::endl;
	return NULL;
}

void	Channel::add_admin(Client* client)
{	
	if (this->isClientAdmin(client))
		return void(_srv->sendResponse("Already Admin", client->getSocketFd()));
	_srv->sendResponse("Admin granted", client->getSocketFd());
	_admins.push_back(client);
}

void	Channel::remove_admin(Client* client) 
{
	if (client != _admins[0])
		_admins.erase(std::remove(_admins.begin(), _admins.end(), client), _admins.end());
	else
		return void(_srv->sendResponse("Can't kick og_admin", client->getSocketFd()));
}

bool	Channel::isClientAdmin(Client* client)
{
	if (std::find(this->_admins.begin(), this->_admins.end(), client) != this->_admins.end()) 
		return true;
	return false;
}
// client_it it = this->_admins.begin();
// for (; it != this->_admins.end(); it++)
// 	if ((*it) == client)
// 		return true;

void	Channel::kick(Client* client, Client* target, const std::string& reason)
{
	this->broadcast(RPL_KICK(client, this->_name, target->getNickname(), reason));
	this->remove_client(target);
	
	log(client->getNickname() + " kicked " + target->getNickname() + " from channel " + _name);
}


bool	Channel::hasMode(char c)
{
	return (_modes.find(c) != _modes.end());
}