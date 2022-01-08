/**
* @brief This function draws the a "latched" dot. it uses the logonumber, colomn and row to
*        determine where.
*
* @param b int 
* @param col int
* @param row int
*
* @return none
*
* @note none
*/
void drawlatched(int b, int col, int row)
{
  int offset;
  if(SCREEN_WIDTH < 480)
  {
    offset = 2;
  }
  else
  {
    offset = 12;
  }
  tft.fillRoundRect((KEY_X - 37 + col * (KEY_W + KEY_SPACING_X)) - offset, (KEY_Y - 37 + row * (KEY_H + KEY_SPACING_Y)) - offset, 18, 18, 4, generalconfig.latchedColour);
}

/**
* @brief This function draws the logos according to the page
         we are currently on. The pagenumber is a global variable
         and doesn't need to be passed. 
*
* @param logonumber int 
* @param col int
* @param row int
* @param transparent boolean
*
* @return none
*
* @note Logos start at the top left and are 0 indexed. The same goes 
         for the colomn and the row.
*/
void drawlogo(int logonumber, int col, int row, bool transparent, bool latch)
{
  if (pageNum == 0)
  {
    //Draw Home screen logo's
    if (logonumber < 6)
    {
        drawBmp(screen[pageNum].logo[logonumber], KEY_X - 36 + col * (KEY_W + KEY_SPACING_X), KEY_Y - 36 + row * (KEY_H + KEY_SPACING_Y), transparent);
    }
  }
  else if (pageNum < 6)
  {
    // --------------------------------- MENU 1 --------------------------------------
    if (logonumber < 5)
    {
      if (latch == true)
      {

        if (strcmp(menu[pageNum-1].button[logonumber].latchlogo, "/logos/") != 0)
        {
          drawBmp(menu[pageNum-1].button[logonumber].latchlogo, KEY_X - 36 + col * (KEY_W + KEY_SPACING_X), KEY_Y - 36 + row * (KEY_H + KEY_SPACING_Y), transparent);
        }
        else
        {
          drawBmp(screen[pageNum].logo[logonumber], KEY_X - 36 + col * (KEY_W + KEY_SPACING_X), KEY_Y - 36 + row * (KEY_H + KEY_SPACING_Y), transparent);
          drawlatched(logonumber, col, row);
        }
      }
      else
      {
        drawBmp(screen[pageNum].logo[logonumber], KEY_X - 36 + col * (KEY_W + KEY_SPACING_X), KEY_Y - 36 + row * (KEY_H + KEY_SPACING_Y), transparent);
      }
    }
    else if (logonumber == 5)
    {
      drawBmp(generallogo.homebutton, KEY_X - 36 + col * (KEY_W + KEY_SPACING_X), KEY_Y - 36 + row * (KEY_H + KEY_SPACING_Y), transparent);
    }
  }
  else if (pageNum == 6)
  {
    // pageNum6 contains settings logos
    if (logonumber == 0)
    {
      drawBmp(generallogo.configurator, KEY_X - 36 + col * (KEY_W + KEY_SPACING_X), KEY_Y - 36 + row * (KEY_H + KEY_SPACING_Y), true);
    }
    else if (logonumber == 1)
    {
      drawBmp("/logos/brightnessdown.bmp", KEY_X - 36 + col * (KEY_W + KEY_SPACING_X), KEY_Y - 36 + row * (KEY_H + KEY_SPACING_Y), true);
    }
    else if (logonumber == 2)
    {
      drawBmp("/logos/brightnessup.bmp", KEY_X - 36 + col * (KEY_W + KEY_SPACING_X), KEY_Y - 36 + row * (KEY_H + KEY_SPACING_Y), true);
    }
    else if (logonumber == 3)
    {
      drawBmp("/logos/sleep.bmp", KEY_X - 36 + col * (KEY_W + KEY_SPACING_X), KEY_Y - 36 + row * (KEY_H + KEY_SPACING_Y), true);
      if (latch)
      {
        drawlatched(logonumber, col, row);
      }
    }
    else if (logonumber == 4)
    {
      drawBmp("/logos/info.bmp", KEY_X - 36 + col * (KEY_W + KEY_SPACING_X), KEY_Y - 36 + row * (KEY_H + KEY_SPACING_Y), true);
    }
    else if (logonumber == 5)
    {
      drawBmp(generallogo.homebutton, KEY_X - 36 + col * (KEY_W + KEY_SPACING_X), KEY_Y - 36 + row * (KEY_H + KEY_SPACING_Y), transparent);
    }
  }
}

