#pragma once
#ifndef TCPSERVER_HPP
# define TCPSERVER_HPP

#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#include <exception>
#include <iostream>
#include <thread>

class TCPServer
{
    private:        
        int socket_fd;
        int port_no;
        struct sockaddr_in serv_addr;
        struct sockaddr_in cli_addr;
        socklen_t clilen;

    public:
        TCPServer(uint16_t port = 3000) : port_no(port)
        {
            socket_fd = socket(AF_INET, SOCK_STREAM, 0);
            if (socket_fd < 0)
            {
                std::cerr << "ERROR opening socket" << std::endl;
                throw std::exception();
            }
            bzero((char*)&serv_addr, sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_addr.s_addr = INADDR_ANY;
            serv_addr.sin_port = htons(port_no);
            if (bind(socket_fd, \
                    (struct sockaddr*)&serv_addr, \
                    sizeof(serv_addr)) < 0)
            {
                close(socket_fd);
                socket_fd = -1;
                std::cerr << "ERROR on binding" << std::endl;
                throw std::exception();
            }
            clilen = sizeof(cli_addr);
        }

        template<typename F, typename ...Args>
        void start(F callable, Args... args)
        {
            int newsocket_fd;

            listen(socket_fd, 5);
            while (true)
            {
                newsocket_fd = accept(socket_fd, 
                            (struct sockaddr *) &cli_addr, 
                            &clilen);
                if (newsocket_fd < 0)
                {
                    std::cerr << "ERROR on accept" << std::endl;
                    continue ;
                }
                std::thread t(callable, newsocket_fd, args...);
                t.detach();
            }
        }

        ~TCPServer()
        {
            close(socket_fd);
            socket_fd = -1;
        }
};

#endif