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

#include "ToshibaHAPS.hpp"

#ifdef DEBUG
#define DPRINT(fmt, ...) \
    IOLog("ToshibaHAPS::%s:%d(): " fmt, __func__, __LINE__, ##__VA_ARGS__);
#else
#define DPRINT(fmt, ...) do {} while (0)
#endif

#define super IOService
OSDefineMetaClassAndStructors(ToshibaHAPS, IOService);

IOReturn ToshibaHAPS::resetHaps() {
    UInt32 dummy; //Throwaway - Avoid NULL (de)referencing
  
    //ACPI method 'RSSS' does not return anything
    return haps->evaluateInteger(RSSS, &dummy);
}

IOReturn ToshibaHAPS::setCurrentPTLV(UInt32 level) {
    DPRINT("Setting protection level: %d", level);
    IOReturn ret = kIOReturnSuccess;
  
    /* Some models that ship with built in SSD have a HAPS device
       with no methods other than _STA (which unconditionally
       returns zero). In this case we set IORegistry protection level
       entry to 2 (default) return success, and move on. */
    ret = haps->validateObject(PTLV);
    if(ret != kIOReturnSuccess) {
      setProperty(ptlv_io_reg_property, OSNumber::withNumber(2, 32));
      return kIOReturnSuccess;
    }
  
    UInt32 dummy;
    OSObject* ptlv = OSDynamicCast(OSObject, OSNumber::withNumber(level, 32));
    
    //ACPI method 'PTLV' does not return anything
    ret = haps->evaluateInteger(PTLV, &dummy, &ptlv, 1);
    if(ret == kIOReturnSuccess) {
        DPRINT("Setting protection level successful");
        setProperty(ptlv_io_reg_property, ptlv);
    } else {
        DPRINT("Setting protection level failed with error: %d", ret);
        return ret;
    }
    
    ret = resetHaps();
    if (ret != kIOReturnSuccess) {
        DPRINT("Could not activate HAPS device");
    }
    return ret;
}

bool ToshibaHAPS::init(OSDictionary* properties) {
    DPRINT("ToshibaHAPS init");
    
    if (!super::init(properties))
        return false;
    
    haps = NULL;
    ptlv_io_reg_property = "ProtectionLevel";
    
    RSSS = OSSymbol::withCStringNoCopy("RSSS");
    PTLV = OSSymbol::withCStringNoCopy("PTLV");
    
    setProperty(ptlv_io_reg_property, (UInt32)0);
    return true;
}

void ToshibaHAPS::free() {
    super::free();
}

bool ToshibaHAPS::start(IOService* provider) {
    DPRINT("ToshibaHAPS starting");
    haps = OSDynamicCast(IOACPIPlatformDevice, provider);
  
    if (!super::start(provider))
        return false;
    
    //PT_LEVEL_MED: Enables accelerometers, no shock protection
    if(setCurrentPTLV(PT_LEVEL_MED) != kIOReturnSuccess)
      return false;
  
    this->registerService();
    return true;
}

void ToshibaHAPS::stop(IOService* provider) {
    super::stop(provider);
}
