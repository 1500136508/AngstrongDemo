#include "logwriter.h"

#include <sys/timeb.h>

unsigned long long currentTimeMillisec(){
    struct timeb tb;
    ftime(&tb);
    return tb.time * 1000 + tb.millitm;
}


logWriter::logWriter()
{
    logFile.open("frame-image.log",std::ios::app);
}

logWriter::~logWriter()
{
    logFile.close();
}

void logWriter::writeAlog(std::string type, bool saved, int saveIndex, long long rgbT)
{
    std::string timeStamp = std::to_string(currentTimeMillisec());
    std::string saveStr = saved? "true":"false";
    std::string logData = timeStamp + " " + type + " " + saveStr;
    if (saved) {
        if (type=="rgb" && rgbT!=-1){
            logData += (" " + savePath + "//" + type + "//" + std::to_string(rgbT) +".jpg");
        }
        else{
            std::string fileName = std::to_string(saveIndex);
            int zeroNumber = 0;
            int tmp = saveIndex;
            while (tmp > 0){
                zeroNumber += 1;
                tmp /= 10;
            }
            for (int k =0;k<4-zeroNumber;k++)  fileName = "0"+fileName;
            logData += (" " + savePath + "//" + type + "//" + fileName);
            if (type == "rgb") logData+=".jpg";
            else if (type == "ir" || type == "depth") logData += ".png";
        }
    }

    logFile << logData << std::endl;
    logFile.flush();
}

void logWriter::setSavePath(std::string _savePath)
{
    savePath = _savePath;
}
