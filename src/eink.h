#include <Arduino.h>
#include "globals.h"
#include "graphics.h"
#include <GxEPD2_3C.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/Picopixel.h>
#include <Fonts/TomThumb.h>

#define MAX_DISPLAY_BUFFER_SIZE 800
#define MAX_HEIGHT_3C(EPD) (EPD::HEIGHT <= (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8) ? EPD::HEIGHT : (MAX_DISPLAY_BUFFER_SIZE / 2) / (EPD::WIDTH / 8))
// 296 × 128
GxEPD2_3C<GxEPD2_290c, MAX_HEIGHT_3C(GxEPD2_290c)> display(GxEPD2_290c(/*CS=*/5, /*DC=*/2, /*RST=*/3, /*BUSY=*/4)); // GDEW029Z10 128x296,

String row1, row2, row3, row4, row5, row6;
String row[] = {
    row1,
    row2,
    row3,
    row4,
    row5,
    row6,
};
String avActivityHighlights = "";

String nextWord(String x)
{
  return x.substring(0, x.indexOf(' '));
}

void formatString()
{
  for (int i = 0; i < 6; i++)
  {
    row[i] = "";
  }

  // Text linebreak - only whole words not only characters
  int zeilenlaenge = 35;
  for (int i = 0; i < 6; i++)
  {
    while (row[i].length() + nextWord(avActivityHighlights).length() < zeilenlaenge)
    {
      row[i] = row[i] + " " + nextWord(avActivityHighlights);
      avActivityHighlights = avActivityHighlights.substring(nextWord(avActivityHighlights).length() + 1, avActivityHighlights.length());
    }
  }
}

void showAvProblemImage(String pic, bool firstRow)
{

  const unsigned char *selectedBitmap;
  const unsigned char *selectedBitmap_red;

  int x_eink = 10;
  int y_eink = 10;

  if (pic == "old_snow")
  {
    selectedBitmap = epd_bitmap_persistent_weak_layers;
    selectedBitmap_red = epd_bitmap_persistent_weak_layers_red;
  }
  else if (pic == "new_snow")
  {
    selectedBitmap = epd_bitmap_new_snow;
    selectedBitmap_red = epd_bitmap_new_snow_red;
  }
  else if (pic == "wind_slab")
  {
    selectedBitmap = epd_bitmap_wind_slab;
    selectedBitmap_red = epd_bitmap_wind_slab_red;
  }
  else if (pic == "wet_snow")
  {
    selectedBitmap = epd_bitmap_wet_snow;
    selectedBitmap_red = epd_bitmap_wet_snow_red;
  }
  else if (pic == "gliding_snow")
  {
    selectedBitmap = epd_bitmap_gliding_snow;
    selectedBitmap_red = epd_bitmap_gliding_snow_red;
  }
  else if (pic == "persistent_weak_layers"){
    selectedBitmap = epd_bitmap_persistent_weak_layers;
    selectedBitmap_red = epd_bitmap_persistent_weak_layers_red;
  }
  else
  {
    selectedBitmap = epd_bitmap_persistent_weak_layers;
    selectedBitmap_red = epd_bitmap_persistent_weak_layers_red;
  }

  if (!firstRow)
  {
    x_eink = 70;
    y_eink = 70;
  }

  display.drawInvertedBitmap(x_eink, 1, selectedBitmap, 50, 50, GxEPD_BLACK);
  display.drawInvertedBitmap(y_eink, 1, selectedBitmap_red, 50, 50, GxEPD_RED);
}

void showAvalancheSize()
{
  display.setTextColor(GxEPD_RED);
  //180 mittig
  display.setCursor(180, 55);
  display.setFont(&FreeMonoBold24pt7b);
  display.print(avalancheData.avalancheSize);
}

void showAVProblemText()
{
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(10, 60);
  display.setFont(&TomThumb);
  display.print(avalancheData.avproblem0);

  display.setCursor(70, 60);
  display.setFont(&TomThumb);
  display.print(avalancheData.avproblem1);
}


void showRegion()
{
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(125, 15);
  display.setFont(&FreeSans9pt7b);
  display.print(avalancheData.region);
}

void showMetaDataTime()
{
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(10, 120);
  display.setFont(&TomThumb);
  display.print(avalancheData.publicationTime);
}

void showDangerPattern()
{
  display.setTextColor(GxEPD_BLACK);
  display.setCursor(80, 120);
  display.setFont(&TomThumb);
  display.print(avalancheData.dangerpattern0);

}

void showavActivityHighlights()
{
  avActivityHighlights = avalancheData.activityhighlights;

  formatString();
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeSans9pt7b);

  // Alle Zeilen ausgeben
  for (int i = 0; i < 6; i++)
  {
    display.setCursor(5, (i * 15) + 75);
    display.print(row[i]);
  }
}

void showAvalancheDataOnDisplay()
{
  Serial.println("Start updating E-ink");
  display.setRotation(1);

  display.setFullWindow();
  display.clearScreen();

  display.firstPage();
  do
  {
    showAvalancheSize();
    showMetaDataTime();
    showavActivityHighlights();
    showRegion();
    showDangerPattern();
    showAVProblemText();
    delay(100);
    showAvProblemImage(avalancheData.avproblem0, true);
    delay(100);
    showAvProblemImage(avalancheData.avproblem1, false);

  } while (display.nextPage());

  Serial.println("E-ink finished");
  state=WAIT;
}