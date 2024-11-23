#include <iostream>
#include <fstream>
#include <thread>
#include <filesystem>
#include "cpp-httplib/httplib.h"
#include "common/common.hpp"
#include "settings/settings.hpp"

extern AXONSETTINGSCONF SETTINGS;

void save_file(std::string filename, std::string data, unsigned __int64 size)
{
    char allow;
    std::cout << "Recieve " << filename << "? (y/n)\n";
    std::cin >> allow;
    if (allow != 'y')
        return;

    //std::cout << "SAVE TO: " << SETTINGS.save_to + filename << '\n';

    std::filesystem::create_directory(SETTINGS.save_to);
    std::ofstream ofs(SETTINGS.save_to + filename, std::ios::binary);
    if (ofs) {
        ofs.write(data.c_str(), size);
        ofs.close();
        std::cout << "File saved: " << filename << std::endl;
    } else {
        std::cerr << "Failed to save file: " << filename << std::endl;
    }
}

int webserver(std::string filepath, std::string &data, unsigned int port, std::string url_path_send, std::string url_path_recv)
{
    httplib::Server svr;

    std::string body_404 =
        "<!DOCTYPE html><html><head><title>404</title></head>"
        "<body><h1>404 Not Found</h1>\n<p>check your url path or scan the QR code</p></body></html>";

    std::string body_upload = 
        "<!DOCTYPE html><html><head><title>Axon upload</title></head>"
        "<body>"
            "<h1>Upload</h1>"
            "<form method=\"post\" enctype=\"multipart/form-data\" action=\"" + url_path_recv + "\">"
                "<input name=\"file\" type=\"file\">"
                "<input type=\"submit\">"
            "</form>"
        "</body>"
        "</html>";

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

    svr.Get(url_path_send, [mimetype, &data, filepath](const httplib::Request &, httplib::Response &res) {
        res.set_content(data, mimetype);
        res.set_header("Content-Disposition", "attachment; filename=\""+getFilename(filepath)+"\"");
    });

    svr.Get(url_path_recv, [&body_upload](const httplib::Request &, httplib::Response &res){
        res.set_content(body_upload, "text/html");
    });

    svr.Post(url_path_recv, [url_path_recv](const auto& req, auto& res){
        for (const auto &file : req.files) {
            const auto &name = file.first;                 // Form field name
            const auto &file_info = file.second;          // MultipartFormData object
            const auto &filename = file_info.filename;    // Uploaded file's name
            const auto &content_type = file_info.content_type; // MIME type
            const auto &content = file_info.content;      // File content as string

            std::thread saveThread(save_file, filename, content, content.size());
            saveThread.detach();

            res.status = 301;
            res.set_header("Location", url_path_recv);

            // std::cout << "name: " << name << '\n';
            // // std::cout << "file_info: " << file_info << '\n';
            // std::cout << "filename: " << filename << '\n';
            // std::cout << "content_type: " << content_type << '\n';
            // std::cout << "content: " << content << '\n';
        }
    });

    // 404 handler
    svr.set_error_handler([&body_404](const httplib::Request& req, httplib::Response& res) {
        res.status = 404;
        res.set_content(body_404, "text/html");
    });

    svr.listen("0.0.0.0", port);

    return 0;
}