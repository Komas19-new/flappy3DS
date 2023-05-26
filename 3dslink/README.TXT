3dslink is a simple and effective method to transfer homebrew games and
applications to your 3DS over wifi. Simply run 3dslink.3dsx on your 3DS
console and use the command line 3dslink host tool to send a 3dsx file.

	3dslink [-a ip address] [-0 argv0] <ndsfile> [arguments]

The 3dslink host uses UDP broadcast messages to discover your 3DS but
this won't work for some people due to routers either not passing on
these packets or sending them at a speed the 3DS can't read.

If you get "No Response from 3DS!" then try specifiying the ip address
of your 3DS with the -a switch.

unrecognised arguments at the end of the command line will be passed on
to the launched 3dsx. Use -0 to set argv[0] if, for instance, you'd like
to set the cwd for the application. use -0 sdmc:/3ds/ to set the cwd to
the 3ds folder in the root of your card.

Copyright (c) 2015 Dave Murphy (WinterMute)

Any redistribution or reproduction of part or all this software in any
form is prohibited other than the following:

You may make as many copies of the software as you require for
your own personal use.

You may copy the complete, unmodified archive to individual third parties
for their personal use, but may not place it on websites or torrent trackers.

You may not, except with my express written permission, distribute or
commercially exploit the software. Nor may you transmit it or store it
in any other website.

The Software shall be used for Good, not Evil.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
