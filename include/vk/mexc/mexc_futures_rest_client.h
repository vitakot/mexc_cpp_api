/**
MEXC Futures REST Client

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
SPDX-License-Identifier: MIT
Copyright (c) 2022 Vitezslav Kot <vitezslav.kot@gmail.com>.
*/


#ifndef INCLUDE_VK_MEXC_FUTURES_REST_CLIENT_H
#define INCLUDE_VK_MEXC_FUTURES_REST_CLIENT_H

#include <string>
#include <memory>
#include "mexc_models.h"

namespace vk::mexc::futures {
class RESTClient {
	struct P;
	std::unique_ptr<P> m_p{};

public:
	RESTClient(const std::string &apiKey, const std::string &apiSecret);

	~RESTClient();

	void setCredentials(const std::string &apiKey, const std::string &apiSecret) const;

	/**
	 * Returns server time in ms
	 * @return timestamp in ms
	 * @see https://www.mexc.com/api-docs/futures/market-endpoints#get-the-server-time
	*/
	[[nodiscard]] std::int64_t getServerTime() const;

	/**
	 * Returns contract funding rate
	 * @return FundingRate
	 * @see https://www.mexc.com/api-docs/futures/market-endpoints#get-contract-funding-rate
	*/
	[[nodiscard]] FundingRate getContractFundingRate(const std::string &contract) const;

	/**
	 * Returns funding rates for all contracts
	 * @see https://www.mexc.com/api-docs/futures/market-endpoints#get-contract-funding-rate
	 * @return vector of FundingRate structures
	 */
	[[nodiscard]] std::vector<FundingRate> getContractFundingRates() const;

	/**
	 * Get the user's single currency asset information
	 * @see https://www.mexc.com/api-docs/futures/account-and-trading-endpoints#get-the-users-single-currency-asset-information
	 * @param currency
	 * @return WalletBalance
	 */
	[[nodiscard]] WalletBalance getWalletBalance(const std::string &currency) const;
};
}

#endif //INCLUDE_VK_MEXC_FUTURES_REST_CLIENT_H
