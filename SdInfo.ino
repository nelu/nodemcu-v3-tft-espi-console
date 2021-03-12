// An example of the SdFatSoftSpi template class.
// This example is for an Adafruit Data Logging Shield on a Mega.
// Software SPI is required on Mega since this shield connects to pins 10-13.
// This example will also run on an Uno and other boards using software SPI.
//
#include <SPI.h>
#include "SdFat.h"



using namespace sdfat;
ArduinoOutStream cout(cl);

#if ENABLE_SOFTWARE_SPI_CLASS  // Must be set in SdFat/SdFatConfig.h
//
// Pin numbers in templates must be constants.
const uint8_t SOFT_MISO_PIN = (D6);
const uint8_t SOFT_MOSI_PIN =(D7);
const uint8_t SOFT_SCK_PIN  = (D5);
//
// Chip select may be constant or RAM variable.
const uint8_t SD_CHIP_SELECT_PIN = (SS);

// SdFat software SPI template
SdFatSoftSpi<SOFT_MISO_PIN, SOFT_MOSI_PIN, SOFT_SCK_PIN> sd;

// Test file.
SdFile file;

// global for card size
uint32_t cardSize;

// global for card erase size
uint32_t eraseSize;

bool hasSdCard = false;

bool SdSetup() {

 cl.println(F("- SDCARD Setup"));

  // use uppercase in hex and use 0X base prefix
 // cout << uppercase << showbase << endl;

  // F stores strings in flash to save RAM
 // cout << F("SdFat version: ") << SD_FAT_VERSION << endl;

  hasSdCard = sd.begin(SD_CHIP_SELECT_PIN);
  if (!hasSdCard) {
      log(F("Failed to sd.begin(SD_CHIP_SELECT_PIN)"));
  //    return hasSdCard;
    //sd.initErrorHalt();
  } else {
    cl.println(F("+ Card Setup done"));
  }

  return hasSdCard;

}

//------------------------------------------------------------------------------
void SdLoop() {

 
  uint32_t t = millis();

  sdScreen();
  
   if(!hasSdCard && !SdSetup())
  {
    delay(1000);
    return;
  }
  cl.println(F("opening SoftSPI"));

  if (!file.open("SoftSPI.txt", O_RDWR | O_CREAT)) {
    cl.println(F("Failed to file.open"));
      return;
    //sd.errorHalt(F("open failed"));
  }
  file.println(F("This line was printed using software SPI."));

  file.rewind();
  
  while (file.available()) {
    cl.write(file.read());
  }

  file.close();
  
  t = millis() - t;

  sdInfo();

  cl.println(F("+ Card Info done"));

  delay(1000);

}


void sdInfo() {

 uint32_t t = millis();

  cardSize = sd.card()->cardSize();
  if (cardSize == 0) {
    cl.println(F("cardSize failed"));
    return;
  }

  t = millis() - t;

  cout << F("init time: ") << t << " ms" << endl;
  cout << F("Card type: ");
  switch (sd.card()->type()) {
  case SD_CARD_TYPE_SD1:
    cout << F("SD1");
    break;

  case SD_CARD_TYPE_SD2:
    cout << F("SD2");
    break;

  case SD_CARD_TYPE_SDHC:
    if (cardSize < 70000000) {
      cout << F("SDHC");
    } else {
      cout << F("SDXC");
    }
    break;

  default:
    cout << F("Unknown\n");
  }

  if (!cidDmp()) {
    return;
  }
  if (!csdDmp()) {
    return;
  }


  uint32_t ocr;
  if (!sd.card()->readOCR(&ocr)) {
    cl.println("\nreadOCR failed");
    return;
  }
  cout << F("OCR: ") << hex << ocr << dec << endl;
  if (!partDmp()) {
    return;
  }
/*   if (!sd.fsBegin()) {
    cl.println("\nFile System initialization failed.\n");
    return;
  } */
  volDmp();

}


void sdScreen() {

  cl.setTitle(String(F("SDCARD: Info")).c_str())
    ->clearContent();

}


