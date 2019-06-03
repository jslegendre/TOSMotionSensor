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

#ifndef TOSMotionSensor_hpp
#define TOSMotionSensor_hpp

#include <IOKit/IOService.h>
#include <IOKit/acpi/IOACPIPlatformDevice.h>
#include <IOKit/IOUserClient.h>

#define TOSMotionSensor SMCMotionSensor
#define TOSMotionSensorClient SMCMotionSensorClient

class TOSMotionSensor : public IOService
{
    OSDeclareDefaultStructors(TOSMotionSensor);
    friend class TOSMotionSensorClient;
    
public:
    virtual bool init(OSDictionary *dictionary = 0);
    virtual bool start(IOService *provider);
    virtual void stop(IOService *provider);
    
    const OSSymbol * findHCI();
    bool isAccelSupported();
    IOReturn getAxes(void *iBuf, void *oBuf);
protected:
    struct s_axes           { SInt16 x; SInt16 y; SInt16 z; };
private:
    IOACPIPlatformDevice*   TVAP;

    OSObject*               HCI_IN[6];
    OSObject*               HCI_OUT;
    OSArray*                axes;
    const OSSymbol*         hci_method; //HCI: Hardware Configuration Interface
    
    struct s_axes           _axes;
};

class TOSMotionSensorClient : public IOUserClient
{
    OSDeclareDefaultStructors(SMCMotionSensorClient);
public:
    virtual bool start(IOService* provider);
    virtual void stop(IOService* provider);
    virtual bool initWithTask(task_t owningTask, void *securityToken, UInt32 type, OSDictionary *properties);
    virtual IOReturn clientClose();
    virtual IOExternalMethod * getTargetAndMethodForIndex(IOService **targetP, UInt32 index);
protected:
    TOSMotionSensor* p_provider;
    task_t  p_task;
private:
    IOSimpleLock *axes_lock;
    
};

static const IOExternalMethod external_methods[1] =
{
    {
        NULL,
        (IOMethod)&TOSMotionSensor::getAxes,
        kIOUCStructIStructO,
        40,
        40
    }
};

#endif
