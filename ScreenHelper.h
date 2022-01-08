/**
* @brief This function reads chuncks of 2 bytes of data from a
         file and returns the data.
*
* @param &f
*
* @return uint16_t
*
* @note litte-endian
*/
uint16_t read16(fs::File &f)
{
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

/**
* @brief This function reads chuncks of 4 bytes of data from a
         file and returns the data.
*
* @param &f
*
* @return uint32_t
*
* @note litte-endian
*/
uint32_t read32(fs::File &f)
{
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

/**
* @brief This functions accepts a HTML including the # colour code 
         (eg. #FF00FF)  and returns it in RGB888 format.
*
* @param *html char (including #)
*
* @return unsigned long
*
* @note none
*/
unsigned long convertHTMLtoRGB888(char *html)
{
  char *hex = html + 1; // remove the #
  unsigned long rgb = strtoul(hex, NULL, 16);
  return rgb;
}

/**
* @brief This function converts RGB888 to RGB565.
*
* @param rgb unsigned long
*
* @return unsigned int
*
* @note none
*/
unsigned int convertRGB888ToRGB565(unsigned long rgb)
{
  return (((rgb & 0xf80000) >> 8) | ((rgb & 0xfc00) >> 5) | ((rgb & 0xf8) >> 3));
}

/**
* @brief This function draws a BMP on the TFT screen according
         to the given x and y coordinates. 
*
* @param  *filename
* @param x int16_t 
* @param y int16_t 
*
* @return none
*/
void drawBmp(const char *filename, int16_t x, int16_t y, bool transparent = false)
{

  if ((x >= tft.width()) || (y >= tft.height()))
    return;

  fs::File bmpFS;

  bmpFS = FILESYSTEM.open(filename, "r");

  if (!bmpFS)
  {

    Serial.print("File not found:");
    Serial.println(filename);
    return;
  }

  uint32_t seekOffset;
  uint16_t w, h;
  uint8_t r, g, b;

  if (read16(bmpFS) == 0x4D42)
  {
    read32(bmpFS);
    read32(bmpFS);
    seekOffset = read32(bmpFS);
    read32(bmpFS);
    w = read32(bmpFS);
    h = read32(bmpFS);
    Serial.printf("Image size: %d x %d\n", w, h);

    if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) && (read32(bmpFS) == 0))
    {
      y += h - 1;

      bool oldSwapBytes = tft.getSwapBytes();
      tft.setSwapBytes(true);
      bmpFS.seek(seekOffset);

      uint16_t padding = (4 - ((w * 3) & 3)) & 3;
      uint8_t lineBuffer[2][w * 3 + padding];
      int8_t bufIdx = 0;

      for (uint16_t row = 0; row < h; row++)
      {
        bmpFS.read(lineBuffer[bufIdx], sizeof(lineBuffer[bufIdx]));
        uint8_t *bptr = lineBuffer[bufIdx];
        uint16_t *tptr = (uint16_t *)lineBuffer[bufIdx];
        // Convert 24 to 16 bit colours
        for (uint16_t col = 0; col < w; col++)
        {
          b = *bptr++;
          g = *bptr++;
          r = *bptr++;
          *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        }

        // Push the pixel row to screen, pushImage will crop the line if needed
        // y is decremented as the BMP image is drawn bottom up
        if(transparent)
        {
          tft.pushImage(x, y--, w, 1, (uint16_t *)lineBuffer[bufIdx], TFT_BLACK);
        }
        else
        {
          tft.pushImageDMA(x, y--, w, 1, (uint16_t *)lineBuffer[bufIdx]);
        }

        bufIdx = 1 - bufIdx;
      }
      tft.setSwapBytes(oldSwapBytes);
    }
    else
      Serial.println("[WARNING]: BMP format not recognized.");
  }
  bmpFS.close();
}

/**
* @brief This function reads a number of bytes from the given
         file at the given position.
*
* @param *p_file File
* @param position int
* @param nBytes byte 
*
* @return int32_t
*
* @note none
*/
int32_t readNbytesInt(File *p_file, int position, byte nBytes)
{
  if (nBytes > 4)
    return 0;

  p_file->seek(position);

  int32_t weight = 1;
  int32_t result = 0;
  for (; nBytes; nBytes--)
  {
    result += weight * p_file->read();
    weight <<= 8;
  }
  return result;
}

/**
* @brief This function reads the RGB565 colour of the first pixel for a
         given the logo number. The pagenumber is global.
*
* @param *filename const char
*
* @return uint16_t
*
* @note Uses readNbytesInt
*/
uint16_t getBMPColor(const char *filename)
{

  // Open File
  File bmpImage = SPIFFS.open(filename, FILE_READ);

  int32_t dataStartingOffset = readNbytesInt(&bmpImage, 0x0A, 4);
  int16_t pixelsize = readNbytesInt(&bmpImage, 0x1C, 2);

  if (pixelsize != 24)
  {
    Serial.println("[WARNING]: getBMPColor: Image is not 24 bpp");
    return 0x0000;
  }

  bmpImage.seek(dataStartingOffset); //skip bitmap header

  byte R, G, B;

  B = bmpImage.read();
  G = bmpImage.read();
  R = bmpImage.read();

  bmpImage.close();

  return tft.color565(R, G, B);
}

/**
* @brief This function returns the RGB565 colour of the first pixel for a
         given the logo number. The pagenumber is global.
*
* @param logonumber int
*
* @return uint16_t
*
* @note Uses getBMPColor to read the actual image data.
*/
uint16_t getImageBG(int logonumber)
{

  // Logo 5 on each screen is the back home button except on the home screen
  if (logonumber == 5 && pageNum > 0)
   {
      return getBMPColor("/logos/home.bmp");
   }
   else
   {
    if (pageNum < 6)
    {
      if (logonumber < 6)
      {
        return getBMPColor(screen[pageNum].logo[logonumber]);
      }
    }
  }

  return 0x0000;
}

/**
* @brief This function returns the RGB565 colour of the first pixel of the image which
*          is being latched to for a given the logo number. The pagenumber is global.
*
* @param logonumber int
*
* @return uint16_t
*
* @note Uses getBMPColor to read the actual image data.
*/
uint16_t getLatchImageBG(int logonumber)
{
  if (pageNum < 6 && logonumber < 5)
  {
    if (strcmp(menu[pageNum-1].button[logonumber].latchlogo, "/logos/") == 0)
    {
      return getBMPColor(screen[pageNum].logo[logonumber]);
    }
    return getBMPColor(menu[pageNum-1].button[logonumber].latchlogo);
  }

  return 0x0000;
}