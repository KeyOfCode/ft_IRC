/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: darismen <darismen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 15:58:30 by darismen          #+#    #+#             */
/*   Updated: 2025/12/19 17:44:19 by darismen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <numeric>
#include <string>
#include <algorithm>

#include "Server.hpp"
#include "client.hpp"
#include "Channel.hpp"
#include "response.hpp"

class Server;

class Command
{
	protected:
		Server	*_srv;
		bool	_auth;
	
	public:
		explicit Command(Server *srv, bool auth = true) : _srv(srv), _auth(auth) {}
		virtual ~Command() {}

		bool auth_required() const { return _auth; }

		virtual void execute(Client* client, std::vector<std::string> args) = 0;
};

class Cap : public Command
{
	public:
		Cap(Server *srv) : Command(srv) {}
		~Cap() {}
		void execute(Client* client, std::vector<std::string> args);
};

class Pass : public Command
{
	public:
		Pass(Server *srv) : Command(srv) {}
		~Pass() {}
		void execute(Client* client, std::vector<std::string> args);
};

#pragma region Operator Commands
//Operator Commands

class Kick : public Command
{
	public:
		Kick(Server *srv) :Command(srv) {}
		~Kick() {}

		void execute(Client* client, std::vector<std::string> args);
};

class Invite : public Command
{
	public:
		Invite(Server *srv) :Command(srv) {}
		~Invite() {}

		void execute(Client* client, std::vector<std::string> args);
};

class Topic : public Command
{
	public:
		Topic(Server *srv) :Command(srv) {}
		~Topic() {}

		void execute(Client* client, std::vector<std::string> args);
};

class Mode : public Command
{
	public:
		Mode(Server *srv) :Command(srv) {}
		~Mode() {}

		void execute(Client* client, std::vector<std::string> args);
};

#pragma endregion


#pragma region User Commands
//User Commands

// JOIN, PART, NICK, USER, PRIVMSG, NOTICE

class Join : public Command
{
	public:
		Join(Server *srv) :Command(srv) {}
		~Join() {}

		void execute(Client* client, std::vector<std::string> args);
};

class Part : public Command
{
	public:
		Part(Server *srv) :Command(srv) {}
		~Part() {}

		void execute(Client* client, std::vector<std::string> args);
};

//Nicknames
class Nick : public Command
{
	public:
		Nick(Server *srv) :Command(srv) {}
		~Nick() {}

		void execute(Client* client, std::vector<std::string> args);
};

class User : public Command
{
	public:
		User(Server *srv) :Command(srv) {}
		~User() {}

		void execute(Client* client, std::vector<std::string> args);
};

class PrivMsg : public Command
{
	public:
		PrivMsg(Server *srv) :Command(srv) {}
		~PrivMsg() {}

		void execute(Client* client, std::vector<std::string> args);
};

class Notice : public Command
{
	public:
		Notice(Server *srv) :Command(srv) {}
		~Notice() {}

		void execute(Client* client, std::vector<std::string> args);
};


#pragma endregion

class Ping : public Command
{
	public:

		Ping(Server* srv) : Command(srv) {}
		~Ping() {}

		void    execute(Client* client, std::vector<std::string> args);
};

class Pong : public Command
{
	public:

		Pong(Server* srv) : Command(srv) {}
		~Pong() {}

		void    execute(Client* client, std::vector<std::string> args);
};