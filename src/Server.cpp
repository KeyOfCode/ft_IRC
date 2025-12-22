/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: darismen <darismen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/10 10:39:51 by paromero          #+#    #+#             */
/*   Updated: 2025/12/19 18:49:03 by darismen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/parser.hpp"

bool Server::signal_ = false;

Server::Server(char **av) : parser_(new Parser(this)) {
	std::string port = av[1];
	this->port_ = atoi(port.c_str());
	this->pswd_ = av[2];
	this->signal_ = false;
}

Server::~Server()
{
	delete parser_;
	// Clean up clients
	for (std::map<int, Client*>::iterator it = clients_.begin(); 
		 it != clients_.end(); ++it)
	{
		delete it->second;
	}
	
	// Clean up channels
	for (std::set<Channel*>::iterator it = channels_.begin(); 
		 it != channels_.end(); ++it)
	{
		delete *it;
	}
}

/* Server::Server(const Server& other) {
	this->port_ = other.port_;
	this->pswd_ = other.pswd_;
	this->serSocket_ = other.serSocket_;
	this->signal_ = other.signal_;
	this->pollfds_ = other.pollfds_;
}

Server& Server::operator=(const Server& other) {
	if (this != &other) {
		this->port_ = other.port_;
		this->pswd_ = other.pswd_;
		this->serSocket_ = other.serSocket_;
		this->signal_ = other.signal_;
		this->pollfds_ = other.pollfds_;
	}
	return *this;
} */

