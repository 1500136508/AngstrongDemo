#include "usbdevice.h"

USBDevice::USBDevice()
{
    //初始化信息
    driveLetter = "";
    productID = "";
    prodecerID = "";
    type = "";
    version = "";
    equipmentNumber = "";
    manufacturer = "";
}

USBDevice::~USBDevice() {}

void USBDevice::setInformation(QString driveLetter, QString productID, QString prodecerID, QString type,
                          QString version, QString equipmentNumber, QString manufacturer)
{
    this->driveLetter = driveLetter;
    this->productID = productID;
    this->prodecerID = prodecerID;
    this->type = type;
    this->version = version;
    this->equipmentNumber = equipmentNumber;
    this->manufacturer = manufacturer;
}

QString USBDevice::getDriveLetter()
{
    return driveLetter;
}

QString USBDevice::getProductID()
{
    return productID;
}

QString USBDevice::getProdecerID()
{
   return prodecerID;
}

QString USBDevice::getType()
{
    return type;
}

QString USBDevice::getVersion()
{
    return version;
}

QString USBDevice::getEquipmentNumber()
{
    return equipmentNumber;
}

QString USBDevice::getManufacturer()
{
    return manufacturer;
}
