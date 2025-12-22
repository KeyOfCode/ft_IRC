/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: darismen <darismen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 08:58:53 by paromero          #+#    #+#             */
/*   Updated: 2025/12/12 16:20:08 by darismen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/Server.hpp"

int main (int ac, char **av) {
    if (ac != 3) {
        std::cerr << "Use: ./ircserv <port> <password>" << std::endl;
        return 1;
    }

    if (!((std::atoi(av[1]) > 1024 && std::atoi(av[1]) < 65535))) {
        std::cerr << "Invalid port" << std::endl;
        return 1;
    }
    Server sv(av);
    signal(SIGINT, Server::handle_signal);
    try {
        sv.init();
        sv.run();
    }
    catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
    return 0;
}