//------------------------------------------------------------------------------
uint8_t cidDmp() {
  cid_t cid;
  if (!sd.card()->readCID(&cid)) {
    cl.println("readCID failed");
    return false;
  }
  cout << F("\nVendor ID: ");
  cout << hex << int(cid.mid) << dec << endl;
  cout << F("OEM ID: ") << cid.oid[0] << cid.oid[1] << endl;
  cout << F("Product: ");
  for (uint8_t i = 0; i < 5; i++) {
    cout << cid.pnm[i];
  }
  cout << F("\nVersion: ");
  cout << int(cid.prv_n) << '.' << int(cid.prv_m) << endl;
  cout << F("Serial number: ") << hex << cid.psn << dec << endl;

  cl.print("Produced: ");
  cout << int(cid.mdt_month) << '/';
  cout << (2000 + cid.mdt_year_low + 10 * cid.mdt_year_high);
  cout << endl;
  return true;
}
//------------------------------------------------------------------------------
uint8_t csdDmp() {
  csd_t csd;
  uint8_t eraseSingleBlock;
  if (!sd.card()->readCSD(&csd)) {
    cl.println("readCSD failed");
    return false;
  }
  if (csd.v1.csd_ver == 0) {
    eraseSingleBlock = csd.v1.erase_blk_en;
    eraseSize = (csd.v1.sector_size_high << 1) | csd.v1.sector_size_low;
  } else if (csd.v2.csd_ver == 1) {
    eraseSingleBlock = csd.v2.erase_blk_en;
    eraseSize = (csd.v2.sector_size_high << 1) | csd.v2.sector_size_low;
  } else {
    cout << F("csd version error\n");
    return false;
  }
  eraseSize++;
  cout << F("cardSize: ") << 0.000512*cardSize;
  cout << F(" MB (MB = 1,000,000 bytes)\n");

  cout << F("flashEraseSize: ") << int(eraseSize) << F(" blocks\n");
  cout << F("eraseSingleBlock: ");
  if (eraseSingleBlock) {
    cout << F("true\n");
  } else {
    cout << F("false\n");
  }
  return true;
}
//------------------------------------------------------------------------------
// print partition table
uint8_t partDmp() {
  mbr_t mbr;
  if (!sd.card()->readBlock(0, (uint8_t*)&mbr)) {
    cl.println("read MBR failed");
    return false;
  }
  for (uint8_t ip = 1; ip < 5; ip++) {
    part_t *pt = &mbr.part[ip - 1];
    if ((pt->boot & 0X7F) != 0 || pt->firstSector > cardSize) {
      cout << F("\nNo MBR. Assuming Super Floppy format.\n");
      return true;
    }
  }

  cout << F("\nSD Partition Table\n");
  cout << F("part,boot,type,start,length\n");
  for (uint8_t ip = 1; ip < 5; ip++) {
    part_t *pt = &mbr.part[ip - 1];
    cout << int(ip) << ',' << hex << int(pt->boot) << ',' << int(pt->type);
    cout << dec << ',' << pt->firstSector <<',' << pt->totalSectors << endl;
  }
  return true;
}
//------------------------------------------------------------------------------
void volDmp() {
  cout << F("Volume is FAT") << int(sd.vol()->fatType()) << endl;
  cout << F("blocksPerCluster: ") << int(sd.vol()->blocksPerCluster()) << endl;
  cout << F("clusterCount: ") << sd.vol()->clusterCount() << endl;
  cout << F("freeClusters: ");
  uint32_t volFree = sd.vol()->freeClusterCount();
  cout <<  volFree << endl;
  float fs = 0.000512*volFree*sd.vol()->blocksPerCluster();
  cout << F("freeSpace: ") << fs << F(" MB (MB = 1,000,000 bytes)\n");
  cout << F("fatStartBlock: ") << sd.vol()->fatStartBlock() << endl;

  cout << F("fatCount: ") << int(sd.vol()->fatCount()) << endl;
  cout << F("blocksPerFat: ") << sd.vol()->blocksPerFat() << endl;
  cout << F("rootDirStart: ") << sd.vol()->rootDirStart() << endl;
  cout << F("dataStartBlock: ") << sd.vol()->dataStartBlock() << endl;
  if (sd.vol()->dataStartBlock() % eraseSize) {
    cout << F("Data area is not aligned on flash erase boundaries!\n");
    cout << F("Download and use formatter from www.sdcard.org!\n");
  }
}
#else  // ENABLE_SOFTWARE_SPI_CLASS
#error ENABLE_SOFTWARE_SPI_CLASS must be set non-zero in SdFat/SdFatConfig.h
#endif  //ENABLE_SOFTWARE_SPI_CLASS
