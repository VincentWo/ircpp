irc: irc_test.o irc.o
	g++ irc_test.o irc.o -std=c++1y -lpthread -lircclient -o irc

irc.o: irc.cpp irc.hpp
	g++ irc.cpp -std=c++1y -c
irc_test.o: irc_test.cpp irc.hpp
	g++ irc_test.cpp -std=c++1y -c

