#include "config.hpp"
#include "master_list_announce.hpp"
#include "core/module_c.hpp"
#include "core/logger/logger.hpp"

#include <functional>
#include <stdexcept>

#define BOOST_ASIO_DISABLE_EPOLL
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

REGISTER_IN_APPLICATION(master_list_announce);

master_list_announce::ptr master_list_announce::instance() {
    return application::instance()->get_item<master_list_announce>();
}

master_list_announce::master_list_announce()
:time_last()
{
}

master_list_announce::~master_list_announce() {
}

void master_list_announce::configure_pre() {
    is_active = false;
    is_trace = false;
    interval = 300;
    http_bind_ip = samp::api::instance()->get_server_var_as_string("bind");
    http_host = "server.sa-mp.com";
    http_host_port = 80;
    http_url = "/0.3.0/announce/$(server_port)";
    http_user_agent = "SAMP/0.30";
    http_referer = "http://Bonus";
}

void master_list_announce::configure(buffer::ptr const& buff, def_t const& def) {
    SERIALIZE_ITEM(is_active);
    SERIALIZE_ITEM(is_trace);
    SERIALIZE_ITEM(interval);
    SERIALIZE_ITEM(http_bind_ip);
    SERIALIZE_ITEM(http_host);
    SERIALIZE_ITEM(http_host_port);
    SERIALIZE_ITEM(http_url);
    SERIALIZE_ITEM(http_user_agent);
    SERIALIZE_ITEM(http_referer);
}

void master_list_announce::configure_post() {
}

void master_list_announce::on_timer5000() {
    if (!is_active) {
        return;
    }
    boost::posix_time::ptime time_curr(boost::posix_time::microsec_clock::universal_time());
    int delta = (time_curr - time_last).total_seconds();
    if (interval < delta) {
        time_last = time_curr;
        do_announce();
    }
}

void master_list_announce::do_announce() const {
    messages_item msg_item;

    http_request_t http_request;
    if (!http_bind_ip.empty()) {
        http_request.bind_ip.reset(http_bind_ip);
    }
    http_request.host       = msg_item.get_params().process_all_vars(http_host);
    http_request.host_port  = http_host_port;
    http_request.url        = msg_item.get_params().process_all_vars(http_url);
    http_request.user_agent = msg_item.get_params().process_all_vars(http_user_agent);
    http_request.referer    = msg_item.get_params().process_all_vars(http_referer);
    
    //// Запустить в текужей нитке
    //do_announce_impl(http_request, is_trace);

    // Запустить в новой нитки
    boost::thread(std::tr1::bind(&master_list_announce::do_announce_impl, http_request, is_trace));
}

// Производим HTTP запрос и получаем ответ
void master_list_announce::do_announce_impl(http_request_t const& http_request, bool is_trace) {
    using boost::asio::ip::tcp;
    try {
        if (is_trace) {
            logger::instance()->debug("announce/begin", (boost::format("[%1%]") % boost::this_thread::get_id()).str());
            if (http_request.bind_ip) {
                logger::instance()->debug("announce/bind", (boost::format("%1%") % http_request.bind_ip.get()).str());
            }
        }

        boost::asio::io_service io_service;

        // Get a list of endpoints corresponding to the server name.
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(http_request.host, boost::lexical_cast<std::string>(http_request.host_port));
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
        tcp::resolver::iterator end;

        // Try each endpoint until we successfully establish a connection.
        tcp::socket socket(io_service);
        
        boost::system::error_code error = boost::asio::error::host_not_found;
        while (error && endpoint_iterator != end) {
            socket.close();
            // Биндим локальный сокет
            if (http_request.bind_ip) {
                tcp::endpoint bind_endpoint;
                bind_endpoint.address(boost::asio::ip::address::from_string(http_request.bind_ip.get()));
                socket.open(bind_endpoint.protocol());
                socket.bind(bind_endpoint);
            }
            socket.connect(*endpoint_iterator++, error);
        }
        if (error) {
            throw boost::system::system_error(error);
        }

        std::string local_ip = socket.local_endpoint().address().to_string();
        if (is_trace) {
            logger::instance()->debug("announce/ip", (boost::format("%1%") % local_ip).str());
        }

        // Проверка, тот ли сокет мы забиндили
        if (http_request.bind_ip && http_request.bind_ip.get() != local_ip) {
            logger::instance()->log("announce/warning", (boost::format("bind ip (%1%) not equal real binded (%2%)") % http_request.bind_ip.get() % local_ip).str());
        }

        // Form the request. We specify the "Connection: close" header so that the
        // server will close the socket after transmitting the response. This will
        // allow us to treat all data up until the EOF as the content.
        boost::asio::streambuf request;
        std::ostream request_stream(&request);
        request_stream << "GET " << http_request.url << " HTTP/1.0\r\n";
        request_stream << "Accept: */*\r\n";
        request_stream << "User-Agent: " << http_request.user_agent << "\r\n";
        request_stream << "Referer: " << http_request.referer << "\r\n";
        request_stream << "Host: " << http_request.host << "\r\n";
        request_stream << "Connection: close\r\n\r\n";

        // Send the request.
        boost::asio::write(socket, request);

        // Read the response status line.
        boost::asio::streambuf response;
        boost::asio::read_until(socket, response, "\r\n");

        // Check that response is OK.
        std::istream response_stream(&response);
        std::string http_version;
        unsigned int status_code;
        std::string status_message;

        response_stream>>http_version>>status_code;
        std::getline(response_stream, status_message);

        if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
            throw std::runtime_error("Invalid response");
        }
        if (status_code != 200) {
            throw std::runtime_error((boost::format("Response returned with status code %1%") % status_code).str());
        }

        // Read the response headers, which are terminated by a blank line.
        boost::asio::read_until(socket, response, "\r\n\r\n");

        // Process the response headers.
        std::string header;
        while (std::getline(response_stream, header) && header != "\r") {
            //std::cout << header << "\n";
        }
        //std::cout << "\n";

        // Write whatever content we already have to output.
        if (response.size() > 0) {
        //    std::cout << &response;
        }

        // Read until EOF, writing data to output as we go.
        while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error)) {
            //std::cout << &response;
        }
        if (error != boost::asio::error::eof) {
            throw boost::system::system_error(error);
        }
        if (is_trace) {
            logger::instance()->debug("announce/done", (boost::format("[%1%]") % boost::this_thread::get_id()).str());
        }
    }
    catch (std::exception& e) {
        logger::instance()->log("announce/error", (boost::format("[%1%] %2%") % boost::this_thread::get_id() % e.what()).str());
    }
}
