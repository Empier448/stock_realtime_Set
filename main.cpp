#include "fetcher.h"
#include <iostream>

int main() {
    bool success = fetchStockData();  // เรียกใช้ฟังก์ชัน fetchStockData()

    if (success) {
        std::cout << "Stock data saved successfully." << std::endl;
        return 0;  // คืนค่า 0 ถ้าสำเร็จ
    }
    else {
        std::cerr << "Warning: Failed to save stock data!" << std::endl;
        return 1;  // คืนค่า 1 ถ้าล้มเหลว
    }
}


//รันคำสั่งนี้ใน MSYS2 MINGW64:
 
//g++ main.cpp curl_functions.cpp fetcher.cpp -o stock_fetcher -I../include -lcurl -ljsoncpp
//. / stock_fetcher