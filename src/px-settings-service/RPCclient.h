//
// Created by Reza Alizadeh Majd on 2019-01-24.
//

#ifndef PX_ACCOUNTS_SERVICE_RPCCLIENT_H
#define PX_ACCOUNTS_SERVICE_RPCCLIENT_H

#include <utility>
#include <iostream>
#include <map>
#include <vector>
#include <thread>
#include <chrono>
#include <functional>
using namespace std;

#include <kj/async-io.h>
#include <capnp/rpc-twoparty.h>

/// @brief RPC Client Class
template <typename TBase, typename TClient>
class RPCClient {

public:
    explicit RPCClient(string addr) : rpcPath(std::move(addr)) {
    }
	
	/// @brief Connect and send RPC request in a thread
    void performRequest(std::function<void(kj::AsyncIoContext& ctx, TClient &client)> func,vector<string> &errors) {
        auto thClient = std::thread([&]() {
            try {
                auto ctx = kj::setupAsyncIo();
                auto netAddr = ctx.provider->getNetwork().parseAddress(rpcPath).wait(ctx.waitScope);
                auto stream = netAddr->connect().wait(ctx.waitScope);
                auto rpc = kj::heap<capnp::TwoPartyClient>(*stream);
                TClient client = rpc->bootstrap().template castAs<TBase>();
                func(ctx, client);
            }
            catch(kj::Exception& e)
            {
                errors.push_back(e.getDescription().cStr());
            }
        });
        thClient.join();
    }

protected:
    string rpcPath;
};


#endif //PX_ACCOUNTS_SERVICE_RPCCLIENT_H
