#include "TCPServer.hpp"

#include <mutex>
#include <fstream>

void routine(int newsocket, std::mutex *w_m)
{
    int n;
    char buffer[256];
    static std::ofstream f("log.txt");

    n = read(newsocket, buffer, 255);
    if (n < 0)
    {
        std::cerr << "ERROR on reading" << std::endl;
        throw std::exception();
    }
    f << buffer << std::endl;
    bzero(buffer, 256);
    {
        std::lock_guard<std::mutex> lock(*w_m);
        n = write(newsocket, "Got", 3);
    }
    if (n < 0)
    {
        std::cerr << "ERROR on writing" << std::endl;
        throw std::exception();
    }
    close(newsocket);
    newsocket = -1;
}

int main(int ac, char **av)
{
    if (ac != 2)
    {
        std::cerr << "usage: " << av[0] << " port\n";
        exit(1);
    }
    std::unique_ptr<std::mutex> write_safety_mutex = std::make_unique<std::mutex>();
    TCPServer s(atoi(av[1]));
    s.start(routine, write_safety_mutex.get());
}