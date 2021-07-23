#include "pzem-004t.h"
#include "logger.h"

extern Logger logger;

Pzem::Pzem() :
    //_pzemSWSerial(PZEM_RX_PIN, PZEM_TX_PIN),
    //_pzem(_pzemSWSerial) {
    _pzem(Serial) {

}

Pzem::~Pzem() {

}

void Pzem::init() {
    uint8_t addr = _pzem.getAddress();
    logger.debug("Pzem::init - address = %d", addr);
}

bool Pzem::loop(PzemData& data) {

    logger.debug("Pzem::loop - begin. ");
    bool ret = false;

    uint8_t addr = _pzem.getAddress();
    logger.debug("Pzem::loop - address = %d", addr);

    // Read the data from the sensor
    data.voltage = _pzem.voltage();
    data.current = _pzem.current();
    data.power = _pzem.power();
    data.energy = _pzem.energy();
    data.frequency = _pzem.frequency();
    data.pf = _pzem.pf();

    // Check if the data is valid
    if(isnan(data.voltage)){
        logger.error("Pzem::loop - Error reading voltage");
    } else if (isnan(data.current)) {
        logger.error("Pzem::loop - Error reading current");
    } else if (isnan(data.power)) {
        logger.error("Pzem::loop - Error reading power");
    } else if (isnan(data.energy)) {
        logger.error("Pzem::loop - Error reading energy");
    } else if (isnan(data.frequency)) {
        logger.error("Pzem::loop - Error reading frequency");
    } else if (isnan(data.pf)) {
        logger.error("Pzem::loop - Error reading power factor");
    } else {

        ret = true;
    }

    logger.debug("Pzem::loop - end. ret=%d", ret);
    return ret;
}

void Pzem::data2json(const PzemData& data, std::string& json) {
    std::stringstream ss;
    ss.precision(3);
    ss << "{\n";
    ss << '\t' << "\"voltage\": " << data.voltage << ",\n";
    ss << '\t' << "\"current\": " << data.current << ",\n";
    ss << '\t' << "\"power\": " << data.power << ",\n";
    ss << '\t' << "\"energy\": " << data.energy << ",\n";
    ss << '\t' << "\"frequency\": " << data.frequency << ",\n";
    ss << '\t' << "\"pf\": " << data.pf << "\n";
    ss << "}";
    json = ss.str();
}