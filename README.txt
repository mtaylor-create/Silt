Hello!

Silt is an application for the Sifteo system that allows you to view and explore large images through tiny windows.  Tilt a tile to pan around the image pixel-by-pixel.  Touch the screen of one cube and then place it next to another to link them together.  The more cubes you use, the bigger the area you can see at once!

I'm still working out a few bugs, including how to export the processing sketch into an application that will still allow gviewer.command to run, so pardon the mess.  Expect more features to come, including support for images at least four times larger than currently allowed.  If you're trying to use this as-is, here is an important note about setting up the software:

VERY IMPORTANT:
I don't want to make any part of Sifteo's SDK publicly available without their explicit consent, so head over to Sifteo's website, make an account, and download their SDK.  Out of the SDK, you'll need to copy a few files and folders and drop them in /data/SDK.  These are:

Folders:
bin
include

Files:
Makefile.defs
Makefile.rules

Otherwise, gviewer.command isn't going to make heads or tails of what it's being asked to do, so it's no use trying to run this prior to doing that.  

If you have any questions or comments, feel free to email me at mdtaylor@cmu.edu