Multiproto
==========

Simple proxy which will proxy your traffic to the correct host and port based on the protocol you're using. The sample.config listens on port 23456, when you send http traffic the actual traffic will be proxied over to 127.0.0.1:80. In case you're using an irc client it'll be 127.0.0.1:6667.
Of course this is very modular and of course you can also just proxy it out to an entirely different host. You could even write a module for protocol x. There is however one limitation, this can only work for client-first protocols, like HTTP and IRC. A protocol which is server-first won't work, ssh is an example of this.

If you want to add a new protocol simply look at one of the modules in modules/src and create your own c file with the 'int matchData(unsigned char* data, size_t length)' function. Return non-zero if it matches your protocol, return 0 otherwise.
