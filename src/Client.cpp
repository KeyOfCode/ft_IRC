#include "../include/Client.hpp"
#include "../include/Parser.hpp"
#include "../include/Server.hpp"
#include <unistd.h>     // close(), write()
#include <algorithm>    // std::min
#include <cstring>
#include <ctime>

// =========================== Constructor / Destructor =========================

Client::Client(int socket_fd)
  : socket_fd_(socket_fd),
	pass_ok_(false),
	nick_ok_(false),
	user_ok_(false),
	// operator_(false),
	nickname_(""),
	username_(""),
	hostname_(""),
	realname_(""),
	recv_buffer_(""),
	send_queue_(),
	send_offset_(0),
	channels_(),
	last_activity_(std::time(0)),
	disconnected_(false)
{
}

Client::~Client()
{
	// No intentar doble-close si ya se cerró externamente; cerramos si queda abierto.
	if (socket_fd_ >= 0) {
		close(socket_fd_);
		socket_fd_ = -1;
	}
}
// MARK: Getters / Setters
// =========================== Getters / Setters ================================

int Client::getSocketFd() const { return socket_fd_; }
const std::string& Client::getNickname() const { return nickname_; }
const std::string& Client::getUsername() const { return username_; }
const std::string& Client::getHostname() const { return hostname_; }
const std::string& Client::getRecvBuffer() const { return recv_buffer_; }
bool Client::getPass(){ return pass_ok_; }
time_t Client::getLastActivity() const { return last_activity_; }
const std::set<std::string>& Client::getChannels() const { return channels_; }
bool Client::isDisconnected() const { return disconnected_; }

void Client::setNickname(const std::string& nick) { nickname_ = nick; }
void Client::setUsername(const std::string& user) { username_ = user; }
void Client::setHostname(const std::string& host) { hostname_ = host; }
void Client::setRealname(const std::string& real) { realname_ = real; }

void Client::setPassOK() { pass_ok_ = true; }
void Client::setNickOK() { nick_ok_ = true; }
void Client::setUserOK() { user_ok_ = true; }
bool Client::isFullyRegistered() const { return pass_ok_ && nick_ok_ && user_ok_; }

// =========================== Recv buffer handling ============================

// MARK: appendToRecvBuffer
void Client::appendToRecvBuffer(const std::string& data)
{
	if (recv_buffer_.size() + data.size() > RECV_BUFFER_LIMIT) {
		pushSendQueue("ERROR :Recv buffer limit exceeded\r\n");
		disconnected_ = true;
		return;
	}
	recv_buffer_ += data;
}

// MARK: recvBufferSize
size_t Client::recvBufferSize() const { return recv_buffer_.size(); }

// MARK: popRecv
void Client::popRecv(size_t len)
{
	if (len >= recv_buffer_.size()) recv_buffer_.clear();
	else recv_buffer_.erase(0, len);
}

// =========================== Parsing / processBuffer ========================

// MARK: parseMessage
IRCMessage Client::parseMessage(const std::string& line) {
	IRCMessage msg;
	size_t pos = 0;
	size_t len = line.size();

	// Prefijo opcional
	if (len > 0 && line[0] == ':') {
		size_t sp = line.find(' ');
		if (sp == std::string::npos) {
			msg.command = "";
			return msg;
		}
		msg.prefix = line.substr(1, sp - 1);
		pos = sp + 1;
	}

	// Comando
	while (pos < len && line[pos] == ' ') pos++;
	size_t sp = line.find(' ', pos);
	if (sp == std::string::npos) {
		msg.command = line.substr(pos);
		std::transform(msg.command.begin(), msg.command.end(), msg.command.begin(), ::toupper);
		return msg;
	}
	msg.command = line.substr(pos, sp - pos);
	std::transform(msg.command.begin(), msg.command.end(), msg.command.begin(), ::toupper);
	pos = sp + 1;

	// Params y trailing
	while (pos < len) {
		if (line[pos] == ':') {
			msg.trailing = line.substr(pos + 1);
			break;
		}
		size_t next = line.find(' ', pos);
		if (next == std::string::npos) {
			msg.params.push_back(line.substr(pos));
			std::cout << "Param: " << line.substr(pos) << std::endl;
			break;
		} else {
			std::cout << "Param: " << line.substr(pos, next - pos) << std::endl;
			msg.params.push_back(line.substr(pos, next - pos));
			pos = next + 1;
		}
	}
	return msg;
}