/**
* @brief This function draws the 6 buttons that are on every page.
         Pagenumber is global and doesn't need to be passed.
*
* @param none
*
* @return none
*
* @note Three possibilities: pagenumber = 0 means homescreen,
         pagenumber = 7 means config mode, anything else is a menu.
*/
void drawKeypad()
{
  // Draw the home screen button outlines and fill them with colours
  if (pageNum == 0)
  {
    tft.startWrite();
    for (uint8_t row = 0; row < 2; row++)
    {
      for (uint8_t col = 0; col < 3; col++)
      {

        uint8_t b = col + row * 3;
        uint16_t buttonBG;
        bool drawTransparent;
        uint16_t imageBGColor = getImageBG(b);
        if (imageBGColor > 0)
        {
          buttonBG = imageBGColor;
          drawTransparent = false;
        }
        else
        {
          buttonBG = generalconfig.menuButtonColour;
          drawTransparent = true;
        }
        tft.setFreeFont(LABEL_FONT);
        key[b].initButton(&tft, KEY_X + col * (KEY_W + KEY_SPACING_X),
                          KEY_Y + row * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                          KEY_W, KEY_H, TFT_WHITE, buttonBG, TFT_WHITE,
                          "", KEY_TEXTSIZE);
        key[b].drawButton();
        drawlogo(b, col, row, drawTransparent, false); // After drawing the button outline we call this to draw a logo.
      }
    }
    tft.endWrite();
    return;
  }
  
  else if (pageNum == 10)
  {
    // Pagenum 10 means that a JSON config failed to load completely.
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.printf("  %s failed to load and might be corrupted.\n", jsonfilefail);
    tft.println("  You can reset that specific file to default by opening the serial monitor");
    tft.printf("  and typing \"reset %s\"\n", jsonfilefail);
    tft.println("  If you don't do this, the configurator will fail to load.");
  }
  else
  {
    // Draw the button outlines and fill them with colours
    for (uint8_t row = 0; row < 2; row++)
    {
      for (uint8_t col = 0; col < 3; col++)
      {

        uint8_t b = col + row * 3;

        if (row == 1 && col == 2)
        {

          // Check if "home.bmp" is a transparent one
          
          uint16_t buttonBG;
          bool drawTransparent;
          uint16_t imageBGColor;

          imageBGColor = getImageBG(b);

          if (imageBGColor > 0)
          {
            buttonBG = imageBGColor;
            drawTransparent = false;
          }
          else
          {
            buttonBG = generalconfig.menuButtonColour;
            drawTransparent = true;
          }
          
          tft.setFreeFont(LABEL_FONT);
          key[b].initButton(&tft, KEY_X + col * (KEY_W + KEY_SPACING_X),
                            KEY_Y + row * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                            KEY_W, KEY_H, TFT_WHITE, buttonBG, TFT_WHITE,
                            "", KEY_TEXTSIZE);
          key[b].drawButton();
          drawlogo(b, col, row, drawTransparent, false);
        }
        else
        {
          // Otherwise use functionButtonColour

          int index = b + ((pageNum-1)*5);

          uint16_t buttonBG;
          bool drawTransparent;
          uint16_t imageBGColor;
          if (islatched[index] && b < 5)
          {
            imageBGColor = getLatchImageBG(b);
          }
          else
          {
            imageBGColor = getImageBG(b);
          }

          if (imageBGColor > 0)
          {
            buttonBG = imageBGColor;
            drawTransparent = false;
          }
          else
          {
            buttonBG = generalconfig.functionButtonColour;
            drawTransparent = true;
          }
          tft.setFreeFont(LABEL_FONT);
          key[b].initButton(&tft, KEY_X + col * (KEY_W + KEY_SPACING_X),
                            KEY_Y + row * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                            KEY_W, KEY_H, TFT_WHITE, buttonBG, TFT_WHITE,
                            "", KEY_TEXTSIZE);
          key[b].drawButton();
          // After drawing the button outline we call this to draw a logo.
          if (islatched[index] && b < 5)
          {
            drawlogo(b, col, row, drawTransparent, true);
          }
          else
          {
            drawlogo(b, col, row, drawTransparent, false);
          }
        }
      }
    }
  }
}

/* ------------- Print an error message the TFT screen  ---------------- 
Purpose: This function prints an message to the TFT screen on a black 
         background. 
Input  : String message
Output : none
Note   : none
*/

void drawErrorMessage(String message)
{

  tft.fillScreen(TFT_BLACK);
  tft.setCursor(20, 20);
  tft.setTextFont(2);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println(message);
}

/**
* @brief This function gets the Bluetooth device address and prints it to the serial monitor
         and the TFT screen in a 6 byte format, seperating each byte by ":".
*
* @param none
*
* @return none
*
* @note e.g. 00:11:22:33:22:EE
*/
void printDeviceAddress()
{

  const uint8_t *point = esp_bt_dev_get_address();

  for (int i = 0; i < 6; i++)
  {

    char str[3];

    sprintf(str, "%02X", (int)point[i]);
    //Serial.print(str);
    tft.print(str);

    if (i < 5)
    {
     // Serial.print(":");
      tft.print(":");
    }
  }
}

/**
* @brief This function prints some information about the current version 
         and setup of FreetouchDeck to the TFT screen.
*
* @param none
*
* @return none
*
* @note none
*/
void printinfo()
{
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(1, 3);
  tft.setTextFont(2);
  if(SCREEN_WIDTH < 480)
  {
    tft.setTextSize(1);
  }
  else
  {
    tft.setTextSize(2);
  }
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.printf("Version: %s\n", versionnumber);

#ifdef touchInterruptPin
  if (generalconfig.sleepenable)
  {
    tft.printf("Sleep: %u minutes\n", generalconfig.sleeptimer);
  }
  else
  {
    tft.println("Sleep: Disabled");
  }
#else
  tft.println("Sleep: Disabled");
#endif

#ifdef speakerPin
  if(generalconfig.beep){
    tft.println("Speaker: Enabled");
  }
  else
  {
    tft.println("Speaker: Disabled");
  }
#else
  tft.println("Speaker: Disabled");
#endif

  tft.print("Free Storage: ");
  float freemem = SPIFFS.totalBytes() - SPIFFS.usedBytes();
  tft.print(freemem / 1000);
  tft.println(" kB");
  tft.print("Heap: ");
  tft.println(ESP.getFreeHeap());
  tft.print("BLE Keyboard version: ");
  tft.println(BLE_KEYBOARD_VERSION);
  tft.print("ArduinoJson version: ");
  tft.println(ARDUINOJSON_VERSION);
  tft.print("TFT_eSPI version: ");
  tft.println(TFT_ESPI_VERSION);
  tft.println("ESP-IDF: ");
  tft.println(esp_get_idf_version());

  displayinginfo = true;
}
