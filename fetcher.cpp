#include "fetcher.h"
#include "config.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <json/json.h>
#include <ctime>
#include <sys/stat.h>

using namespace std;

// อ่านไฟล์ CSV ที่เก็บรายชื่อหุ้นจากพาธที่กำหนด (filePath)
vector<string> getStockSymbolsFromCSV(const string& filePath) {
    vector<string> symbols;
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filePath << endl;
        return symbols;
    }

    string line;
    getline(file, line); // ข้ามบรรทัดแรก (header)
    while (getline(file, line)) {
        stringstream ss(line);
        string ticker;
        getline(ss, ticker, ',');

        // ดึงคอลัมน์แรก (สัญลักษณ์หุ้น) และเพิ่ม ".BK" ต่อท้ายหากยังไม่มี
        if (ticker.length() < 3 || ticker.substr(ticker.length() - 3) != ".BK") {
            ticker += ".BK";
        }

        symbols.push_back(ticker);
    }

    file.close();
    return symbols;
}

// ตรวจสอบว่าโฟลเดอร์ที่กำหนด (path) มีอยู่จริงหรือไม่
bool directoryExists(const string& path) {
    struct stat info;
    return stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR);
}

// รับข้อมูลเป็น vector<vector<string>> และบันทึกลงไฟล์ CSV
void saveToCSV(const vector<vector<string>>& data, const string& filename) {
    if (!directoryExists(OUTPUT_DIR)) {
        cerr << "Error: Directory " << OUTPUT_DIR << " does not exist." << endl;
        return;
    }

    ofstream file(OUTPUT_DIR + filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
            file << row[i];
            if (i < row.size() - 1) file << ",";
        }
        file << endl;
    }
    file.close();
}

// ✅ ฟังก์ชันใหม่สำหรับบันทึกข้อมูลเป็น JSON
void saveToJSON(const Json::Value& jsonData, const string& filename) {
    if (!directoryExists(OUTPUT_DIR)) {
        cerr << "Error: Directory " << OUTPUT_DIR << " does not exist." << endl;
        return;
    }

    ofstream file(OUTPUT_DIR + filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    Json::StreamWriterBuilder writer;
    file << Json::writeString(writer, jsonData);
    file.close();
}

bool fetchStockData() {
    vector<string> symbols = getStockSymbolsFromCSV(STOCK_LIST_PATH);
    vector<vector<string>> csvData;
    Json::Value jsonRoot; // ใช้เก็บ JSON

    csvData.push_back({ "Ticker", "Date", "Open", "High", "Low", "Close", "Volume" });

    bool success = false;

    for (const auto& symbol : symbols) {
        string url = "https://query1.finance.yahoo.com/v8/finance/chart/" + symbol + "?interval=1d";
        string jsonData = fetchData(url);

        if (jsonData.empty()) {
            cerr << "Error: Received empty JSON response for symbol: " << symbol << endl;
            continue;
        }

        Json::CharReaderBuilder reader;
        Json::Value root;
        string errs;
        istringstream ss(jsonData);
        if (!Json::parseFromStream(reader, ss, &root, &errs)) {
            cerr << "Error parsing JSON: " << errs << endl;
            cerr << "Received JSON: " << jsonData << endl;
            continue;
        }

        if (root["chart"]["result"].empty()) {
            cerr << "Error: No data found for symbol: " << symbol << endl;
            continue;
        }

        const Json::Value quote = root["chart"]["result"][0]["indicators"]["quote"][0];
        string open = quote["open"][0].isNull() ? "N/A" : to_string(quote["open"][0].asFloat());
        string high = quote["high"][0].isNull() ? "N/A" : to_string(quote["high"][0].asFloat());
        string low = quote["low"][0].isNull() ? "N/A" : to_string(quote["low"][0].asFloat());
        string close = quote["close"][0].isNull() ? "N/A" : to_string(quote["close"][0].asFloat());
        string volume = quote["volume"][0].isNull() ? "N/A" : to_string(quote["volume"][0].asUInt64());

        const Json::Value timestamps = root["chart"]["result"][0]["timestamp"];
        if (!timestamps.empty()) {
            time_t timestamp = timestamps[0].asInt64();
            char buffer[80];
            strftime(buffer, sizeof(buffer), "%m/%d/%Y", localtime(&timestamp));
            string stockDate(buffer);

            string cleanSymbol = (symbol.size() > 3 && symbol.substr(symbol.size() - 3) == ".BK") ? symbol.substr(0, symbol.size() - 3) : symbol;
            csvData.push_back({ cleanSymbol, stockDate, open, high, low, close, volume });

            // ✅ เพิ่มข้อมูลลง JSON
            Json::Value stockJson;
            stockJson["Ticker"] = cleanSymbol;
            stockJson["Date"] = stockDate;
            stockJson["Open"] = open;
            stockJson["High"] = high;
            stockJson["Low"] = low;
            stockJson["Close"] = close;
            stockJson["Volume"] = volume;
            jsonRoot.append(stockJson);

            success = true;
        }
        else {
            cerr << "Error: No timestamp found for symbol: " << symbol << endl;
        }
    }

    if (success) {
        saveToCSV(csvData, "stocks_data_yahoo.csv");
        saveToJSON(jsonRoot, "stocks_data_yahoo.json"); // ✅ บันทึกเป็น JSON
    }
    else {
        cerr << "No stock data retrieved. Skipping CSV and JSON save." << endl;
    }

    return success;
}
