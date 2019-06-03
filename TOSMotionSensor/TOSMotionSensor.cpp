/*
 * Copyright (c) 2019 jslegendre / Xord. All rights reserved.
 *
 *  Released under "The GNU General Public License (GPL-2.0)"
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

/*
 Credits:
    https://github.com/torvalds/linux/blob/master/drivers/platform/x86/toshiba_acpi.c
 */

#include "TOSMotionSensor.hpp"

#ifdef DEBUG
#define DPRINT(fmt, ...) \
IOLog("TOSMotionSensor::%s:%d(): " fmt, __func__, __LINE__, ##__VA_ARGS__);
#else
#define DPRINT(fmt, ...) do {} while (0)
#endif

#define HCI_GET                         0xfe00
#define HCI_ACCEL                       0x6d
#define HCI_ACCEL_SUPPORT               0xa6
#define HCI_ACCEL_MASK                  0x7fff
#define HCI_ACCEL_DIRECTION_MASK        0x8000
#define HCI_ACCEL_Y_SHIFT               0x10
#define HCI_UNSUPPORTED                 0x8000
#define HCI_SUCCESSS                    0x0

UInt32 getUnsignedIntFromArray(OSArray * array, UInt8 index);

#define super IOService
OSDefineMetaClassAndStructors(SMCMotionSensor, IOService)

bool TOSMotionSensor::init(OSDictionary *dict)
{
    DPRINT("Initializing\n");
    
    bool result = super::init(dict);
    TVAP = NULL;
    hci_method = NULL;
    _axes = { 0, 0, 0 };
    HCI_OUT = NULL;
    axes = NULL;
    
    for(UInt8 i = 0; i < 6; i++) {
        HCI_IN[i] = OSNumber::withNumber((unsigned long long)0, 32);
    }
    
    return result;
}

const OSSymbol * TOSMotionSensor::findHCI() {
    if(!TVAP->validateObject("SPFC")) {
        DPRINT("Found HCI method SPFC");
        return OSSymbol::withCString("SPFC");
    }
    
    if(!TVAP->validateObject("GHCI")) {
        DPRINT("Found HCI method GHCI");
        return OSSymbol::withCString("GHCI");
    }
    
    return NULL;
}

bool TOSMotionSensor::isAccelSupported() {
    HCI_IN[0] = OSDynamicCast(OSObject, OSNumber::withNumber(HCI_GET, 32));
    HCI_IN[1] = OSDynamicCast(OSObject, OSNumber::withNumber(HCI_ACCEL_SUPPORT, 32));
    
    TVAP->evaluateObject(hci_method, &HCI_OUT, HCI_IN, 6);
    
    if(HCI_OUT && (OSTypeIDInst(HCI_OUT) == OSTypeID(OSArray))) {
        if(getUnsignedIntFromArray(OSDynamicCast(OSArray, HCI_OUT), 0) != HCI_UNSUPPORTED) {
            DPRINT("Accelerometer supported");
            return true;
        }
    }
    
    return false;
}

bool TOSMotionSensor::start(IOService *provider)
{
    DPRINT("TOSMotionSensor::start: called\n");
    
    TVAP = OSDynamicCast(IOACPIPlatformDevice, provider);
    if (NULL == TVAP || !super::start(provider))
        return false;
    
    /*
     * Make sure the accelerometer is activated
     * Sidenote: Not sure if SSD only Toshiba machines have this device. If they do
     * not, how to make sure devices shipped with HDD have activated the sensor hub
     * before continuing?
     */
    IOService *haps_service = waitForMatchingService(serviceMatching("ToshibaHAPS"));
    if(!haps_service->getProperty("ProtectionLevel")) {
        DPRINT("Accelerometer not activated");
        haps_service->release();
        return false;
    }
    haps_service->release();
    
    hci_method = findHCI();
    if(!hci_method) {
        DPRINT("Could not find HCI method");
        return false;
    }
    
    if(!isAccelSupported()) {
        DPRINT("Accelerometer not supported");
        return false;
    }
    
    //Setup for HCI method to get accelerometer values as default
    HCI_IN[1] = OSNumber::withNumber(HCI_ACCEL, 32);
    HCI_IN[3] = OSNumber::withNumber(0x01, 32);

    registerService();
    return true;
}

//Should this be locking? Specifically access to _axes?
IOReturn TOSMotionSensor::getAxes(void *iBuf, void *oBuf) {
    DPRINT("getAxes Called");
    TVAP->evaluateObject(hci_method, &HCI_OUT, HCI_IN, 6);
    if(HCI_OUT && (OSTypeIDInst(HCI_OUT) == OSTypeID(OSArray))) {
        axes = OSDynamicCast(OSArray, HCI_OUT);
        if(getUnsignedIntFromArray(axes, 0) != (UInt32)HCI_SUCCESSS){  //One more check
            DPRINT("HCI method did not return with HCI_SUCCESS");
            return kIOReturnError;
        }
        _axes.z = (SInt16)getUnsignedIntFromArray(axes, 4);
        _axes.z = _axes.z & HCI_ACCEL_DIRECTION_MASK ?
        -(_axes.z & HCI_ACCEL_MASK) : _axes.z & HCI_ACCEL_MASK;
        
        UInt32 xy = getUnsignedIntFromArray(axes, 2);
        _axes.x = xy & HCI_ACCEL_DIRECTION_MASK ?
        -(xy & HCI_ACCEL_MASK) : xy & HCI_ACCEL_MASK;
        
        _axes.y = (xy >> HCI_ACCEL_Y_SHIFT) & HCI_ACCEL_DIRECTION_MASK ?
        -((xy >> HCI_ACCEL_Y_SHIFT) & HCI_ACCEL_MASK) :
        (xy >> HCI_ACCEL_Y_SHIFT) & HCI_ACCEL_MASK;
    }
    
    if(!HCI_OUT) {
        DPRINT("HCI_OUT is NULL");
        return kIOReturnError;
    }
    
    memcpy((char*)oBuf, &_axes, sizeof(struct s_axes));
    return kIOReturnSuccess;
}

void TOSMotionSensor::stop(IOService *provider)
{
    DPRINT("called\n");
    super::stop(provider);
}

UInt32 getUnsignedIntFromArray(OSArray * array, UInt8 index) {
    OSObject * obj = array->getObject(index);
    if(obj && (OSTypeIDInst(obj) == OSTypeID(OSNumber))) {
        OSNumber * num = OSDynamicCast(OSNumber, obj);
        return num->unsigned32BitValue();
    }
    
    return -1;
}
