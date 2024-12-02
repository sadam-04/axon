#include <string>
#include <queue>
#include <mutex>
#include "common/common.hpp"

int create_window(std::string qr_bmp, std::string url, std::queue<FileRC> &file_q, std::mutex &file_q_mutex, std::string filename = "");