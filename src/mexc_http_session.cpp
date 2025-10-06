/**
MEXC HTTPS Session

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2022 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/

#include "vk/mexc/mexc_http_session.h"
#include "nlohmann/json.hpp"
#include <boost/asio/ssl.hpp>
#include <boost/beast/version.hpp>
#include "date.h"
#include "vk/utils/utils.h"

namespace vk::mexc {
namespace ssl = boost::asio::ssl;
using tcp = net::ip::tcp;

auto API_URI_SPOT = "api.mexc.com";
auto API_URI_FUTURES = "contract.mexc.com";

struct HTTPSession::P {
	net::io_context m_ioc;
	std::string m_apiKey;
	int m_receiveWindow = 25000;
	std::string m_apiSecret;
	std::string m_uri;
	const EVP_MD *m_evp_md;

	P() : m_evp_md(EVP_sha256()) {
	}

	http::response<http::string_body> request(http::request<http::string_body> req);

	static std::string createQueryStr(const std::map<std::string, std::string> &parameters) {
		std::string queryStr;

		for (const auto &[fst, snd]: parameters) {
			queryStr.append(fst);
			queryStr.append("=");
			queryStr.append(snd);
			queryStr.append("&");
		}

		if (!queryStr.empty()) {
			queryStr.pop_back();
		}
		return queryStr;
	}

	void authenticateGet(http::request<http::string_body> &req, const std::map<std::string, std::string> &parameters) const {

		const std::string parameterString = createQueryStr(parameters);
		const auto ts = getMsTimestamp(currentTime()).count();
		const std::string strToSign = m_apiKey + std::to_string(ts) + parameterString;

		unsigned char digest[SHA256_DIGEST_LENGTH];
		unsigned int digestLength = SHA256_DIGEST_LENGTH;

		HMAC(m_evp_md, m_apiSecret.data(), m_apiSecret.size(),
		     reinterpret_cast<const unsigned char *>(strToSign.data()),
		     strToSign.length(), digest, &digestLength);

		req.set("ApiKey", m_apiKey);
		req.set("Content-Type", "application/json");
		req.set("Request-Time", std::to_string(ts));
		req.set("Signature", stringToHex(digest, sizeof(digest)));
	}

	void authenticatePost(http::request<http::string_body> &req, const nlohmann::json &json) const {
		const auto bodyString = json.dump();

		// std::string parameterString;
		//
		// const auto now = time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
		// const auto ts = date::format("%FT%T", date::sys_time{now}).append("Z");
		//
		// parameterString.append(ts);
		// parameterString.append("POST");
		// parameterString.append(req.target());
		// parameterString.append(bodyString);
		//
		// unsigned char digest[SHA256_DIGEST_LENGTH];
		// unsigned int digestLength = SHA256_DIGEST_LENGTH;
		//
		// HMAC(m_evp_md, m_apiSecret.data(), m_apiSecret.size(),
		//      reinterpret_cast<const unsigned char *>(parameterString.data()),
		//      parameterString.length(), digest, &digestLength);
		//
		// const std::string signature = base64_encode(digest, sizeof(digest));
		//
		// req.body() = bodyString;
		// req.prepare_payload();
		//
		// req.set("ApiKey", m_apiKey);
		// req.set("Signature", signature);
		// req.set("Request-Time", ts);
		// req.set(http::field::content_type, "application/json");
	}
};

HTTPSession::HTTPSession(const std::string &apiKey, const std::string &apiSecret, const bool futures) : m_p(
	std::make_unique<P>()) {
	if (futures) {
		m_p->m_uri = API_URI_FUTURES;
	} else {
		m_p->m_uri = API_URI_SPOT;
	}

	m_p->m_apiKey = apiKey;
	m_p->m_apiSecret = apiSecret;
}

HTTPSession::~HTTPSession() = default;

http::response<http::string_body> HTTPSession::methodGet(const std::string &path,
                                                         const std::map<std::string, std::string> &parameters,
                                                         const bool isPublic) const {
	std::string finalPath = path;

	if (const auto queryString = P::createQueryStr(parameters); !queryString.empty()) {
		finalPath.append("?");
		finalPath.append(queryString);
	}

	http::request<http::string_body> req{http::verb::get, finalPath, 11};

	if (!isPublic) {
		m_p->authenticateGet(req,parameters);
	}

	return m_p->request(req);
}

http::response<http::string_body> HTTPSession::P::request(
	http::request<http::string_body> req) {
	req.set(http::field::host, m_uri);
	req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

	ssl::context ctx{ssl::context::sslv23_client};
	ctx.set_default_verify_paths();

	tcp::resolver resolver{m_ioc};
	ssl::stream<tcp::socket> stream{m_ioc, ctx};

	// Set SNI Hostname (many hosts need this to handshake successfully)
	if (!SSL_set_tlsext_host_name(stream.native_handle(), m_uri.c_str())) {
		boost::system::error_code ec{
			static_cast<int>(::ERR_get_error()),
			net::error::get_ssl_category()
		};
		throw boost::system::system_error{ec};
	}

	auto const results = resolver.resolve(m_uri, "443");
	net::connect(stream.next_layer(), results.begin(), results.end());
	stream.handshake(ssl::stream_base::client);

	if (req.method() == http::verb::post) {
		req.set(http::field::content_type, "application/json");
	}

	http::write(stream, req);
	beast::flat_buffer buffer;
	http::response<http::string_body> response;
	http::read(stream, buffer, response);

	boost::system::error_code ec;
	stream.shutdown(ec);
	if (ec == boost::asio::error::eof) {
		// Rationale:
		// http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
		ec.assign(0, ec.category());
	}

	return response;
}
}
