/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paromero <paromero@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 15:58:30 by darismen          #+#    #+#             */
/*   Updated: 2026/03/02 12:47:16 by paromero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <numeric>
#include <string>
#include <algorithm>
#include <vector>

#include "Response.hpp"

class Server;
class Client;
class Channel;

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

//MARK: Cap
class Cap : public Command
{
	public:
		Cap(Server *srv) : Command(srv) {}
		~Cap() {}
		void execute(Client* client, std::vector<std::string> args);
};

//MARK: Userhost
class Userhost : public Command
{
	public:
		Userhost(Server *srv) : Command(srv) {}
		~Userhost() {}
		void execute(Client* client, std::vector<std::string> args);
};

//MARK: Pass
class Pass : public Command
{
	public:
		Pass(Server *srv) : Command(srv) {}
		~Pass() {}
		void execute(Client* client, std::vector<std::string> args);
};

//MARK: Who
class Who : public Command
{
	public:
		Who(Server *srv) : Command(srv) {}
		~Who() {}
		void execute(Client* client, std::vector<std::string> args);
};

//MARK: List
class List : public Command
{
	public:
		List(Server *srv) : Command(srv) {}
		~List() {}
		void execute(Client* client, std::vector<std::string> args);
};

#pragma region Operator Commands
//Operator Commands

//MARK: Kick
class Kick : public Command
{
	public:
		Kick(Server *srv) :Command(srv) {}
		~Kick() {}

		void execute(Client* client, std::vector<std::string> args);
};

//MARK: Invite
class Invite : public Command
{
	public:
		Invite(Server *srv) :Command(srv) {}
		~Invite() {}

		void execute(Client* client, std::vector<std::string> args);
};

//MARK: Topic
class Topic : public Command
{
	public:
		Topic(Server *srv) :Command(srv) {}
		~Topic() {}

		void execute(Client* client, std::vector<std::string> args);
};

//MARK: Mode
class Mode : public Command
{
	public:
		Mode(Server *srv) :Command(srv) {}
		~Mode() {}

		void execute(Client* client, std::vector<std::string> args);
		void change_sub(Client *client, Channel *channel, std::string mode, std::string arg);
};

#pragma endregion


#pragma region User Commands
//User Commands

// JOIN, PART, NICK, USER, PRIVMSG, NOTICE

//MARK: Join
class Join : public Command
{
	public:
		Join(Server *srv) :Command(srv) {}
		~Join() {}

		void execute(Client* client, std::vector<std::string> args);
};

//MARK: Part
class Part : public Command
{
	public:
		Part(Server *srv) :Command(srv) {}
		~Part() {}

		void execute(Client* client, std::vector<std::string> args);
};

//Nick
//MARK: Nicknames
class Nick : public Command
{
	public:
		Nick(Server *srv) :Command(srv) {}
		~Nick() {}

		void execute(Client* client, std::vector<std::string> args);
};

//MARK: User
class User : public Command
{
	public:
		User(Server *srv) :Command(srv) {}
		~User() {}

		void execute(Client* client, std::vector<std::string> args);
};

//MARK: PrivMsg
class PrivMsg : public Command
{
	public:
		PrivMsg(Server *srv) :Command(srv) {}
		~PrivMsg() {}

		void execute(Client* client, std::vector<std::string> args);
};

//MARK: Notice
class Notice : public Command
{
	public:
		Notice(Server *srv) :Command(srv) {}
		~Notice() {}

		void execute(Client* client, std::vector<std::string> args);
};


#pragma endregion

//MARK: Ping
class Ping : public Command
{
	public:

		Ping(Server* srv) : Command(srv) {}
		~Ping() {}

		void    execute(Client* client, std::vector<std::string> args);
};

//MARK: Pong
class Pong : public Command
{
	public:

		Pong(Server* srv) : Command(srv) {}
		~Pong() {}

		void    execute(Client* client, std::vector<std::string> args);
};