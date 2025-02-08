// fetcher.h
#ifndef FETCHER_H
#define FETCHER_H

#include <string>
#include <vector>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);
std::string fetchData(const std::string& url);
bool fetchStockData();  // ประกาศฟังก์ชัน fetchStockData()

#endif // FETCHER_H
