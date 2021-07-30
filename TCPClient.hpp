#pragma once
#ifndef TCPCLIENT_HPP
# define TCPCLIENT_HPP

# include <netinet/in.h>
# include <string.h>
# include <netdb.h> 
# include <unistd.h>

# include <exception>
# include <iostream>
# include <chrono>

# define HOSTNAME "localhost"

std::string gettimestamp()
{
    using namespace std::chrono;

    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    auto now = system_clock::now();
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    oss << std::put_time(&tm, "[%Y-%m-%d %H:%M:%S") \
        << '.' << std::setfill('0') \
        << std::setw(3) << ms.count() << "]";

    return oss.str();
}

class TCPClient
{
    private:
        int socket_fd;
        int serv_port_no;
        struct sockaddr_in serv_addr;
        struct hostent *server;

    public:
        TCPClient(int i_port):
        serv_port_no(i_port)
        {
            socket_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (socket_fd < 0)
            {
                std::cerr << "ERROR opening socket" << std::endl;
                throw std::exception();
            }
            server = gethostbyname(HOSTNAME);
            if (!server)
            {
                std::cerr << "ERROR no such host" << std::endl;
                throw std::exception();
            }
            bzero((char *) &serv_addr, sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            bcopy((char *)server->h_addr, 
                (char *)&serv_addr.sin_addr.s_addr,
                server->h_length);
            serv_addr.sin_port = htons(serv_port_no);
        }

        template<typename F, typename ...Args>
        void start(F callable, Args ...args)
        {
            while (true)
            {
                if (socket_fd == -1)
                    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
                if (connect(socket_fd, \
                            (struct sockaddr*)&serv_addr, \
                            sizeof(serv_addr)) < 0) 
                {
                    std::cerr << "ERROR connecting" << std::endl;
                    throw std::exception();
                }
                callable(socket_fd, args...);
                close(socket_fd);
                socket_fd = -1;
            }
        }

        ~TCPClient()
        {
            close(socket_fd);
            socket_fd = -1;
        }
};

#endif