//MARK: Init
void	Server::init() {
	struct sockaddr_in add;
	struct pollfd NewPoll;
	
	add.sin_family = AF_INET; //! address set to ipv4
	add.sin_port = htons(this->port_);
	add.sin_addr.s_addr = INADDR_ANY;

	this->serSocket_ = socket(add.sin_family,SOCK_STREAM, 0); //! create socket
	if (this->serSocket_ == -1)
		throw(std::runtime_error("Failed to create a socket"));
	
	int en = 1;
	if (setsockopt(this->serSocket_, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
		throw(std::runtime_error("Failed to set SO_REUSEADDR in the socket"));
	if (fcntl(this->serSocket_, F_SETFL, O_NONBLOCK) == -1)
		throw(std::runtime_error("Failed to set NONBLOCK in the socket"));
	if (bind(this->serSocket_, (struct sockaddr *)&add, sizeof(add)) == -1)
		throw(std::runtime_error("Failed to bind the socket"));
	if (listen(this->serSocket_, SOMAXCONN) == -1)
		throw(std::runtime_error("Failed to listen"));
	NewPoll.fd = this->serSocket_;
	NewPoll.events = POLLIN; //! setted to reading data
	NewPoll.revents = 0; //! set the returned events to 0
	this->pollfds_.push_back(NewPoll);
}

//MARK: SendResponse
void	Server::sendResponse(const std::string& response, int fd)
{
	Client* client = getclient(fd);
	if (!client) {
		std::cerr << "sendResponse: client fd " << fd << " not found" << std::endl;
		return;
	}
	std::string msg = response;
	// Only add \r\n if not already present
	if (msg.size() < 2 || msg.substr(msg.size() - 2) != "\r\n")
		msg += "\r\n";
	
	ssize_t sent = send(fd, msg.c_str(), msg.length(), 0);
	if (sent == -1) {
		std::cerr << "Send error for fd " << fd << ": " << strerror(errno) << std::endl;
	} else {
		std::cout << "Sent " << sent << " bytes to fd " << fd << ": " << msg;
	}
}
std::string	Server::getpassword() {
	return this->pswd_;
}

void	Server::handle_signal(int sig) {
	(void)sig;
	std::cout << std::endl << "Signal detected, exiting..." << std::endl;
	Server::signal_ = true;
}

int	Server::find_poll_index(int fd) {
	for (size_t i = 0; i < pollfds_.size(); ++i) {
		if (pollfds_[i].fd == fd)
			return static_cast<int>(i);
	}
	return -1;
}

Client*	Server::getclient(int fd) {
	std::map<int, Client *>::iterator it = clients_.find(fd);
	if (it != clients_.end())
		return it->second;
	return NULL;
}

Client*	Server::getclient(std::string name) {
	std::map<int, Client *>::const_iterator it = clients_.begin();
	for (; it != clients_.end(); it++)
		if (it->second->getNickname() == name)
			return it->second;
	return NULL;
}

std::set<Channel *>	Server::get_all_Channels() {return this->channels_;}

Channel*		Server::getChannel(std::string name)
{
	std::set<Channel *>::const_iterator it = this->channels_.begin();
	for (; it != this->channels_.end(); it++)
		if ((*it)->getName() == name)
			return *it;
	std::cout << "Channel does not exist" << std::endl;
	return NULL;
}

void		Server::add_Channel(Channel * new_channel)
{
	if (this->getChannel(new_channel->getName()) == NULL)
		this->channels_.insert(new_channel);
	else
		std::cout << "Channel already exists" << std::endl;
}

/*
! EVENTS
! POLLIN: There is data for reading
! POLLOUT: Data for writing
! POLLERR: Socket error
! POLLHUP: Connection closed
! POLLNVAL: Invalid fd
MARK: run
*/
void	Server::run() {
	while (signal_ == false) {
		if (this->pollfds_.empty())
			continue;
		
		if ((poll(&pollfds_[0], pollfds_.size(), -1) == -1) && signal_ == false)
			throw std::runtime_error("Poll failed");
		for (size_t i = 0; i < pollfds_.size(); i++) {
			if (pollfds_[i].revents & POLLIN) {
				if (pollfds_[i].fd == serSocket_)
					accept_new_clients();
				else
					handle_read(pollfds_[i].fd);
			}
			if (pollfds_[i].revents & POLLOUT)
				handle_write(pollfds_[i].fd);
			if (pollfds_[i].revents & (POLLERR | POLLHUP | POLLNVAL)) 
				close_client(pollfds_[i].fd, "Conection error");
		}
	}
	stop();
}

//MARK: accept_new_clients
void	Server::accept_new_clients() {
	struct sockaddr_in cliadd;
	socklen_t len = sizeof(cliadd);
	int clientsocket_ = accept(serSocket_, (struct sockaddr *)&cliadd, &len);
	if (clientsocket_ == -1) {
		std::cerr << "Accept error: " << strerror(errno) << std::endl;
		return;
	}
	//! ponemos no bloqueante
	if (fcntl(clientsocket_, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "fcntl error: " << strerror(errno) << std::endl;
		close(clientsocket_);
		return;
	}
	Client* newclient = new Client(clientsocket_);
	char	host[NI_MAXHOST];
	int ret = getnameinfo((struct sockaddr *)&cliadd, len, host, NI_MAXHOST, NULL, 0, NI_NUMERICSERV);
	if (ret == 0)
		newclient->setHostname(std::string(host));
	else
		newclient->setHostname("localhost");
	clients_[clientsocket_] = newclient;
	struct pollfd newFD;
	newFD.fd = clientsocket_;
	newFD.events = POLLIN;
	newFD.revents = 0;
	pollfds_.push_back(newFD);
	//DEBUG std::cout << "Client connected: FD=" << clientsocket_ << " Host=" << newclient->getHostname() << std::endl;
}

// /command params = CMD <params>
// normal message == /privmsg <message>

/* IRCMessage parseMessage(std::string line)
{
	IRCMessage msg;
	size_t start = 0;

	line = line.substr(start, line.size() - 2); //delete \r\n
	
	if (line[start] == ':')
	{
		msg.prefix = line.substr(start, line.find(' '));
		start = line.find(' ', start);
	}
	
	msg.command = line.substr(start, line.find(' ', start));
	start = line.find(' ', start);

	while(start != std::string::npos)
	{
		std::string param = line.substr(start, line.find(' ', start));
		msg.params.push_back(param);
		start = line.find(' ', start);
	}
	return msg;	
} */

//MARK: parseMEssage
// IRCMessage parseMessage(std::string line)
// {
// 	IRCMessage msg;
// 	size_t start = 0;

// 	// Remove \r\n
// 	if (line.size() >= 2 && line[line.size() - 2] == '\r')
// 		line = line.substr(0, line.size() - 2);
	
// 	// Parse prefix (if starts with :)
// 	if (!line.empty() && line[0] == ':')
// 	{
// 		size_t space = line.find(' ');
// 		if (space != std::string::npos)
// 		{
// 			msg.prefix = line.substr(1, space - 1);
// 			start = space + 1;
// 		}
// 	}
	
// 	// Handle / prefix for commands
// 	if (start < line.size() && line[start] == '/')
// 		start++;  // Skip the /
		
// 	// Parse command
// 	size_t space = line.find(' ', start);
// 	// line = line.substr(start+1);
// 	if (space != std::string::npos)
// 	{
// 		msg.command = line.substr(start, space - start);
// 		std::transform(msg.command.begin(), msg.command.end(), msg.command.begin(), ::toupper);
// 		start = space + 1;
// 	}
// 	else
// 	{
// 		msg.command = line.substr(start);
// 		std::transform(msg.command.begin(), msg.command.end(), msg.command.begin(), ::toupper);
// 		return msg;  // No params
// 	}
	
// 	// Parse params and trailing
// 	while (start < line.size())
// 	{
// 		if (line[start] == ':')
// 		{
// 			msg.trailing = line.substr(start + 1);
// 			break;
// 		}
		
// 		space = line.find(' ', start);
// 		if (space != std::string::npos)
// 		{
// 			msg.params.push_back(line.substr(start, space - start));
// 			start = space + 1;
// 		}
// 		else
// 		{
// 			msg.params.push_back(line.substr(start));
// 			break;
// 		}
// 	}
	
// 	return msg;	
// }

//MARK: handle_read
void	Server::handle_read(int fd) {
	Client* client = getclient(fd);
	if (!client) {
		close_client(fd, "Client not found");
		return;
	}
	char buffer[1024] = {0};
	ssize_t n = recv(fd, buffer, sizeof(buffer) - 1, 0);
	if (n == 0) {
		std::cout << "Client fd = " << fd << std::endl;
		close_client(fd, "Client disconnected");
		return;
	}
	if (n < 0) {
		//! no data
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return; //! no es un error, solo no hay datos
		std::cerr << "recv error fd " << fd << ": " << strerror(errno) << std::endl;
		close_client(fd, "recv error");
		return;
	}
	std::string data(buffer, n);
	std::cout << "Received " << n << " bytes from fd " << fd << std::endl;
	
	client->appendToRecvBuffer(data);
	//! password management
	if (!client->getPass()) {	
		if (client->getRecvBuffer().find("PASS") != std::string::npos) {
			std::string line = client->getRecvBuffer();
			size_t firstpos = line.find("PASS") + 5; //! We pass the word PASS and the space to focus in the password
			size_t finalpos = line.find('\r', firstpos);
			
			if (finalpos == std::string::npos)
			return;
			finalpos = finalpos - firstpos;
			std::string password = line.substr(firstpos, finalpos);
			if (password != this->pswd_) {
				close_client(fd, "Wrong password");
				return;
			}
			else {
				client->setPassOK();
				client->popRecv(finalpos + 2);
			}
		}
	}
	
	std::cout << "Message from client: " << client->getRecvBuffer() << std::endl;
	std::cout << "returned recv: " << n << std::endl;
	client->processBuffer(this,parser_,invoke);

	


	
	//! hay clientes que pasan de primera info (irssi: CAP LS, NICK, USER, etc) y otros que no pasan nada (netcat, alias nc)
}

//MARK: handle_write
void	Server::handle_write(int fd) {
	Client* client = getclient(fd);
	if (!client) {
		close_client(fd, "Client not found in write");
		return;
	}

	if (!client->hasPendingSend()) {
		//- No data, deactivate POLLOUT
		int idx = find_poll_index(fd);
		if (idx != -1)
			pollfds_[idx].events &= ~POLLOUT;
		return;
	}
	//- Get the data for sending
	std::string data = client->peekSend(4096); //4KB max
	if (data.empty()) {
		int idx = find_poll_index(fd);
		if (idx != -1)
			pollfds_[idx].events &= ~POLLOUT;
		return;
	}
	//- try sending
	ssize_t sent = send(fd, data.c_str(), data.size(), 0);
	
	if (sent > 0) {
		//- success
		client->markSent(sent);
		//- If there is no more data, deactivate POLLOUT
		if (!client->hasPendingSend()) {
			int idx = find_poll_index(fd);
			if (idx != -1)
				pollfds_[idx].events &= ~POLLOUT;
		}
	}

	else if (sent < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) //- socket not ready, try later
			return;
		//- Error, close client - strerror transform errno to readable text
		std::cerr << "send error fd " << fd << ": " << strerror(errno) << std::endl;
		close_client(fd, "Send error");
	}
}

//MARK: close_client
void	Server::close_client(int fd, std::string reason) {
	std::cout << "Closing client " << fd << " because of : " << reason << std::endl;
	
	std::map<int, Client*>::iterator it = clients_.find(fd);
	if (it != clients_.end()) {
		delete it->second;
		clients_.erase(it);
	}
	
	int idx = find_poll_index(fd);
	if (idx != -1) {
		size_t last = pollfds_.size() - 1;
		if (static_cast<size_t>(idx) != last)
			pollfds_[idx] = pollfds_[last];
		pollfds_.pop_back();
	}
}

//MARK: stop
void	Server::stop() {
	std::cout << "\n=== Stopping Server ===" << std::endl;
	
	std::vector<int> fds_to_close;
	for (std::map<int, Client*>::iterator it = clients_.begin(); it != clients_.end(); it++)
		fds_to_close.push_back(it->first);


	for (size_t i = 0; i < fds_to_close.size(); i++) {
		if (fds_to_close[i] != serSocket_) {
			close_client(fds_to_close[i], "Server stopping");
		}
	}

	if (serSocket_ != -1) {
		std::cout << "Closing Server socket " << serSocket_ << std::endl;
		close(serSocket_);
		serSocket_ = -1;
	}
	pollfds_.clear();
	
	std::cout << "=== Server stopped ===" << std::endl;
}

