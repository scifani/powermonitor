#ifndef __PZEM_004T__
#define __PZEM_004T__

#include <PZEM004Tv30.h>
#include <SoftwareSerial.h>

/* Use software serial for the PZEM
 * Pin 12 Rx (Connects to the Tx pin on the PZEM)
 * Pin 13 Tx (Connects to the Rx pin on the PZEM)
*/
#if !defined(PZEM_RX_PIN) && !defined(PZEM_TX_PIN)
#define PZEM_RX_PIN 12
#define PZEM_TX_PIN 13
#endif

typedef struct _PzemData {
    float voltage;
    float current;
    float power;
    float energy;
    float frequency;
    float pf;
} PzemData;

class Pzem {
public:
    Pzem();
    virtual ~Pzem();

    void init();
    bool loop(PzemData& data);

    static void data2json(const PzemData& data, std::string& json);

private:
    SoftwareSerial _pzemSWSerial;
    PZEM004Tv30 _pzem;
};

#endif //__PZEM_004T__