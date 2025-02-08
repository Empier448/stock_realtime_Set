#include "curl_functions.h"
#include <iostream>

using namespace std;

 
//const string CACERT_PATH = "C:/Users/Plaif/Downloads/Data/Stock_Set/cacert-2024-12-31.pem";
const string CACERT_PATH = "C:/Users/Plaifa/Downloads/Data/cacert-2024-12-31.pem";

size_t WriteCallback(void* contents, size_t size, size_t nmemb, string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}
     //fetchData (ดึงข้อมูลจาก URL)
string fetchData(const string& url) {
    CURL* curl;
    CURLcode res;
    string buffer;

    curl = curl_easy_init();  //ใช้ curl_easy_init() เพื่อเริ่มต้นใช้งาน cURL
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl, CURLOPT_CAINFO, CACERT_PATH.c_str());
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "YourCustomUserAgentString");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

        res = curl_easy_perform(curl);  //เรียกใช้ curl_easy_perform() เพื่อส่งคำขอ HTTP ไปยัง URL
        if (res != CURLE_OK) {          //ตรวจสอบว่าคำขอสำเร็จหรือไม่ (CURLE_OK)
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;
            return "";
        }
        else {
            long response_code;  //ดึง HTTP Response Code จากเซิร์ฟเวอร์
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            cout << "HTTP Response Code: " << response_code << endl;
            if (response_code != 200) {       //ถ้า ไม่ใช่ 200 (OK) แสดงว่าเกิดข้อผิดพลาด
                cerr << "Error: Received HTTP response code " << response_code << endl;
                return "";
            }
        }
        curl_easy_cleanup(curl);   //ใช้ curl_easy_cleanup() เพื่อเคลียร์หน่วยความจำ
    }
    return buffer;
}
//CURLOPT_SSL_VERIFYPEER → เปิดการตรวจสอบใบรับรอง SSL
//CURLOPT_CAINFO → ใช้ไฟล์ใบรับรองที่กำหนด(CACERT_PATH)
//CURLOPT_USERAGENT → กำหนด User - Agent
//CURLOPT_URL → กำหนด URL ที่ต้องการดึงข้อมูล
//CURLOPT_WRITEFUNCTION → ใช้ WriteCallback เพื่อเก็บข้อมูล
//CURLOPT_WRITEDATA → บันทึกข้อมูลลงใน buffer