#include "Print.h"
#include <TFT_eSPI.h>

#define LINE_CHARACTERS 26

class Logger : public Print {

 private:
  //  char lineBuffer[LINE_CHARACTERS];
   String lineBuffer;
   int buffPos = 0;
   int linesPrinted = 0;
   int scrollDelay = 0; // delay between scrolls for readibility
   int scrollDelayLines = 4; // apply scrollDelay every n lines
 public:

   TFT_eSPI *_tftDisplay;
   TFT_eSprite *header, 
                *content;

   Logger(TFT_eSPI *display);
      ~Logger(void);


   Logger* const setDisplay();
   Logger* const setTitle(String* text);
   Logger* const setTitle(char const* text);
   Logger* const setLineDelay(int d);
   Logger* const setDelayAfterLines(int d);
   Logger* const clearContent();
   void log(char const* text);

// Support function to UTF8 decode and draw characters piped through print stream
   size_t   write(uint8_t);
};