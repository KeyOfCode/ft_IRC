/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: darismen <darismen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 15:58:30 by darismen          #+#    #+#             */
/*   Updated: 2025/11/19 17:26:28 by darismen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <numeric>
#include <string>

#include "server.hpp"
#include "client.hpp"

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

#pragma region Operator Commands
//Operator Commands

class Kick : public Command
{
	public:
		Kick(Server *srv);
		~Kick();

		void execute(Client* client, std::vector<std::string> args);
};

class Invite : public Command
{
	public:
		Invite(Server *srv);
		~Invite();

		void execute(Client* client, std::vector<std::string> args);
};

class Topic : public Command
{
	public:
		Topic(Server *srv);
		~Topic();

		void execute(Client* client, std::vector<std::string> args);
};

class Mode : public Command
{
	public:
		Mode(Server *srv);
		~Mode();

		void execute(Client* client, std::vector<std::string> args);
};

#pragma endregion


#pragma region User Commands
//User Commands

// JOIN, PART, NICK, USER, PRIVMSG

class Join : public Command
{
	public:
		Join(Server *srv);
		~Join();

		void execute(Client* client, std::vector<std::string> args);
};

class Part : public Command
{
	public:
		Part(Server *srv);
		~Part();

		void execute(Client* client, std::vector<std::string> args);
};

//Nicknames
class Nick : public Command
{
	public:
		Nick(Server *srv);
		~Nick();

		void execute(Client* client, std::vector<std::string> args);
};

class User : public Command
{
	public:
		User(Server *srv);
		~User();

		void execute(Client* client, std::vector<std::string> args);
};

class PrivMsg : public Command
{
	public:
		PrivMsg(Server *srv);
		~PrivMsg();

		void execute(Client* client, std::vector<std::string> args);
};

#pragma endregion

class Ping : public Command
{
    public:

        Ping(Server* srv);
        ~Ping();

        void    execute(Client* client, std::vector<std::string> args);
};

class Pong : public Command
{
    public:

        Pong(Server* srv);
        ~Pong();

        void    execute(Client* client, std::vector<std::string> args);
};