#include <EEPROM.h>
#include "RFIDAuthenticator.h"

static const uint8_t MAGIC_BYTE = 0x7C;
static const uint16_t MAGIC_BYTE_ADDR = 0x0;

static const uint16_t LENGTH_ADDR = 0x1;
static const uint16_t DATA_START_ADDR = 0x3;

static void write_uint16(uint16_t addr, uint16_t val) {
    EEPROM[addr] = (val >> 8) & 0xff;
    EEPROM[addr + 1] = val & 0xff;
}

static uint16_t read_uint16(uint16_t addr) {
    uint16_t val = 0;
    val |= (EEPROM[addr] << 8) & 0xff00;
    val |= EEPROM[addr + 1] & 0xff;
    return val;
}

static void write_uint32(uint16_t addr, uint32_t val) {
    EEPROM[addr] = (val >> 24) & 0xff;
    EEPROM[addr + 1] = (val >> 16) & 0xff;
    EEPROM[addr + 2] = (val >> 8) & 0xff;
    EEPROM[addr + 3] = val & 0xff;
}

static uint32_t read_uint32(uint16_t addr) {
    uint32_t val = 0;
    val |= (((uint32_t) EEPROM[addr]) << 24) & 0xff000000L;
    val |= (((uint32_t) EEPROM[addr + 1]) << 16) & 0xff0000L;
    val |= (((uint32_t) EEPROM[addr + 2]) << 8) & 0xff00L;
    val |= EEPROM[addr + 3] & 0xffL;
    return val;
}

RFIDAuthenticator::RFIDAuthenticator(const uint8_t rx, const uint8_t tx) :
        rfid(rx, tx) {
    if (EEPROM[MAGIC_BYTE_ADDR] != MAGIC_BYTE) {
        EEPROM[MAGIC_BYTE_ADDR] = MAGIC_BYTE;
        write_uint16(LENGTH_ADDR, 0);
        enrollID(15032277);
    }
}

Authenticator::Command RFIDAuthenticator::getCommand() {
    if (rfid.isAvailable()) {
        unsigned long id = rfid.cardNumber();
        Serial.print("# Scanned tag: ");
        Serial.println(id);
        if (checkID((uint32_t) id)) return TOGGLE;
    }
    return NONE;
}

bool RFIDAuthenticator::enrollID(uint32_t id) {
    uint16_t length = read_uint16(LENGTH_ADDR);
    if (((length * 4) + DATA_START_ADDR) <= EEPROM.length() - 4 &&
        !checkID(id)) {
        write_uint32(DATA_START_ADDR + (length * 4), id);
        write_uint16(LENGTH_ADDR, length + 1);
        return true;
    } else {
        return false;
    }
}

bool RFIDAuthenticator::checkID(uint32_t id) {
    uint16_t len = read_uint16(LENGTH_ADDR);
    for (uint16_t i = 0; i < len; ++i) {
        uint32_t db_id = read_uint32((i * 4) + DATA_START_ADDR);
        if (db_id == id) return true;
    }
    return false;
}
