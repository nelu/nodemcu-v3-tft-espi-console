#include "Logger.h"
#include <TFT_eSPI.h>

Logger::~Logger(void) {
    delete header;
    delete content;
}

/* default implementation: may be overridden */
Logger::Logger(TFT_eSPI *display)
{
   _tftDisplay = display; 
}

Logger* const Logger::setDisplay()
{

    header = new TFT_eSprite(_tftDisplay); // console title
    content = new TFT_eSprite(_tftDisplay); // output container
 
    _tftDisplay->fillScreen(TFT_BLACK);
    _tftDisplay->setRotation(1);

    // Create a sprite for header
    header->setColorDepth(8);
    header->createSprite(160, 14);
    header->setScrollRect(0, 0, 160, 14, TFT_DARKGREY); 
    header->setTextColor(TFT_WHITE); 
 

    content->setColorDepth(8);
    content->createSprite(160, 114);
    content->fillSprite(TFT_BLUE); 
    content->setScrollRect(0, 0, 160, 114, TFT_BLUE);   
    content->setTextColor(TFT_WHITE);
    content->setTextDatum(BL_DATUM);  
    return this;
}


Logger* const Logger::clearContent() {
    content->fillSprite(TFT_BLUE); 
    return this;
}

Logger* const Logger::setTitle(String* text) {

    return setTitle(text->c_str());
}

Logger* const Logger::setTitle(char const* text) {

    header->fillSprite(TFT_DARKGREY);
    header->drawString(text, 2, -2, 2);
    header->pushSprite(0, 0);

    return this;
}

Logger* const Logger::setLineDelay(int d) {

    scrollDelay = d;
    return this;
}

Logger* const Logger::setDelayAfterLines(int l) {

    scrollDelayLines = l;
    return this;
}



void Logger::log(char const* text) {

    content->drawString(text, 2, 111, 1); 
    content->pushSprite(0, 15); 
    content->scroll(0,-11);

    linesPrinted++;

    if(linesPrinted%scrollDelayLines == 0){
        delay(scrollDelay);
    }

    if(linesPrinted > scrollDelayLines) {
        linesPrinted = 0;
    } 
}


size_t Logger::write(uint8_t utf8)
{
    
    if (utf8 == '\n') {
        log(lineBuffer.c_str());
        buffPos = 0;
        //lineBuffer[0] = (char) 0;
        lineBuffer = "";
        return 1;
    }

    if (buffPos == (int)LINE_CHARACTERS) {
        log(lineBuffer.c_str());
        buffPos = 0;
        //lineBuffer[0] = (char)utf8;
        lineBuffer = (char)utf8;
        return 1;
    } 

/*     
    Serial.print("Got char Logger:Write utf8: ");
    Serial.println(utf8);
    Serial.print("Got char sprintln (char):  ");
    Serial.println((char)utf8);
    Serial.println("-----------------"); 
*/
 
    //lineBuffer[buffPos] = (char)utf8;
    lineBuffer.concat(String((char)utf8));
    buffPos++;

    return 1;

}

