import logging
import serial
import time

logging.basicConfig(format='%(asctime)s %(levelname)-8s %(message)s', level=logging.DEBUG)
logger = logging.getLogger("PZEM")

class CRC16:
  INITIAL_MODBUS = 0xFFFF
  INITIAL_DF1 = 0x0000

  table = (
  0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
  0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
  0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
  0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
  0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
  0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
  0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
  0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
  0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
  0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
  0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
  0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
  0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
  0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
  0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
  0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
  0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
  0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
  0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
  0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
  0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
  0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
  0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
  0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
  0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
  0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
  0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
  0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
  0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
  0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
  0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
  0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040 )

  @staticmethod
  def get(data: bytearray, size: int):
    crc = CRC16.INITIAL_MODBUS
    for i in range(0,size):
        b=data[i]
        crc = (crc >> 8) ^ CRC16.table[(crc ^ b) & 0xFF]
    return crc

class PZEM:

  PZEM_BAUD_RATE = 9600
  PZEM_DEFAULT_ADDR = 0xF8  
  REG_VOLTAGE     = 0x0000
  REG_CURRENT_L   = 0x0001
  REG_CURRENT_H   = 0X0002
  REG_POWER_L     = 0x0003
  REG_POWER_H     = 0x0004
  REG_ENERGY_L    = 0x0005
  REG_ENERGY_H    = 0x0006
  REG_FREQUENCY   = 0x0007
  REG_PF          = 0x0008
  REG_ALARM       = 0x0009
  CMD_RHR         = 0x03
  CMD_RIR         = 0X04
  CMD_WSR         = 0x06
  CMD_CAL         = 0x41
  CMD_REST        = 0x42
  WREG_ALARM_THR   = 0x0001
  WREG_ADDR        = 0x0002
  UPDATE_TIME     = 200
  RESPONSE_SIZE = 32
  READ_TIMEOUT = 100
  INVALID_ADDRESS = 0x00

  def __init__(self) -> None:
      self.port = None
      self.addr = PZEM.PZEM_DEFAULT_ADDR

  def connect(self, com: str):
    self.port = serial.Serial(com, PZEM.PZEM_BAUD_RATE)
    if self.port.is_open:
      logger.info(f"Connected to port {com}")

  def disconnect(self):
    if self.port is not None:
      self.port.close()

  def read_address(self, update: bool = True):
    logger.debug("PZEM.read_address - Begin.")

    self._sendCmd8(PZEM.CMD_RHR, PZEM.WREG_ADDR, 0x01, False)
    response = self._receive()

    if len(response) != 7:
      raise f"Invalid response length! Expected 7, got {len(response)}"

    #Get the current address
    addr = (response[3] << 8 | response[4])

    # Update the internal address if desired
    if update:
        self.addr = addr

    logger.debug(f"PZEM.read_address - End. addr = {addr}")

  def update_values(self):
    logger.debug("PZEM.update_values - Begin.")
    self._sendCmd8(PZEM.CMD_RIR, PZEM.REG_VOLTAGE, 10)
    response = self._receive()

    if len(response) != 25:
      raise f"Invalid response length! Expected 25, got {len(response)}"

    out_values = {
      "voltage": (response[3] << 8 | response[4])/10.0,
      "current": (response[5] << 8 | response[6] | response[7] << 24 | response[8] << 16) / 1000.0,
      "power": (response[9] << 8 | response[10] | response[11] << 24 | response[12] << 16) / 10.0,
      "energy": (response[13] << 8 | response[14] | response[15] << 24 | response[16] << 16) / 1000.0,
      "frequency": (response[17] << 8 | response[18]) / 10.0,
      "pf": (response[19] << 8 | response[20])/100.0,
      "alarms": (response[21] << 8 | response[22])   
    }

    logger.debug(f"PZEM.update_values - End. {out_values}")
    return out_values

  def _sendCmd8(self, cmd, rAddr, val, slave_addr=0xFFFF):
    
    if (slave_addr == 0xFFFF) or (slave_addr < 0x01) or (slave_addr > 0xF7):
        slave_addr = self.addr
    
    sendBuffer = bytearray(8)
    sendBuffer[0] = slave_addr
    sendBuffer[1] = cmd
    sendBuffer[2] = (rAddr >> 8) & 0xFF
    sendBuffer[3] = (rAddr) & 0xFF
    sendBuffer[4] = (val >> 8) & 0xFF
    sendBuffer[5] = (val) & 0xFF
    crc = CRC16.get(sendBuffer, 6)
    sendBuffer[6] = (crc) & 0xFF
    sendBuffer[7] = (crc >> 8) & 0xFF

    self.port.write(sendBuffer)

  def _receive(self):
    recBuffer = bytearray()
    while True:
      time.sleep(0.1)
      numBytes = self.port.inWaiting()
      if numBytes > 0:
        bytes_ = self.port.read(numBytes)
        recBuffer += bytes_
      else:
        break
    return recBuffer


if __name__ == "__main__":
  pzem = PZEM()
  pzem.connect("COM5")
  pzem.read_address()
  
  try:
    while True:
      pzem.update_values()
      time.sleep(3)
  except KeyboardInterrupt:
    print('interrupted!')

  pzem.disconnect()