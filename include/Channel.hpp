/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: darismen <darismen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/09 13:30:09 by darismen          #+#    #+#             */
/*   Updated: 2025/12/17 19:35:47 by darismen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <algorithm>
#include <string>
#include <vector>
#include <map>

#include "response.hpp"

class Server;
class Client;


class Channel
{
	typedef std::vector<Client *>::iterator client_it;
	private:
		std::string				_name;
		std::string				_topic;
		Server*					_srv;
		std::vector<Client *>	_admins;
		std::vector<Client *>	_clients;
		std::map<char, bool>	_modes;

		std::string             _key; // channel key
		size_t                  _limit; // limit of channel members
		bool                    _n; // yes/no external messages

		Channel();
		Channel(const Channel& src);
	public:
		Channel(const std::string &name, const std::string &key, Client* admin, Server* srv);
		~Channel();

		/* Getters */
		std::string					getName() const;
		
		std::vector<Client *>		getAdminList() const;
		
		std::string					getKey() const;
		size_t						getLimit() const;

		size_t                      getSize() const;
		std::vector<std::string>    getNicknames();


		/* Setters */

		void	setKey(std::string key);
		void	setLimit(size_t limit);
		void	setTopic(std::string topic);
		// void                        set_ext_msg(bool flag);

		/* Channel Actions */

		void	broadcast(const std::string& message);
		void	broadcast(const std::string& message, Client* exclude);

		void	add_client(Client*);
		void	remove_client(Client*);
		void	add_admin(Client*);
		void	remove_admin(Client*);
		
		bool	isClientMember(Client*);
		bool	isClientAdmin(Client*);
		bool	hasMode(char);


		void	kick(Client* client, Client* target, const std::string& reason);
};
