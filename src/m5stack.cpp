/*
 *   Copyright (C) 2022 by Jim KI6ZUM, Lauren KJ6KDZ
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <Arduino.h>
#include <M5Unified.h>
#include "bitmaps.h"
#include "nextion.h"

element * currentElement;
char savedIP[100];
char savedRX[100];
char savedTX[100];

void screenInit(void)
{
  M5.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.drawBitmap(0,0,320, 240, (uint16_t *)idle);
  currentElement = pageMMDVM;
}

void displayString(element * page, uint8_t * dest, uint8_t * output)
{
  char text[100];
  int index=0;

  while (strcmp(page[index].name, "") !=0 ) {
    if (strcmp((const char *) page[index].name, (const char *) dest)==0) {

      M5.Lcd.setTextColor(page[index].color_font, page[index].color_background);
      M5.Lcd.setFreeFont(page[index].font);
      M5.Lcd.setTextPadding(page[index].max_width);

      strcpy(text, (const char *) output);
      int i=strlen(text);
      while ((M5.Lcd.textWidth(text) > page[index].max_width) && (i>0)) {
        if (M5.Lcd.textWidth(text) > page[index].max_width) {
          i-=1;
          text[i] = 0;
        }
        else 
          break;
      }
      M5.Lcd.drawString(text, page[index].x, page[index].y, GFXFF);
    } // end if page name match found
    index++;
  } // end while page array element not empty
}

// each time a "page" message is received, repaint the background and populate 
// the tx/rx frequency and IP address if needed
void changePage(uint8_t * newPage)
{

  //Serial.println((const char *)newPage);
  if (strstr((const char *)newPage, "MMDVM") != 0) {
    M5.Lcd.drawBitmap(0,0,320, 240, (uint16_t *)idle);
    currentElement = pageMMDVM;
  }
  else if (strstr((const char *)newPage, "DMR") != 0) {
    M5.Lcd.drawBitmap(0,0,320, 240, (uint16_t *)dmr);
    currentElement = pageDMR;
    displayString(currentElement, (uint8_t *)"tx", (uint8_t *)savedTX); // tx
    displayString(currentElement, (uint8_t *)"rx", (uint8_t *)savedRX); // tx
    displayString(currentElement, (uint8_t *)"ip", (uint8_t *)savedIP); // ip
  }
  else if (strstr((const char *)newPage, "DSTAR") != 0) {
    M5.Lcd.drawBitmap(0,0,320, 240, (uint16_t *)dstar);
    currentElement = pageDSTAR;
    displayString(currentElement, (uint8_t *)"tx", (uint8_t *)savedTX); // tx
    displayString(currentElement, (uint8_t *)"rx", (uint8_t *)savedRX); // tx
    displayString(currentElement, (uint8_t *)"ip", (uint8_t *)savedIP); // ip
  }
  else if (strstr((const char *)newPage, "YSF") != 0) {
    M5.Lcd.drawBitmap(0,0,320, 240, (uint16_t *)ysf);
    currentElement = pageYSF;
    displayString(currentElement, (uint8_t *)"tx", (uint8_t *)savedTX); // tx
    displayString(currentElement, (uint8_t *)"rx", (uint8_t *)savedRX); // tx
    displayString(currentElement, (uint8_t *)"ip", (uint8_t *)savedIP); // ip
  }
  else if (strstr((const char *)newPage, "NXDN") != 0) {
    M5.Lcd.drawBitmap(0,0,320, 240, (uint16_t *)nxdn);
    currentElement = pageNXDN;
    displayString(currentElement, (uint8_t *)"tx", (uint8_t *)savedTX); // tx
    displayString(currentElement, (uint8_t *)"rx", (uint8_t *)savedRX); // tx
    displayString(currentElement, (uint8_t *)"ip", (uint8_t *)savedIP); // ip
  }
  else if (strstr((const char *)newPage, "M17") != 0) {
    M5.Lcd.drawBitmap(0,0,320, 240, (uint16_t *)m17);
    displayString(currentElement, (uint8_t *)"tx", (uint8_t *)savedTX); // tx
    displayString(currentElement, (uint8_t *)"rx", (uint8_t *)savedRX); // tx
    displayString(currentElement, (uint8_t *)"ip", (uint8_t *)savedIP); // ip
    currentElement = pageM17;
  }
  else if (strstr((const char *)newPage, "POCSAG") != 0) {
    M5.Lcd.drawBitmap(0,0,320, 240, (uint16_t *)pocsag);
    currentElement = pagePOCSAG;
  }
  else 
  {
    //Serial.println("no page found");
  }
}

void testCountries(void)
{

      // calculate the country from the callsign
    M5.Lcd.drawBitmap(0,0,320, 240, (uint16_t *)dmr);
    currentElement = pageDMR;

    int index=0;
    while (strcmp(countries[index].prefix1, "") !=0 ) {
      if (countries[index].bitmap != flagBlank)
      {
        //Serial.println(countries[index].name);
        displayString(currentElement, (uint8_t *)"country", (uint8_t *)countries[index].name); // country
        M5.Lcd.drawBitmap(5, 6, 120, 60, (uint16_t *)countries[index].bitmap);
        delay(1000);
      }
      index++;
    }

    M5.Lcd.drawBitmap(0,0,320, 240, (uint16_t *)idle);
    currentElement = pageMMDVM;

}


void displayField(uint8_t * newPage)
{
  char dest[100], output[100];
  // find the '.' character to figure out what the destination field is named
  strcpy(dest, (const char *)newPage);
  int i;
  for(i = 0; i <= strlen((const char *)dest); i++) {
  		if(dest[i] == '.') {
  			dest[i] = 0;
			  break;    	
 		  }  
	  }

  // remove the quote characters  
  if ((newPage[i+5]=='\"') && (newPage[i+6]=='\"')){
    output[0] = 0;
  }
  else {
    strcpy(output, (const char *)newPage +i+6);
    output[strlen(output)-1]=0;
  }

  // remove the 2/2 N characters in callsign fields
  char temp[100];
  if (strncmp(output, "2 ", 2) == 0) { // string prefaced by "2 " is likely to be the callsign field
    if (strncmp(output, "2 N ", 4) == 0) {
      strcpy(temp, output+4);
      strcpy(output, temp);
    }
    else if (strncmp(output, "2 Listening", 11) == 0) {
      strcpy(temp, output+2);
      strcpy(output, temp);
    }

    // calculate the country from the callsign
    int index=0;
    while (strcmp(countries[index].prefix1, "") !=0 ) {
      if ((strcmp((const char *) countries[index].prefix1, (const char *) output)<=0) &&
          (strcmp((const char *) countries[index].prefix2, (const char *) output)>=0) )
      {
        // found it
        //Serial.println(countries[index].name);
        displayString(currentElement, (uint8_t *)"country", (uint8_t *)countries[index].name); // country
        M5.Lcd.drawBitmap(5, 6, 120, 60, (uint16_t *)countries[index].bitmap);
        break;
      }
      index++;
    }
    if (strcmp(countries[index].prefix1, "") ==0 )
     {
        // country not found from call sign
        //Serial.println("not found");
        displayString(currentElement, (uint8_t *)"country", (uint8_t *)" "); // country
        M5.Lcd.drawBitmap(5, 6, 120, 60, (uint16_t *)flagBlank);
     }
  } // end if "2 " string test

  displayString(currentElement, (uint8_t *)dest, (uint8_t *)output);

  // save a copy of the rx/tx/ip fields when on the idle page
  if (currentElement==pageMMDVM) {

    // t30 - rx
    if (strncmp(dest, "t30", 4) == 0) {
      strcpy(savedRX, output);
    }
    // t32 - tx
    else if (strncmp(dest, "t32", 4) == 0) {
      strcpy(savedTX, output);
    }
    // t3 - ip address
    else if (strncmp(dest, "t3", 3) == 0) {
      strcpy(savedIP, output);
    }
  }

}

// this is sample code that tests printing text into each field on each screen to check the
// color, position and size to make sure it fits within the space 
void testModes(void) {
// sendCommand("page MMDVM");
  M5.Lcd.drawBitmap(0,0,320, 240, (uint16_t *)idle);
  displayString(pageMMDVM, (uint8_t *)"t0", (uint8_t *)"N0CALL/1234567 this is a really long string");
  displayString(pageMMDVM, (uint8_t *)"t1", (uint8_t *)"STATUS");
  displayString(pageMMDVM, (uint8_t *)"t2", (uint8_t *)"12/12/20 13:37:54");
  displayString(pageMMDVM, (uint8_t *)"t3", (uint8_t *)"wlan0:192.168.1.240"); // ip
  displayString(pageMMDVM, (uint8_t *)"t30", (uint8_t *)"434.6000"); // rx
  displayString(pageMMDVM, (uint8_t *)"t32", (uint8_t *)"434.6000"); // tx
  delay(5000);

// sendCommand("page DMR");
  M5.Lcd.drawBitmap(0,0,320, 240, (uint16_t *)dmr);
  displayString(pageDMR, (uint8_t *)"t2", (uint8_t *)"N0CALL/1234567 this is a really long string");
  displayString(pageDMR, (uint8_t *)"t3", (uint8_t *)"TG91"); // Talkgroup
  displayString(pageDMR, (uint8_t *)"t5", (uint8_t *)"12.25%"); // RSSI
  displayString(pageDMR, (uint8_t *)"t7", (uint8_t *)"12.25%"); // BER
  displayString(pageDMR, (uint8_t *)"tx", (uint8_t *)"434.6000"); // tx
  displayString(pageDMR, (uint8_t *)"rx", (uint8_t *)"434.6000"); // tx
  displayString(pageDMR, (uint8_t *)"ip", (uint8_t *)"wlan0:192.168.1.240"); // ip
  displayString(pageDMR, (uint8_t *)"time", (uint8_t *)"3:30"); // time
  displayString(pageDMR, (uint8_t *)"country", (uint8_t *)"Saint Helena, Ascension and Tristan"); // country
  displayString(pageDMR, (uint8_t *)"onair", (uint8_t *)"ON AIR"); // onair
  // display the 120x60 flag bitmap  
  M5.Lcd.drawBitmap(5, 6, 120, 60, (uint16_t *)flagAU);
  delay(5000);

// sendCommand("page DSTAR");
  M5.Lcd.drawBitmap(0,0,320, 240, (uint16_t *)dstar);
  displayString(pageDSTAR, (uint8_t *)"t0", (uint8_t *)"N0CALL/1234567 this is a really long string");
  displayString(pageDSTAR, (uint8_t *)"t1", (uint8_t *)"REF012AL"); // YR
  displayString(pageDSTAR, (uint8_t *)"t2", (uint8_t *)"5100"); // RT
  displayString(pageDSTAR, (uint8_t *)"t3", (uint8_t *)"12.25%"); // RSSI
  displayString(pageDSTAR, (uint8_t *)"t4", (uint8_t *)"12.25%"); // BER
  displayString(pageDSTAR, (uint8_t *)"tx", (uint8_t *)"434.6000"); // tx
  displayString(pageDSTAR, (uint8_t *)"rx", (uint8_t *)"434.6000"); // tx
  displayString(pageDSTAR, (uint8_t *)"ip", (uint8_t *)"wlan0:192.168.1.240"); // ip
  displayString(pageDSTAR, (uint8_t *)"time", (uint8_t *)"3:30"); // time
  displayString(pageDSTAR, (uint8_t *)"country", (uint8_t *)"Saint Helena, Ascension and Tristan"); // country
  displayString(pageDSTAR, (uint8_t *)"onair", (uint8_t *)"ON AIR"); // onair
  // display the 120x60 flag bitmap  
  M5.Lcd.drawBitmap(5, 6, 120, 60, (uint16_t *)flagCA);
  delay(5000);

// sendCommand("page YSF");
  M5.Lcd.drawBitmap(0,0,320, 240, (uint16_t *)ysf);
  displayString(pageYSF, (uint8_t *)"t0", (uint8_t *)"N0CALL/1234567 this is a really long string");
  displayString(pageYSF, (uint8_t *)"t1", (uint8_t *)"DST"); // DST
  displayString(pageYSF, (uint8_t *)"t2", (uint8_t *)"SRC"); // SRC
  displayString(pageYSF, (uint8_t *)"t3", (uint8_t *)"12.25%"); // RSSI
  displayString(pageYSF, (uint8_t *)"t4", (uint8_t *)"12.25%"); // BER
  displayString(pageYSF, (uint8_t *)"tx", (uint8_t *)"434.6000"); // tx
  displayString(pageYSF, (uint8_t *)"rx", (uint8_t *)"434.6000"); // tx
  displayString(pageYSF, (uint8_t *)"ip", (uint8_t *)"wlan0:192.168.1.240"); // ip
  displayString(pageYSF, (uint8_t *)"time", (uint8_t *)"3:30"); // time
  displayString(pageYSF, (uint8_t *)"country", (uint8_t *)"Saint Helena, Ascension and Tristan"); // country
  displayString(pageYSF, (uint8_t *)"onair", (uint8_t *)"ON AIR"); // onair
  // display the 120x60 flag bitmap  
  M5.Lcd.drawBitmap(5, 6, 120, 60, (uint16_t *)flagAT);
  delay(5000);

// sendCommand("page P25");
  M5.Lcd.drawBitmap(0,0,320, 240, (uint16_t *)p25);
  displayString(pageP25, (uint8_t *)"t0", (uint8_t *)"N0CALL/1234567 this is a really long string");
  displayString(pageP25, (uint8_t *)"t1", (uint8_t *)"DST"); // DST
  displayString(pageP25, (uint8_t *)"t2", (uint8_t *)"12.25%"); // RSSI
  displayString(pageP25, (uint8_t *)"t3", (uint8_t *)"12.25%"); // BER
  displayString(pageP25, (uint8_t *)"tx", (uint8_t *)"434.6000"); // tx
  displayString(pageP25, (uint8_t *)"rx", (uint8_t *)"434.6000"); // tx
  displayString(pageP25, (uint8_t *)"ip", (uint8_t *)"wlan0:192.168.1.240"); // ip
  displayString(pageP25, (uint8_t *)"time", (uint8_t *)"3:30"); // time
  displayString(pageP25, (uint8_t *)"country", (uint8_t *)"Saint Helena, Ascension and Tristan"); // country
  displayString(pageP25, (uint8_t *)"onair", (uint8_t *)"ON AIR"); // onair
  // display the 120x60 flag bitmap  
  M5.Lcd.drawBitmap(5, 6, 120, 60, (uint16_t *)flagNZ);
  delay(5000);

// sendCommand("page NXDN");
  M5.Lcd.drawBitmap(0,0,320, 240, (uint16_t *)nxdn);
  displayString(pageNXDN, (uint8_t *)"t0", (uint8_t *)"N0CALL/1234567 this is a really long string");
  displayString(pageNXDN, (uint8_t *)"t1", (uint8_t *)"DST"); // DST
  displayString(pageNXDN, (uint8_t *)"t2", (uint8_t *)"12.25%"); // RSSI
  displayString(pageNXDN, (uint8_t *)"t3", (uint8_t *)"12.25%"); // BER
  displayString(pageNXDN, (uint8_t *)"tx", (uint8_t *)"434.6000"); // tx
  displayString(pageNXDN, (uint8_t *)"rx", (uint8_t *)"434.6000"); // tx
  displayString(pageNXDN, (uint8_t *)"ip", (uint8_t *)"wlan0:192.168.1.240"); // ip
  displayString(pageNXDN, (uint8_t *)"time", (uint8_t *)"3:30"); // time
  displayString(pageNXDN, (uint8_t *)"country", (uint8_t *)"Saint Helena, Ascension and Tristan"); // country
  displayString(pageNXDN, (uint8_t *)"onair", (uint8_t *)"ON AIR"); // onair
  // display the 120x60 flag bitmap  
  M5.Lcd.drawBitmap(5, 6, 120, 60, (uint16_t *)flagDE);
  delay(5000);

// sendCommand("page M17");
  M5.Lcd.drawBitmap(0,0,320, 240, (uint16_t *)m17);
  displayString(pageM17, (uint8_t *)"t0", (uint8_t *)"N0CALL/1234567 this is a really long string");
  displayString(pageM17, (uint8_t *)"t1", (uint8_t *)"DST"); // DST
  displayString(pageM17, (uint8_t *)"t2", (uint8_t *)"12.25%"); // RSSI
  displayString(pageM17, (uint8_t *)"t3", (uint8_t *)"12.25%"); // BER
  displayString(pageM17, (uint8_t *)"tx", (uint8_t *)"434.6000"); // tx
  displayString(pageM17, (uint8_t *)"rx", (uint8_t *)"434.6000"); // tx
  displayString(pageM17, (uint8_t *)"ip", (uint8_t *)"wlan0:192.168.1.240"); // ip
  displayString(pageM17, (uint8_t *)"time", (uint8_t *)"3:30"); // time
  displayString(pageM17, (uint8_t *)"country", (uint8_t *)"Saint Helena, Ascension and Tristan"); // country
  displayString(pageM17, (uint8_t *)"onair", (uint8_t *)"ON AIR"); // onair
  // display the 120x60 flag bitmap  
  M5.Lcd.drawBitmap(5, 6, 120, 60, (uint16_t *)flagFR);
  delay(5000);

// sendCommand("page POCSAG");
  M5.Lcd.drawBitmap(0,0,320, 240, (uint16_t *)pocsag);
  displayString(pagePOCSAG, (uint8_t *)"t0", (uint8_t *)"RIC: 012345678"); // RIC
  displayString(pagePOCSAG, (uint8_t *)"t1", (uint8_t *)"this is a sample POCSAG message"); // message
  displayString(pagePOCSAG, (uint8_t *)"tx", (uint8_t *)"434.6000"); // tx
  displayString(pagePOCSAG, (uint8_t *)"ip", (uint8_t *)"wlan0:192.168.1.240"); // ip
  displayString(pagePOCSAG, (uint8_t *)"time", (uint8_t *)"13:37:54"); // time
  delay(5000);
}
