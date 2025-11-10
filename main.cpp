/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paromero <paromero@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 08:58:53 by paromero          #+#    #+#             */
/*   Updated: 2025/11/10 12:12:04 by paromero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/server.hpp"

//TODO Comprobar si el puerto es valido o no

int main (int ac, char **av) {
    if (ac != 3) {
        std::cerr << "Use: ./ircserv <port> <password>" << std::endl;
        return 1;
    }
    server sv(av);
    try {
        sv.init();
        std::cout << "funciona" << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
    return 0;
}