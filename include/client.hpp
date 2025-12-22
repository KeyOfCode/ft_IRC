#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <set>
#include <deque>
#include <vector>
#include <ctime>

// Forward declarations
class Parser;
class Server;

struct IRCMessage {
    std::string prefix;
    std::string command;
    std::vector<std::string> params;
    std::string trailing;
};

class Client {
public:
    // Constructor / destructor
    Client(int socket_fd);
    ~Client();

    // Getters
    int getSocketFd() const;
    const std::string& getNickname() const;
    const std::string& getUsername() const;
    const std::string& getHostname() const;
    const std::string& getRecvBuffer() const;
    bool getPass();
    time_t getLastActivity() const;
    const std::set<std::string>& getChannels() const;
    bool isDisconnected() const;

    // Setters
    void setNickname(const std::string& nick);
    void setUsername(const std::string& user);
    void setHostname(const std::string& host);
    void setRealname(const std::string& real);

    void setPassOK();
    void setNickOK();
    void setUserOK();
    bool isFullyRegistered() const;

    // Recive el buffer 
    void appendToRecvBuffer(const std::string& data);

    // procesa el Buffer y parsea líneas completas, llamando a onMessageParsed
    void processBuffer(Server *srv ,Parser* parser,void (*onMessageParsed)(Server *srv,Parser *parser,Client *, std::string command, std::vector<std::string> params));
    //Mensaje recibido por el servidor.
    bool enqueueMessage(const std::string& raw);

    // Devuelve string con hasta maxlen bytes que deberían enviarse ahora (sin eliminar).
    std::string peekSend(size_t maxlen) const;

    // Indica que se han enviado n bytes; avanza la cola internamente.
    void markSent(size_t n);

    // Si hay datos pendientes para enviar
    bool hasPendingSend() const;

    // Limpia la cola de envío
    void clearSendQueue();

    // Total length of recv buffer (útil para límites)
    size_t recvBufferSize() const;

    // Pop n bytes from recv buffer (uso interno tras procesar líneas)
    void popRecv(size_t len);

    // Canales
    void joinChannel(const std::string& name);
    void leaveChannel(const std::string& name);

    void updateLastActivity();

    // Crea el prefijo del cliente: nickname!username@hostname
    std::string prefix() const;

    // Manejo de desconexión
    void handleDisconnect(); // cierra y notifica

private:
    int socket_fd_;

    // validadores
    bool pass_ok_;
    bool nick_ok_;
    bool user_ok_;
    // bool operator_;

    // identificadores
    std::string nickname_;
    std::string username_;
    std::string hostname_;
    std::string realname_;

    // Buffers
    std::string recv_buffer_;

    // Cola ppara envío
    std::deque<std::string> send_queue_;
    size_t send_offset_; // cuantos bytes ya se han enviado del front()

    std::set<std::string> channels_;

    time_t last_activity_;

    // flag de desconexión
    bool disconnected_;


    // Limits
    static const size_t IRC_LINE_MAX = 512;
    static const size_t RECV_BUFFER_LIMIT = 4096;

   
    static IRCMessage parseMessage(const std::string& line);

    
    void pushSendQueue(const std::string& msg);
};

#endif // CLIENT_HPP
