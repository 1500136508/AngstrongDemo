#ifndef LOGWRITER_H
#define LOGWRITER_H
#include <string>
#include <fstream>
class logWriter
{
public:
    logWriter();
    ~logWriter();
    void writeAlog(std::string type, bool saved, int saveIndex, long long rgbT);
    void setSavePath(std::string _savePath);

private:
    std::string savePath;
    int saveIndex = 0;
    int saveCount = 0;
    int saveMax = 30;

    std::ofstream logFile;
};

#endif // LOGWRITER_H
