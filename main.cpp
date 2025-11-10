/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paromero <paromero@student.42malaga.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 08:58:53 by paromero          #+#    #+#             */
/*   Updated: 2025/11/10 11:00:55 by paromero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/server.hpp"

int main (int ac, char **av) {
    if (ac != 3) {
        std::cerr << "Use: ./ircserv <port> <password>" << std::endl;
        return 1;
    }
    server sv(av);
    sv.init();
    return 0;
}