#include "cpp-httplib/httplib.h"
#include "common/common.hpp"

int webserver(std::string filepath, std::string &data, unsigned int port, std::string url_path)
{
    httplib::Server svr;

    //build http_200_response
    std::string ext, content_disposition = "attachment", mimetype = "application/octet-stream";
    ext = getFileExt(getFilename(filepath));

    std::vector<std::pair<std::vector<std::string>, std::string>> known_types = {
        {{"jpg", "jpeg"}, "image/jpeg"},
        {{"png"}, "image/png"},
        {{"pdf"}, "application/pdf"},
        {{"mp4"}, "video/mp4"},
        {{"txt"}, "text/plain"}
    };

    for (int t = 0; t < known_types.size(); t++)
    {
        for (int e = 0; e < known_types[t].first.size(); e++)
        {
            if (ext == known_types[t].first[e]) mimetype = known_types[t].second;
        }
    }

    // Simple HTTP response
    std::string body_404 =
        "<!DOCTYPE html><html><head><title>404</title></head>"
        "<body><h1>404 Not Found</h1>\n<p>check your url path or scan the QR code</p></body></html>";

    svr.Get(url_path, [mimetype, &data, filepath](const httplib::Request &, httplib::Response &res) {
        res.set_content(data, mimetype);
        res.set_header("Content-Disposition", "attachment; filename=\""+getFilename(filepath)+"\"");
    });

    // 404 handler
    svr.set_error_handler([&body_404](const httplib::Request& req, httplib::Response& res) {
        res.status = 404;
        res.set_content(body_404, "text/html");
    });

    svr.listen("0.0.0.0", port);

    return 0;
}