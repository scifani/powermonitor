#include "pzem.h"
#include "logger.h"

extern Logger logger;

Pzem::Pzem(uint8_t receivePin, uint8_t transmitPin) :
    _pzemSWSerial(receivePin, transmitPin),
    _pzem(_pzemSWSerial) {

}

Pzem::~Pzem() {

}

void Pzem::init() {    
    uint8_t addr = _pzem.readAddress();
    logger.debug("Pzem::init - address = %d", addr);
}

bool Pzem::loop(PzemData& data, uint8_t addr) {

    logger.debug("Pzem::loop - begin. addr=%d", addr);
    bool ret = false;

    _pzem.setAddress(addr);
  
    // Read the data from the sensor
    data.addr = (int)addr;
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
    ss << '\t' << "\"addr\": " << data.addr << ",\n";
    ss << '\t' << "\"voltage\": " << data.voltage << ",\n";
    ss << '\t' << "\"current\": " << data.current << ",\n";
    ss << '\t' << "\"power\": " << data.power << ",\n";
    ss << '\t' << "\"energy\": " << data.energy << ",\n";
    ss << '\t' << "\"frequency\": " << data.frequency << ",\n";
    ss << '\t' << "\"pf\": " << data.pf << "\n";
    ss << "}";
    json = ss.str();
}


void PzemArray::addPzem(Pzem* pzem, uint8_t addr) {  
  PzemItem item(pzem, addr);
 _pzemArray.push_back(item);
}

bool PzemArray::loop(std::vector<PzemData>& data) {
  for (auto it : _pzemArray) {
    PzemData item_data;    
    it._pzem->loop(item_data, it._addr);
    data.push_back(item_data);
  }

  return true;
}