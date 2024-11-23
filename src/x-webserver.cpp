#include <iostream>
#include <fstream>
#include <thread>
#include <filesystem>
#include "cpp-httplib/httplib.h"
#include "common/common.hpp"
#include "settings/settings.hpp"

extern AXONSETTINGSCONF SETTINGS;
extern std::string ROOT_DIR;

void save_file(std::string filename, std::string data, unsigned __int64 size)
{
    char allow;
    std::cout << "Recieve " << filename << "? (y/n)\n";
    std::cin >> allow;
    if (allow != 'y')
        return;

    //std::cout << "SAVE TO: " << SETTINGS.save_to + filename << '\n';

    std::cout << ROOT_DIR + '/' + SETTINGS.save_to + filename << '\n';

    std::filesystem::create_directory(ROOT_DIR + '/' + SETTINGS.save_to);
    std::ofstream ofs(ROOT_DIR + '/' + SETTINGS.save_to + filename, std::ios::binary);
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
"<!DOCTYPE html>"
"<html>"
"<head>"
"    <title>Axon upload</title>"
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
"    <style>"
"        "
"        body {"
"            font-family: \"Geist Mono\", serif;"
"            font-weight: 400;"
"        }"
"        input {"
"            font-family: \"Geist Mono\", serif;"
"            font-weight: 400;"
"        }"
"        .main-content {"
"            justify-content: center;"
"            justify-items: center;"
"            text-align: center;"
"            width: 80%;"
"            margin: auto;"
"            display: block;"
"        }"
"        .section-title {"
"            display: block; /* Make the span behave like a block element */"
"            text-align: center; /* Center-align the text */"
"            font-size: 34px;"
"            margin-bottom: 10px; /* Optional: Adds spacing between the title and the form */"
"        }"
"        form {"
"            display: flex;"
"            flex-direction: column; /* Stack elements vertically */"
"            align-items: center;    /* Center elements horizontally */"
"            background-color: #f9f9f9; /* Light gray background */"
"            padding: 20px;          /* Add some space inside the form */"
"            border: 1px solid #ccc; /* Add a border */"
"            width: 20%;           /* Set form width */"
"            margin: 20px auto;      /* Center the form on the page */"
"            box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1); /* Subtle shadow */"
"        }"
"        @media (max-width: 768px) {"
"            form {"
"                display: flex;"
"                flex-direction: column; /* Stack elements vertically */"
"                align-items: center;    /* Center elements horizontally */"
"                background-color: #f9f9f9; /* Light gray background */"
"                padding: 20px;          /* Add some space inside the form */"
"                border: 1px solid #ccc; /* Add a border */"
"                width: 90%;           /* Set form width */"
"                margin: 20px auto;      /* Center the form on the page */"
"                box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1); /* Subtle shadow */"
"            }"
"        }"
"        .file-input-label {"
"            padding: 10px 0px;"
"            margin: 5px;"
"            background-color: #FF353C; /* Blue background */"
"            color: #fff;              /* White text */"
"            border-radius: 5px;"
"            width: 85%;"
"            transition: background-color 0.3s ease; /* Smooth hover effect */"
"            text-align: center;"
"        }"
"        .file-input-label:hover {"
"            background-color: #DD2a2a; /* Darker blue on hover */"
"        }"
"        input[type=\"file\"] {"
"            display: none;"
"        }"
"        input[type=\"submit\"] {"
"            background-color: #FF353C; /* Blue button background */"
"            color: #fff;              /* White text */"
"            border: none;             /* Remove border */"
"            width: 85%;"
"            margin: 5px;"
"            padding: 10px 0px;       /* Add padding */"
"            font-size: 1em;           /* Font size */"
"            cursor: pointer;          /* Pointer cursor on hover */"
"            border-radius: 5px;       /* Rounded corners */"
"            transition: background-color 0.3s ease; /* Smooth hover effect */"
"        }"
"        input[type=\"submit\"]:hover {"
"            background-color: #DD2a2a; /* Darker blue on hover */"
"        }"
"    </style>"
"    <link rel=\"preconnect\" href=\"https://fonts.googleapis.com\">"
"    <link rel=\"preconnect\" href=\"https://fonts.gstatic.com\" crossorigin>"
"    <link href=\"https://fonts.googleapis.com/css2?family=Geist+Mono:wght@100..900&family=Geist:wght@100..900&display=swap\" rel=\"stylesheet\">"
"</head>"
"<body>"
"    <div class=\"main-content\">"
"        <form method=\"post\" enctype=\"multipart/form-data\" action=\"" + url_path_recv + "\">"
"            <span class=\"section-title\">Send to " + SETTINGS.host + "</span>"
"            <hr style=\"width:100%;\">"
"            <label for=\"file-input\" class=\"file-input-label\">"
"                Browse"
"            </label>"
"            <input id=\"file-input\" name=\"file\" type=\"file\">"
"            <input type=\"submit\">"
"        </form>"
"    </div>"
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