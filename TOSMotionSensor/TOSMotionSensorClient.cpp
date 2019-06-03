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

#include "TOSMotionSensor.hpp"

#ifdef DEBUG
#define DPRINT(fmt, ...) \
IOLog("TOSMotionSensorClient::%s:%d(): " fmt, __func__, __LINE__, ##__VA_ARGS__);
#else
#define DPRINT(fmt, ...) do {} while (0)
#endif

#define super IOUserClient
OSDefineMetaClassAndStructors(SMCMotionSensorClient, IOUserClient);

bool TOSMotionSensorClient::initWithTask(task_t owningTask, void *securityToken, UInt32 type, OSDictionary *properties) {
    DPRINT("TOS1900 initwithtask");
    
    if(!super::initWithTask(owningTask, securityToken, type)) {
        DPRINT("TOS1900: could not initWithTask");
        return false;
    }
    
    p_task = owningTask;
    return true;
}

IOReturn TOSMotionSensorClient::clientClose() {
    p_task = NULL;
    p_provider = NULL;
  
    if(terminate())
      return kIOReturnSuccess;
    
    return kIOReturnError;
}

IOExternalMethod * TOSMotionSensorClient::getTargetAndMethodForIndex(IOService **targetP, UInt32 index) {
    DPRINT("called");
    *targetP = p_provider;
    
    if(index == 5)      //Index 5 is the orginal SMCMotionSensor selector to get accelerometer data
        index = 0;
    
    return (IOExternalMethod*)&external_methods[index];
}

bool TOSMotionSensorClient::start(IOService* provider) {
    DPRINT("called");
    if (!super::start(provider))
        return false;
    
    p_provider = OSDynamicCast(TOSMotionSensor, provider);
    registerService();
    return true;
}

void TOSMotionSensorClient::stop(IOService* provider) {
    DPRINT("called");
    super::stop(provider);
}
