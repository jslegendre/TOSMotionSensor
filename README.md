##TOSMotionSensor
TOSMotionSensor is an accelerometer driver for Toshiba hackintosh devices.  It has been designed to use the built-in accelerometer in any Toshiba device sporting one and should be compatible with any supported device. 

TOSMotionSensor also kinda-sorta emulates the built in accelerometer HDD MacBook and MacBook Pros used to come with by publishing the same IOService and can be queried by user-space applications in the same way.

tldr: This is compatible with native macOS applications that use SMS (SMCMotionSensor/Sudden Motion Sensor) data. No proprietary software/patches/hacks necessary.

**NOTE: This driver does NOT offer HDD protection. A lot more testing and research needs to be done in order for that to happen.** I did not include this in the first release as I don't feel this is as important as it used to be. Most of us are using solid state storage these days. Would still be cool to get working though.

## Usage
I have tested this kext on High Sierra and Mojave in Clover's 'kexts/Other', SLE, and LE. All seem to be working.

If you do not want to build it yourself you can **[Download the latest release here](https://github.com/jslegendre/TOSMotionSensor)**

## Development
If you would like to get started developing motion-based applications you can check out [SMSLib](http://suitable.com/smslib.html), a library made especially for grabbing accelerometer data on Apple hardware or check out my own display autorotation app [Displace](https://github.com/jslegendre/Displace) for a real-world example. 

## Future 
I am really excited about iOS apps (supposedly) coming to macOS and the possibilities this opens up. Possible uses include:

- Gaming. Playing motion-based iOS games "natively" on a bigger screen. 
-  Testing for iOS motion-based app developers.

## Contributing
I am very open to pull requests.  This is my first kext release and I'm sure that I have some areas to improve upon.

- Development. Check out the source, there are comments, questions, and todos all over the place.
- Testing. If you have a Toshiba device with an accelerometer, please try this out alongside [Displace](https://github.com/jslegendre/Displace) and let me know how it worked for you.
- Financially. Yeeeeessshh, okay, this is where I get uncomfortable. This and other projects take time and something I've come to learn since having a child is that time isn't free. Especially (and ironically) free time. The only thing I have up right now is a [Patreon](https://www.patreon.com/jslegendre) account if you are interested. \*cue Sarah McLachlan* Even $1/month is a huge help and allows me to dedicate more time to Hackintosh projects as well as access to more devices to make drivers for. 

I cannot thank you enough for even reading this far let alone helping out in any way you can.