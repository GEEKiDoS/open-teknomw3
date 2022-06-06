#pragma once
#include <loader/component_loader.hpp>

#include "game/demonware/stun_server.hpp"
#include "game/demonware/service_server.hpp"

#define TCP_BLOCKING true
#define UDP_BLOCKING false

namespace demonware
{
	template <typename... Args>
	std::shared_ptr<service_server> register_server(Args ... args);

	std::shared_ptr<stun_server> register_stun_server(const std::string& name);

	int recv_datagam_packet(SOCKET s, char* buf, int len, sockaddr* from, int* fromlen);
	void send_datagram_packet(SOCKET s, const std::string& data, const sockaddr* to, int tolen);

	bool is_blocking_socket(SOCKET s, bool def);
	void remove_blocking_socket(SOCKET s);
	void set_blocking_socket(SOCKET s, bool blocking);

	std::shared_ptr<stun_server> find_stun_server_by_name(const std::string& name);
	std::shared_ptr<stun_server> find_stun_server_by_address(unsigned long address);

	std::shared_ptr<service_server> find_server_by_name(const std::string& name);
	std::shared_ptr<service_server> find_server_by_address(unsigned long address);
	std::shared_ptr<service_server> find_server_by_socket(SOCKET s);
	bool link_socket(SOCKET sock, unsigned long address);
	void unlink_socket(SOCKET sock);

	void set_key(bool encrypt, uint8_t* key);
	uint8_t* get_key(bool encrypt);
}
