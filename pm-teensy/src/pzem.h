#ifndef __PZEM_004T__
#define __PZEM_004T__

#include <PZEM004Tv30.h>
#include <SoftwareSerial.h>
#include <string>
#include <sstream>
#include <vector>

typedef struct _PzemData {
    float voltage;
    float current;
    float power;
    float energy;
    float frequency;
    float pf;
    int addr;
} PzemData;

class Pzem {
public:
    Pzem(uint8_t receivePin, uint8_t transmitPin);
    virtual ~Pzem();

    void init();
    bool loop(PzemData& data, uint8_t addr=1);

    static void data2json(const PzemData& data, std::string& json);

private:
    SoftwareSerial _pzemSWSerial;
    PZEM004Tv30 _pzem;
};

class PzemArray {
public:
  PzemArray() {}
  virtual ~PzemArray() {}

    void addPzem(Pzem* pzem, uint8_t addr);
    bool loop(std::vector<PzemData>& data);    
      
private:

  class PzemItem {
  public:
    PzemItem(Pzem* pzem, uint8_t addr) {
      _pzem = pzem;
      _addr = addr;
    }
        
    Pzem* _pzem;
    uint8_t _addr;
  };

  std::vector<PzemItem> _pzemArray;
};

#endif //__PZEM_004T__