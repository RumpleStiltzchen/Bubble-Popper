# Bubble-Popper <br>

==========================================================================<br>
Setup:<br>
==========================================================================<br>
Launcher.exe and options.txt need to be in the same folder along with the popper folder. Options.txt works like this: <br>
<br>
Line 1 is ignored.<br>
<br>
Line 2 holds the FULL COMPLETE ABSOLUTE path to Bubble popper INCLUDING THE FILE ITSELF SURROUNDED BY QUOTATIONS ("Ex: C:/BubblePopper/bubblepopper.exe").<br>
<br>
Line 3 holds the directory to the icon for the system tray. Same restrictions as Line 2. Must include ico file in path, must be surrounded by quotes, and must be the absolute path.<br>
<br>
Line 4 holds the activation key code in decimal. By default it is set to PAGE DOWN<br>
	a list of keycodes can be found here. convert to decimal before use.<br>
	https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes <br>
<br>
Line 5 is either "true" or "false" (remove quotations) and controls if music should be played.<br>
<br>
Line 6 Your monitor's width in pixels (x resolution)<br>
<br>
Line 7 Your monitor's height in pixels (y resolution)<br>
<br>
==========================================================================br>
Usage:<br>
==========================================================================br>
<br>
By default, the activation key is "PAGE DOWN" by default. This can be changed in options.txt. The activation key toggles Bubble Popper. To terminate the Launcher program, simply open the system tray, and right click the icon. <br>
<br>
==========================================================================br>
Build:<br>
==========================================================================br>
<br>
mingw:<br>
  run build.bat<br>
anything else:<br>
  good luck<br>


