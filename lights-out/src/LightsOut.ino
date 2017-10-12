/*************************************************** 
  This is a test example for the Adafruit Trellis w/HT16K33

  Designed specifically to work with the Adafruit Trellis 
  ----> https://www.adafruit.com/products/1616
  ----> https://www.adafruit.com/products/1611

  These displays use I2C to communicate, 2 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h>
#include "Adafruit_Trellis.h"

#define DEBUG

/*************************************************** 
  This example shows reading buttons and setting/clearing buttons in a loop
  "momentary" mode has the LED light up only when a button is pressed
  "latching" mode lets you turn the LED on/off when pressed

  Up to 8 matrices can be used but this example will show 4 or 1
 ****************************************************/

#define MOMENTARY 0
#define LATCHING 1
// set the mode here
#define MODE LATCHING

#define NCOLS 4
#define NROWS 8

Adafruit_Trellis matrix0 = Adafruit_Trellis();

// uncomment the below to add 3 more matrices

Adafruit_Trellis matrix1 = Adafruit_Trellis();
//Adafruit_Trellis matrix2 = Adafruit_Trellis();
//Adafruit_Trellis matrix3 = Adafruit_Trellis();
// you can add another 4, up to 8


// Just one
//Adafruit_TrellisSet trellis =  Adafruit_TrellisSet(&matrix0);
// or use the below to select 4, up to 8 can be passed in
Adafruit_TrellisSet trellis =  Adafruit_TrellisSet(&matrix0, &matrix1);

// set to however many you're working with here, up to 8
#define NUMTRELLIS 2

#define numKeys (NROWS*NCOLS)
#define RESET_HOLD_TIME 5000

// Connect Trellis Vin to 5V and Ground to ground.
// Connect the INT wire to pin #A2 (can change later!)
#define INTPIN A2
// Connect I2C SDA pin to your Arduino SDA line
// Connect I2C SCL pin to your Arduino SCL line
// All Trellises share the SDA, SCL and INT pin! 
// Even 8 tiles use only 3 wires max

byte mask[10][28] ={{1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1},   // zero
					{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},   // one
					{1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1},   // two
					{1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1},   // three
					{1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},   // four
					{1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1},   // five
					{1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1},   // six
					{1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},   // seven
					{1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1},   // eight
					{1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1}};   // nine
				
byte current[NROWS*NCOLS];
	
int buttonPressed = -1;
int matrix[NROWS][NCOLS];
int k = 0;
boolean done = false;


void setup() {
  Serial.begin(9600);
  Serial.println("Trellis");
  
for (int i=0; i<NROWS; i++)
{
	for (int j=0; j<NCOLS; j++)
	{
		matrix[i][j] = 0;
		current[k] = 0;
		k++;
	}
}


  // INT pin requires a pullup
  pinMode(INTPIN, INPUT);
  digitalWrite(INTPIN, HIGH);
  
  // begin() with the addresses of each panel in order
  // I find it easiest if the addresses are in order
 // trellis.begin(0x70);  // only one
  trellis.begin(0x70, 0x71);  // or four!

  // light up all the LEDs in order
  for (uint8_t i=0; i<numKeys; i++) {
    trellis.setLED(i);
    trellis.writeDisplay();    
    delay(50);
  }
  // then turn them off
  for (uint8_t i=0; i<numKeys; i++) {
    trellis.clrLED(i);
    trellis.writeDisplay();    
    delay(50);
  }
  
	randomSeed(analogRead(0));
	randomizeStart(5);
   trellis.writeDisplay();
}


void loop()
{
	delay(30); // 30ms delay is required, dont remove me!
	buttonPressed = checkSwitches();
  
	if (buttonPressed >= 0)
	{
		setLEDs(buttonPressed);
		// tell the trellis to set the LEDs we requested
		trellis.writeDisplay();
		done = checkcomplete();
		if(done)
		{
			Serial.println("done!");
			setup();
		}
	}
	checkReset();
}

void lineFill()
{
	  // light up all the LEDs in order
  for (uint8_t i=0; i<numKeys; i++) {
    if (current[i] == 1)
	{
	trellis.setLED(i);
    trellis.writeDisplay();    
    delay(50);
	}
  }
  // then turn them off
  for (uint8_t i=0; i<numKeys; i++) {
    if (current[i] == 1)
	{
	trellis.clrLED(i);
    trellis.writeDisplay();    
    delay(50);
	}
  }
	
}

void reverseFlash()
{
	for (uint8_t i=0; i<numKeys; i++)
	{
		if (trellis.isLED(i))
		{
			trellis.clrLED(i);
		}
		else
		{
			trellis.setLED(i);
		}
	}
	trellis.writeDisplay();    
}

