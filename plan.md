# Compatibility with Haptic Display

Tasks for haptic display compatibility:

`TACTILE_DISPLAY_V1.ino` - file must be modified to have everything to interact with the hardware that the rest of the team is developing
1. `void loop()` - has to continuously check serial port for incoming messages.
	- Read the string until a newline character (`n`, and send it to `checkString()` method
2. `void checkString(String inputStr)` - checks the string to determine if it is a single character or a string
	- If the first character is a tilde (~), then the string being sent is a binary one for the entire display, sent to `binaryToWaveForm(String bString)` method.
		- The binary string generators on the webpages must all be adapted to send a tilde at the beginning of each string to accomodate this
		- The testing LED matrix display must also be adapted to fix this.
	- If the first character is not a tilde, then the string being sent is just one character, and it must be sent to the display.
		- Utilize the existing `TD.PLAY_CHAR` method to send a letter to the display
3. `binaryToWaveForm(String bString)` - iterates through the string with a number of rows and columns, a value of '1' in the char is on and a value of '0' in the char is off
	- ==Issue==: using an existing function like `TCA_and_PORT` will not work exactly because that segments the display into the 6 characters. Normally, this would work fine for text, but this leaves gaps between the 6 characters of unreachable piezos that we cannot enable.
	- Additional concern: efficiency of displaying an entire grid of pins at once, we want a very quick display and therefore the method for detecting the characters in the string and then lifting the piezo should be as optimized as possible