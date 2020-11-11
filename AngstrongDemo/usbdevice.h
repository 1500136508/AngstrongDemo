#ifndef USBDEVICE_H
#define USBDEVICE_H
#include <QString>

class USBDevice
{
public:
    USBDevice();
    ~USBDevice();
    //U盘的信息
    void setInformation(QString driveLetter, QString productID, QString prodecerID, QString type,
                        QString version, QString equipmentNumber, QString manufacturer);
    void setIN(QString driveLetter)
    {
        this->driveLetter = driveLetter;
    }

    QString getDriveLetter();
    QString getProductID();
    QString getProdecerID();
    QString getType();
    QString getVersion();
    QString getEquipmentNumber();
    QString getManufacturer();

private:
    QString driveLetter;           //U盘驱动器盘符
    QString productID;             //产品ID
    QString prodecerID;            //厂商ID
    QString type;                  //设备型号
    QString version;               //设备版本
    QString equipmentNumber;       //设备序列号
    QString manufacturer;          //设备制造商

};

#endif // USBDEVICE_H