void flash()
{
	for (uint8_t i=0; i<numKeys; i++)
	{
		trellis.setLED(i);
	}
	trellis.writeDisplay();    
	delay(250);
	
  // then turn them off
  for (uint8_t i=0; i<numKeys; i++)
	{
		trellis.clrLED(i);
	}
    trellis.writeDisplay();    
    delay(250);
}

int checkSwitches()
{
			// If a button was just pressed or released...
	if (trellis.readSwitches())
	{
	  // go through every button
		for (uint8_t i=0; i<numKeys; i++)
		{
			// if it was pressed...
			if (trellis.justPressed(i))
			{
				#ifdef DEBUG
					Serial.println(i);
				#endif
				return i;
			}
		} 
	}
	return -1;
}

void setLEDs(int active)
{
	//toggle the LEDs and the adjacent LEDs, while also setting the appropriate matrix elements
	int row = active/NCOLS;
	int col = active%NCOLS;
			#ifdef DEBUG
				Serial.print("Row = ");
				Serial.print(row);
				Serial.print(", Column = ");
				Serial.println(col);
			#endif
			
	// first, toggle the pressed key
	if (current[active] == 0)
	{
		// turn on the center (pressed) location LED
		trellis.setLED(active);
		current[active] = 1;
		matrix[row][col] = 1;
	}
	else
	{
		// turn off the center (pressed) location LED
		trellis.clrLED(active);
		current[active] = 0;
		matrix[row][col] = 0;
	}
	// now, check the location above (row-1) and toggle
	if (row > 0)  //it's not the first row, so it's on the board
	{
		if (matrix[row-1][col]==0)  // it's off, so turn it on
		{
			matrix[row-1][col] = 1;
			current[active-NCOLS] = 1;
			trellis.setLED(active-NCOLS);
		}
		else  // it's on, so turn it off
		{
			matrix[row-1][col] = 0;
			current[active-NCOLS] = 0;
			trellis.clrLED(active-NCOLS);
		}	
	}
	// next, check the location below (row+1) and toggle
	if (row < NROWS-1)  //it's not the last row, so it's on the board
	{
		if (matrix[row+1][col]==0)  // it's off, so turn it on
		{
			matrix[row+1][col] = 1;
			current[active+NCOLS] = 1;
			trellis.setLED(active+NCOLS);
		}
		else  // it's on, so turn it off
		{
			matrix[row+1][col] = 0;
			current[active+NCOLS] = 0;
			trellis.clrLED(active+NCOLS);
		}	
	}
	// let's move on to the location on the left (col-1)
	if (col > 0)  //it's not the first col, so it's on the board
	{
		if (matrix[row][col-1]==0)  // it's off, so turn it on
		{
			matrix[row][col-1] = 1;
			current[active-1] = 1;
			trellis.setLED(active-1);
		}
		else  // it's on, so turn it off
		{
			matrix[row][col-1] = 0;
			current[active-1] = 0;
			trellis.clrLED(active-1);
		}	
	}
	// and finally, check the locationto the right (col+1) and toggle
	if (col < NCOLS-1)  //it's not the last column, so it's on the board
	{
		if (matrix[row][col+1]==0)  // it's off, so turn it on
		{
			matrix[row][col+1] = 1;
			current[active+1] = 1;
			trellis.setLED(active+1);
		}
		else  // it's on, so turn it off
		{
			matrix[row][col+1] = 0;
			current[active+1] = 0;
			trellis.clrLED(active+1);
		}	
	}
}

boolean checkcomplete()
{
	int k = 0;
	for (int i=0; i<NROWS; i++)
	{
		for (int j=0; j<NCOLS; j++)
		{
			if(matrix[i][j] != current[k])
			{
				Serial.println("Oops, the matrix and current arrays aren't equal.  Something isn't right.");
				break;
			}
			if(matrix[i][j] != 0)
			{
				return false;
			}
			k++;
		}
	}
	return true;
}

void randomizeStart(int numPoints)
{
	for (int i=0; i<numPoints; i++)
	{
		setLEDs(random(numKeys));
	}
}

void checkReset()
{
	static unsigned long ts;
		
	if (!(trellis.isKeyPressed(0))) //& trellis.isKeyPressed(3) & trellis.isKeyPressed(28) & trellis.isKeyPressed(31)))
	{
		ts = millis();
	}
	else
	{
		if (millis() > ts + RESET_HOLD_TIME)
		{
			while (trellis.isKeyPressed(0))
			{
				flash();
				checkSwitches();
			}
			ts = millis();
			setup();
		}
	}
}