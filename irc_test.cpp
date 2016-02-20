#include <iostream>
#include <future>
#include <unistd.h>
#include "irc.hpp"

int main()
{
        irc::Connection connection{"irc.twitch.tv", 6667,
                                   "endofmc", "endofmc", "endofmc",
                                   "oauth:4v8sy2z7v2tfjw8yf77j5t8vk2jqz7"};
    try
    {
        auto _ = std::async([&]{connection.run();});
        _.wait();
    }catch(const irc::Exception& error)
    {
        std::cerr << error.what() << std::endl;
    }
}
