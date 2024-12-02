#include <iostream>
#include <fstream>
#include <thread>
#include <queue>
#include <mutex>
#include "cpp-httplib/httplib.h"
#include "common/common.hpp"
#include "settings/settings.hpp"

extern AXONSETTINGSCONF SETTINGS;
extern std::string ROOT_DIR;

int webserver(std::string filepath, std::string &data, unsigned int port, std::string url_path_send, std::string url_path_recv, std::queue<FileRC> &file_q, std::mutex& file_q_mutex)
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
"    <link rel=\"icon\" type=\"image/png\" href=\"data:image/png;base64,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\" />"
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
"            <input id=\"file-input\" name=\"file\" type=\"file\"\\>"
"            <input type=\"text\" name=\"strdata\"\\>"
"            <input type=\"submit\"\\>"
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

    svr.Post(url_path_recv, [url_path_recv, &file_q, &file_q_mutex](const httplib::Request& req, httplib::Response& res){
        for (const auto &file : req.files) {
            const auto &name = file.first;                 // Form field name
            const auto &file_info = file.second;          // MultipartFormData object
            const auto &filename = file_info.filename;    // Uploaded file's name
            const auto &content_type = file_info.content_type; // MIME type
            const auto &content = file_info.content;      // File content as string

            std::cout << "Name:"
                << "\n\t" << name << '\n';
                // << "\n\t" << filename
                // << "\n\t" << content_type
                // << "\n\t" << content
                // << '\n';

            if (name == "strdata")
            {
                std::lock_guard<std::mutex> lock(file_q_mutex);
                file_q.push({"text-" + req.remote_addr + ".txt", content, req.remote_addr, false});   
            }
            else if (!filename.empty()) {
                if (
                    filename.find("\\") != std::string::npos &&
                    filename.find("/")  != std::string::npos
                )
                {
                    std::cerr << "Suspicious filename (" << filename << "). Exiting.\n";
                    exit(0);
                }

                // std::thread saveThread(save_file, filename, content, req.remote_addr);
                // saveThread.detach();
                std::lock_guard<std::mutex> lock(file_q_mutex);
                file_q.push({filename, content, req.remote_addr, false});
            }

            // std::cout << "name: " << name << '\n';
            // // std::cout << "file_info: " << file_info << '\n';
            // std::cout << "filename: " << filename << '\n';
            // std::cout << "content_type: " << content_type << '\n';
            // std::cout << "content: " << content << '\n';
        }

        std::cout << "SETTING HEADER FOR REDIRECT...\n";
        res.status = 301;
        res.set_header("Location", url_path_recv);
    });

    // 404 handler
    svr.set_error_handler([&body_404](const httplib::Request& req, httplib::Response& res) {
        res.status = 404;
        res.set_content(body_404, "text/html");
    });

    svr.listen("0.0.0.0", port);

    return 0;
}