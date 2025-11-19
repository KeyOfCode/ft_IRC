/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: darismen <darismen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/19 16:26:28 by darismen          #+#    #+#             */
/*   Updated: 2025/11/19 17:26:28 by darismen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <sstream>
#include <string>

#include <vector>
#include <map>

#include "command.hpp"

class Parser
{
	private:
		Server	*_srv;
		std::map<std::string, Command *>	_commands;
		
		std::string     trim(const std::string& str);

	public:
		Parser(Server* srv);
		~Parser();

		void    invoke(Client* client, const std::string& message);
};