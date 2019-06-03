/*
 * Bare minimum setup to poll SMCMotionSensor service
 * Build:
 *   gcc -framework CoreFoundation -framework IOKit test.c -o test
 */

#include <stdio.h>
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>

typedef struct {
    short x;
    short y;
    short z;
} axes;

int main(int argc, const char * argv[]) {
    CFMutableDictionaryRef dict = IOServiceMatching("SMCMotionSensor");
    if(!dict) {
        printf("Could not find service\n");
        return 1;
    }
    
    kern_return_t result;
    io_connect_t connection;
    io_service_t device = 0;
    
    device = IOServiceGetMatchingService(kIOMasterPortDefault, dict);
    if(!device) {
        printf("No device\n");
        return 1;
    }
    
    
    if(IOServiceOpen(device, mach_task_self(), 0, &connection) != KERN_SUCCESS) {
        printf("Could not open device\n");
        return 1;
    }
    
    size_t smc_arg_size = 40;
    axes *out_struct = NULL;
    
    void* smc_in = malloc(smc_arg_size);
    memset(smc_in, 1, smc_arg_size);
    
    void* smc_out = malloc(smc_arg_size);
    memset(smc_out, 0, smc_arg_size);
    
    result = IOConnectCallStructMethod(connection,
                                       5,
                                       smc_in,
                                       40,
                                       smc_out,
                                       &smc_arg_size);
    if(result != KERN_SUCCESS) {
        printf("Could not call struct method\n");
        goto end;
    }
    
    out_struct = (axes*)smc_out;
    printf("Success\nAccelerometer reading: { x: %d, y: %d, z: %d }\n", out_struct->x, out_struct->y, out_struct->z);
    
end:
    free(smc_in);
    free(smc_out);
    
    if(result != KERN_SUCCESS)
        return 1;
        
    return 0;
}

