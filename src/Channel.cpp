/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paromero <paromero@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 14:05:47 by darismen          #+#    #+#             */
/*   Updated: 2026/03/02 12:45:23 by paromero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Channel.hpp"
#include "../include/Server.hpp"
#include "../include/Client.hpp"

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
		{'o', false}, //Operator
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
std::map<char, bool> &Channel::getAllModes() { return this->_modes;}

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

//MARK: The Clients
void	Channel::add_client(Client* client)
{	
	_clients.push_back(client);
}

void	Channel::remove_client(Client* client) 
{
	if (this->isClientMember(client))
		_clients.erase(std::remove(_clients.begin(), _clients.end(), client), _clients.end());
	if (_clients.size() == 0)
		_srv->del_Channel(this);
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

	log(client + " is not member of " + _name);
	return NULL;
}
//MARK: The Admins
void	Channel::add_admin(Client* client)
{	
	if (this->isClientAdmin(client))
		return void(_srv->sendResponse("Already Admin", client->getSocketFd()));
	_srv->sendResponse("Admin granted", client->getSocketFd());
	_admins.push_back(client);
}

void	Channel::remove_admin(Client* client) 
{
	if (this->isClientAdmin(client))
	{
		if (_admins.size() == 1 && _clients.size() > 1)
		{
			_admins.pop_back();
			_admins.push_back(_clients[1]);
			return ;
		}
		else if (_admins.size() == 1 && _clients.size() == 1)
			{_admins.pop_back(); return ;}
		else if (client != _admins[0] && _admins.size() > 1)
			_admins.erase(std::remove(_admins.begin(), _admins.end(), client), _admins.end());
		log(client->getNickname() + " lost admin priviledges from channel " + _name);
	}
}

bool	Channel::isClientAdmin(Client* client)
{
	if (std::find(this->_admins.begin(), this->_admins.end(), client) != this->_admins.end()) 
		return true;
	return false;
}

//MARK: The invited
void	Channel::add_invited(Client* client)
{	
	// if (this->isClientInvited(client))
		// return void(_srv->sendResponse("Already Invited", client->getSocketFd()));
	_invited.push_back(client);
}

void	Channel::remove_invited(Client* client) 
{
	if (this->isClientInvited(client))
		_invited.erase(std::remove(_invited.begin(), _invited.end(), client), _invited.end());
}

bool	Channel::isClientInvited(Client* client)
{
	if (std::find(this->_invited.begin(), this->_invited.end(), client) != this->_invited.end()) 
		return true;
	return false;
}

void	Channel::kick(Client* client, Client* target, const std::string& reason)
{
	this->broadcast(RPL_KICK(client, this->_name, target->getNickname(), reason));
	this->remove_admin(target);
	this->remove_client(target);
	
	log(client->getNickname() + " kicked " + target->getNickname() + " from channel " + _name);
}


bool	Channel::hasMode(char c)
{
	return (_modes.find(c) != _modes.end());
}