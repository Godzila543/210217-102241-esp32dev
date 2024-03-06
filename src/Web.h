#pragma once

#include <NimBLEDevice.h>
#include "Database.h"

static NimBLEServer *pServer;

/** Handler class for characteristic actions */
class GeneratorCallback : public NimBLECharacteristicCallbacks
{
	void onWrite(NimBLECharacteristic *pCharacteristic)
	{
		Serial.print(pCharacteristic->getUUID().toString().c_str());
		Serial.print(": onWrite(), value: ");
		Serial.println(pCharacteristic->getValue().c_str());
		Database::getInstance().JSONtoPreset((char *)pCharacteristic->getValue().c_str());
	};
};

class PaletteCallback : public NimBLECharacteristicCallbacks
{
	void onWrite(NimBLECharacteristic *pCharacteristic)
	{
		Serial.print(pCharacteristic->getUUID().toString().c_str());
		Serial.print(": onWrite(), value: ");
		Serial.println(pCharacteristic->getValue().c_str());
		Database::getInstance().JSONtoPalette((char *)pCharacteristic->getValue().c_str());
	};
};

class BrightnessCallback : public NimBLECharacteristicCallbacks
{
	void onWrite(NimBLECharacteristic *pCharacteristic)
	{
		Serial.print(pCharacteristic->getUUID().toString().c_str());
		Serial.print(": onWrite(), value: ");
		Serial.println(pCharacteristic->getValue().c_str());
		Database::getInstance().JSONtoBrightness((char *)pCharacteristic->getValue().c_str());
	};
};

static GeneratorCallback genCallbacks;
static PaletteCallback palCallbacks;
static BrightnessCallback brtCallbacks;

void webInit()
{
	Serial.begin(115200);
	Serial.println("Starting NimBLE Server");

	/** sets device name */
	NimBLEDevice::init("NimBLE-Arduino");

	/** Optional: set the transmit power, default is 3db */
	NimBLEDevice::setPower(ESP_PWR_LVL_P9); /** +9db */
	NimBLEDevice::setSecurityAuth(false, false, false);

	pServer = NimBLEDevice::createServer();

	NimBLEService *pService = pServer->createService("c17378c5-b652-4e4b-9b53-45222f86e4d6");

	NimBLECharacteristic *pGenCharacteristic = pService->createCharacteristic(
		"6EEE",
		NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);

	NimBLECharacteristic *pPalCharacteristic = pService->createCharacteristic(
		"9A11",
		NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);

	NimBLECharacteristic *pBrtCharacteristic = pService->createCharacteristic(
		"B177",
		NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE);

	pGenCharacteristic->setCallbacks(&genCallbacks);
	pPalCharacteristic->setCallbacks(&palCallbacks);
	pBrtCharacteristic->setCallbacks(&brtCallbacks);

	pService->start();

	NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
	pAdvertising->addServiceUUID(pService->getUUID());
	pAdvertising->setScanResponse(true);
	pAdvertising->start();

	Serial.println("Advertising Started");
}