#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "server.hpp"

class Client
{
    private:
        int				socket_fd_;
        bool			registered_;
        bool            isOperator_;
        bool            loggedin_;
        std::string		recv_buffer_;
        std::string		send_buffer_;
        std::string		nickname_;
        time_t			last_activity_;
        
    public:
        Client(std::string nickname_, int socket_fd);
        ~Client();
        
        //getters ---------------------------->
        int				getSocketFd() const;
        const std::string&	getRecvBuffer() const;
        const std::string&	getSendBuffer() const;
        const std::string&	getNickname() const;
        bool			isRegistered() const;
        time_t			getLastActivity() const;
        
        void			appendToRecvBuffer(const std::string& data);
        void			appendToSendBuffer(const std::string& data);
        void			setNickname(const std::string& nickname);
        void			markAsRegistered();
        void			updateLastActivity();
};


#endif