// MARK: processBuffer
void Client::processBuffer(Server *srv,Parser* parser,void (*onMessageParsed)(Server *srv, Parser* parser,Client *, std::string command, std::vector<std::string> params)) {
	const size_t IRC_MAX = IRC_LINE_MAX;
	while (true) {
		size_t pos = recv_buffer_.find("\r\n");
		if (pos == std::string::npos) {
			if (recv_buffer_.size() > IRC_MAX) {
				pushSendQueue("ERROR :Line too long\r\n");
				recv_buffer_.clear();
				disconnected_ = true;
			}
			break;
		}

		size_t lineLenWithCRLF = pos + 2;
		if (lineLenWithCRLF > IRC_MAX) {
			pushSendQueue("ERROR :Line too long\r\n");
			recv_buffer_.erase(0, lineLenWithCRLF);
			continue;
		}

		//mensaje parseado
		std::string line = recv_buffer_.substr(0, pos);
		recv_buffer_.erase(0, lineLenWithCRLF);

		// Validación caracteres control
		bool bad = false;
		for (size_t i = 0; i < line.size(); ++i) {
			unsigned char c = static_cast<unsigned char>(line[i]);
			if (c == '\r' || c == '\n') { bad = true; break; }
			if (c < 32) { bad = true; break; }
		}
		if (bad) {
			pushSendQueue("ERROR :Invalid characters in message\r\n");
			continue;
		}
		
		//aqui seguramente habra que hacer cambios
		// Parse y callback
		//LO QUE LE PASE AL MENSAJE PARSEADO
		/** ! Comments must be in english */
		IRCMessage msg = parseMessage(line);
		if (!msg.trailing.empty())
			msg.params.push_back(msg.trailing);
		std::cout << "msg log: prefix: " << msg.prefix << " command: " << msg.command << " trailing: " << msg.trailing << std::endl;

		if (onMessageParsed && !disconnected_)
			onMessageParsed(srv, parser, this, msg.command, msg.params);

		updateLastActivity();
	}
}

// =========================== Send queue =====================

// MARK: pushSendQueue
void Client::pushSendQueue(const std::string& msg) {
	send_queue_.push_back(msg);
}

// MARK: enqueueMessage
bool Client::enqueueMessage(const std::string& raw) {
	std::string msg = raw;
	if (msg.size() < 2 || msg.substr(msg.size() - 2) != "\r\n")
		msg += "\r\n";

	if (msg.size() > IRC_LINE_MAX) {
		return false;
	}

	pushSendQueue(msg);
	return true;
}


// MARK: peekSend
std::string Client::peekSend(size_t maxlen) const {
	if (send_queue_.empty()) return std::string();

	const std::string& front = send_queue_.front();
	if (send_offset_ >= front.size()) return std::string();

	size_t available = front.size() - send_offset_;
	size_t take = (available < maxlen ? available : maxlen);
	return front.substr(send_offset_, take);
}

// MARK: markSent
void Client::markSent(size_t n) {
	if (send_queue_.empty()) return;
	size_t remaining = n;
	while (remaining > 0 && !send_queue_.empty()) {
		std::string &front = send_queue_.front();
		size_t avail = front.size() - send_offset_;
		if (remaining < avail) {
			send_offset_ += remaining;
			remaining = 0;
		} else {
			remaining -= avail;
			send_queue_.pop_front();
			send_offset_ = 0;
		}
	}
}

// MARK: hasPendingSend
bool Client::hasPendingSend() const {
	return !send_queue_.empty();
}

// MARK: clearSendQueue
void Client::clearSendQueue() {
	send_queue_.clear();
	send_offset_ = 0;
}

// =========================== Canales ===========================

// MARK: joinChannel
void Client::joinChannel(const std::string& name) { channels_.insert(name); }

// MARK: leaveChannel
void Client::leaveChannel(const std::string& name) {
	std::set<std::string>::iterator it = channels_.find(name);
	if (it != channels_.end()) channels_.erase(it);
}
// MARK: updateLastActivity
void Client::updateLastActivity() { last_activity_ = std::time(0); }

// MARK: prefix
std::string Client::prefix() const {
	std::string nick = nickname_.empty() ? "*" : nickname_;
	std::string user = username_.empty() ? "unknown" : username_;
	std::string host = hostname_.empty() ? "localhost" : hostname_;
	return ":" + nick + "!" + user + "@" + host;
}

// =========================== Disconnect =======================

// MARK: handleDisconnect
void Client::handleDisconnect() {
	if (disconnected_) return;
	disconnected_ = true;
	clearSendQueue();
	recv_buffer_.clear();
	// notificar a A
	// cerrar descriptor localmente (A podría ya haberlo cerrado; esto es defensivo)
	if (socket_fd_ >= 0) {
		close(socket_fd_);
		socket_fd_ = -1;
	}
}



