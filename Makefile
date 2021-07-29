all:
	clang++ server.cpp -o server -pthread
	clang++ client.cpp -o client -pthread

fclean:
	rm -rf server
	rm -rf client

re: fclean all
