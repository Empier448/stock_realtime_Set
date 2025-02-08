# stock_realtime_Set


//รันคำสั่งนี้ใน MSYS2 MINGW64:
 
//g++ main.cpp curl_functions.cpp fetcher.cpp -o stock_fetcher -I../include -lcurl -ljsoncpp
//. / stock_fetcher


/CURLOPT_SSL_VERIFYPEER → เปิดการตรวจสอบใบรับรอง SSL
//CURLOPT_CAINFO → ใช้ไฟล์ใบรับรองที่กำหนด(CACERT_PATH)
//CURLOPT_USERAGENT → กำหนด User - Agent
//CURLOPT_URL → กำหนด URL ที่ต้องการดึงข้อมูล
//CURLOPT_WRITEFUNCTION → ใช้ WriteCallback เพื่อเก็บข้อมูล
//CURLOPT_WRITEDATA → บันทึกข้อมูลลงใน buffer
