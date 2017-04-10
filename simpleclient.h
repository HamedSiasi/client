/*
 *
 * Copyright (C) u-blox Melbourn Ltd
 * u-blox Melbourn Ltd, Melbourn, UK
 *
 * All rights reserved.
 *
 * This source file is the sole property of u-blox Melbourn Ltd.
 * Reproduction or utilisation of this source in whole or part is
 * forbidden without the written consent of u-blox Melbourn Ltd.
 * hamed.siasi@u-blox.com
 *
 */

#ifndef __SIMPLECLIENT_H__
#define __SIMPLECLIENT_H__

#include "mbed-client/m2minterfacefactory.h"
#include "mbed-client/m2mdevice.h"
#include "mbed-client/m2minterfaceobserver.h"
#include "mbed-client/m2minterface.h"
#include "mbed-client/m2mobject.h"
#include "mbed-client/m2mobjectinstance.h"
#include "mbed-client/m2mresource.h"
#include "mbed-client/m2mconfig.h"
#include "security.h"
#include "mbed.h"


M2MInterface::BindingMode SOCKET_MODE = M2MInterface::UDP;//UDP_QUEUE;


struct MbedClientDevice {
    const char* Manufacturer;
    const char* Type;
    const char* ModelNumber;
    const char* SerialNumber;
};





class MbedClient: public M2MInterfaceObserver {
public:


	// (1) constructor for MbedClient object, initialize private variables
    MbedClient(struct MbedClientDevice device) {
        _interface = NULL;
        _bootstrapped = false;
        _error = false;
        _registered = false;
        _unregistered = false;
        _register_security = NULL;
        _value = 0;
        _object = NULL;
        _device = device;
    }




    // (2) de-constructor for MbedClient object, you can ignore this
    ~MbedClient() {
        if(_interface) {
            delete _interface;
        }
        if(_register_security){
            delete _register_security;
        }
    }



    // (3) debug printf function
    void trace_printer(const char* str) {
        printf("\r\n%s\r\n", str);
    }





    /*
     *  Interface
     *  Creates M2MInterface using which endpoint can
     *  setup its name, resource type, life time, connection mode,
     *  Currently only LwIPv4 is supported.
     */
    void create_interface(const char *server_address, void *handler = NULL) {
    	_server_address = server_address;
    	uint16_t listen_port = 0; // Network interface will randomize with port 0 defaul 5683

    	_interface = M2MInterfaceFactory::create_interface(*this,
                                                      	  MBED_ENDPOINT_NAME,       // endpoint name string
														  "",                       // endpoint type string
														  100,                      // lifetime
														  listen_port,              // listen port
														  MBED_DOMAIN,              // domain string
														  SOCKET_MODE,              // binding mode
														  M2MInterface::LwIP_IPv4,  // network stack
														  "");


    	printf("Connecting to %s\r\n", server_address);

    	if(_interface) {
    		_interface->set_platform_network_handler(handler);
    	}
    	else{
    		printf("No interface !\r\n");
    	}
    }




    // (5) check private variable to see if the registration was sucessful or not
    bool register_successful() {
    	printf("simpleClient - register_successful \r\n");
        return _registered;
    }



    // (6) check private variable to see if un-registration was sucessful or not
    bool unregister_successful() {
    	printf("simpleClient - unregister_successful \r\n");
        return _unregistered;
    }




    /*
     *  Creates register server object with mbed device server address and other parameters
     *  required for client to connect to mbed device server.
     */

    M2MSecurity* create_register_object() {
    	// create security object using the interface factory.
    	// this will generate a security ObjectID and ObjectInstance

    	printf("simpleClient - create_register_object \r\n");
        M2MSecurity *security = M2MInterfaceFactory::create_security(M2MSecurity::M2MServer);

        if(security) {
            security->set_resource_value(M2MSecurity::M2MServerUri, _server_address); 	                     // 1
            security->set_resource_value(M2MSecurity::SecurityMode, M2MSecurity::NoSecurity);	             // 3
            //security->set_resource_value(M2MSecurity::BootstrapServer, false);	                         // 2
            //security->set_resource_value(M2MSecurity::PublicKey, CERT, sizeof(CERT));	                     // 4
            //security->set_resource_value(M2MSecurity::ServerPublicKey, SERVER_CERT, sizeof(SERVER_CERT));	 // 5
            //security->set_resource_value(M2MSecurity::Secretkey, KEY, sizeof(KEY));	                     // 6
            //security->set_resource_value(M2MSecurity::SMSSecurityMode, M2MSecurity::NoSecurity);	         // 7
            //security->set_resource_value(M2MSecurity::SMSBindingKey, NULL);	                             // 8
            //security->set_resource_value(M2MSecurity::SMSBindingSecretKey, NULL);	                         // 9
            //security->set_resource_value(M2MSecurity::M2MServerSMSNumber, NULL);	                         // 10
            //security->set_resource_value(M2MSecurity::ShortServerID, NULL);	                             // 11
            //security->set_resource_value(M2MSecurity::ClientHoldOffTime, NULL);	                         // 12
        }
        return security;
    }






    /*
     * Creates device object which contains mandatory resources linked with
     * device endpoint.
     */

    M2MDevice* create_device_object() {
    	printf("simpleClient - create_device_object \r\n");
        M2MDevice *device = M2MInterfaceFactory::create_device();

        if(device) {
            device->create_resource(M2MDevice::Manufacturer, "ublox");                                      // 1
            device->create_resource(M2MDevice::DeviceType,   "nbiot");                                      // 2
            //device->create_resource(M2MDevice::ModelNumber,  _device.ModelNumber);                        // 3
            //device->create_resource(M2MDevice::SerialNumber, _device.SerialNumber);                       // 4
            //device->create_resource(M2MDevice::HardwareVersion,  "a.0");                                  // 5
            //device->create_resource(M2MDevice::FirmwareVersion, "b.0");                                   // 6
            //device->create_resource(M2MDevice::SoftwareVersion,  _device.ModelNumber);                    // 7
            //device->create_resource(M2MDevice::Reboot, _device.SerialNumber);                             // 8
            //device->create_resource(M2MDevice::FactoryReset,  _device.ModelNumber);                       // 9
            //device->create_resource(M2MDevice::AvailablePowerSources, _device.SerialNumber);              // 10
            //device->create_resource(M2MDevice::PowerSourceVoltage,  _device.ModelNumber);                 // 11
            //device->create_resource(M2MDevice::PowerSourceCurrent, _device.SerialNumber);                 // 12
            //device->create_resource(M2MDevice::BatteryLevel,  _device.ModelNumber);                       // 13
            //device->create_resource(M2MDevice::BatteryStatus, _device.SerialNumber);                      // 14
            //device->create_resource(M2MDevice::MemoryFree,  _device.ModelNumber);                         // 15
            //device->create_resource(M2MDevice::MemoryTotal,  _device.ModelNumber);                        // 16
            //device->create_resource(M2MDevice::ErrorCode,  _device.ModelNumber);                          // 17
            //device->create_resource(M2MDevice::ResetErrorCode, _device.SerialNumber);                     // 18
            //device->create_resource(M2MDevice::CurrentTime,  _device.ModelNumber);                        // 19
            //device->create_resource(M2MDevice::UTCOffset, _device.SerialNumber);                          // 20
            //device->create_resource(M2MDevice::Timezone,  _device.ModelNumber);                           // 21
            //device->create_resource(M2MDevice::SupportedBindingMode, _device.SerialNumber);               // 22
        }
        return device;
    }




//    M2MServer* create_server_object() {
//    	M2MServer* server = M2MInterfaceFactory::create_server();
//        if(server) {
//        	server->create_resource(M2MServer::Lifetime, 10);
//
////        	server->set_resource_value(M2MServer::ShortServerID, "test");
//        	//server->set_resource_value(M2MServer::Lifetime, 10);
////        	server->set_resource_value(M2MServer::DefaultMinPeriod, "test");
////        	server->set_resource_value(M2MServer::DefaultMaxPeriod, "test");
////        	server->set_resource_value(M2MServer::Disable, "test");
////        	server->set_resource_value(M2MServer::DisableTimeout, "test");
////        	server->set_resource_value(M2MServer::NotificationStorage, "test");
////        	server->set_resource_value(M2MServer::Binding, "test");
////        	server->set_resource_value(M2MServer::RegistrationUpdate, "test");
//        }
//        return server;
//    }



//    M2MFirmware* create_firmware_object() {
//    	M2MFirmware* firmware = M2MInterfaceFactory::create_firmware();
//    	if(firmware) {
//        }
//    	return firmware;
//    }




    /*
     * register an object
     */
    void test_register(M2MSecurity *register_object, M2MObjectList object_list){
    	printf("simpleClient - test_register \r\n");
        if(_interface) {
        	_interface->register_object(register_object, object_list);
        }
    }




    /*
    * unregister all objects
    */
    void test_unregister() {
    	printf("simpleClient - test_unregister \r\n");
        if(_interface) {
            _interface->unregister_object(NULL); // NULL will unregister all objects
        }
    }





    // (11)
    //
    // Callback from mbed client stack when the bootstrap
    // is successful, it returns the mbed Device Server object
    // which will be used for registering the resources to
    // mbed Device server.
    //
    void bootstrap_done(M2MSecurity *server_object){
    	printf("simpleClient - object_registered \r\n");
        if(server_object) {
            _bootstrapped = true;
            _error = false;
            trace_printer("Bootstrapped");
        }
    }




    // (12)
    //
    // Callback from mbed client stack when the registration
    // is successful, it returns the mbed Device Server object
    // to which the resources are registered and registered objects.
    //
    void object_registered(M2MSecurity */*security_object*/, const M2MServer &/*server_object*/){
    	printf("simpleClient - object_registered \r\n");
        _registered = true;
        _unregistered = false;
        printf("Registered object successfully ! \r\n\n\n\n\n\n");
    }




    // (13)
    //
    // Callback from mbed client stack when the unregistration
    // is successful, it returns the mbed Device Server object
    // to which the resources were unregistered.
    //
    void object_unregistered(M2MSecurity */*server_object*/){
    	printf("simpleClient - object_unregistered \r\n");
        _unregistered = true;
        _registered = false;
        printf("Unregistered Object Successfully ! \r\n\n\n\n\n\n\n\n\n");
    }





    // (14)
    /*
    * Callback from mbed client stack when registration is updated
    */
    void registration_updated(M2MSecurity */*security_object*/, const M2MServer & /*server_object*/){
        /* The registration is updated automatically and frequently by the
        *  mbed client stack. This print statement is turned off because it
        *  tends to happen alot.
        */
        printf("registration_updated \r\n\n");
    }





    // Callback from mbed client stack if any error is encountered
    // during any of the LWM2M operations. Error type is passed in
    // the callback.
    void error(M2MInterface::Error error){
        _error = true;
        switch(error){
            case M2MInterface::AlreadyExists:
                trace_printer("[ERROR:] M2MInterface::AlreadyExist");
                break;
            case M2MInterface::BootstrapFailed:
                trace_printer("[ERROR:] M2MInterface::BootstrapFailed");
                break;
            case M2MInterface::InvalidParameters:
                trace_printer("[ERROR:] M2MInterface::InvalidParameters");
                break;
            case M2MInterface::NotRegistered:
                trace_printer("[ERROR:] M2MInterface::NotRegistered");
                break;
            case M2MInterface::Timeout:
                trace_printer("[ERROR:] M2MInterface::Timeout");
                break;
            case M2MInterface::NetworkError:
                trace_printer("[ERROR:] M2MInterface::NetworkError");
                break;
            case M2MInterface::ResponseParseFailed:
                trace_printer("[ERROR:] M2MInterface::ResponseParseFailed");
                break;
            case M2MInterface::UnknownError:
                trace_printer("[ERROR:] M2MInterface::UnknownError");
                break;
            case M2MInterface::MemoryFail:
                trace_printer("[ERROR:] M2MInterface::MemoryFail");
                break;
            case M2MInterface::NotAllowed:
                trace_printer("[ERROR:] M2MInterface::NotAllowed");
                break;
            case M2MInterface::SecureConnectionFailed:
                trace_printer("[ERROR:] M2MInterface::SecureConnectionFailed");
                break;
            case M2MInterface::DnsResolvingFailed:
                trace_printer("[ERROR:] M2MInterface::DnsResolvingFailed");
                break;

            default:
                break;
        }
    }







    // (15)
    /*
     *  Callback from mbed client stack if any value has changed
     *  during PUT operation. Object and its type is passed in
     *  the callback.
     *  BaseType enum from m2mbase.h
     *  Object = 0x0, Resource = 0x1, ObjectInstance = 0x2, ResourceInstance = 0x3
     */
    void value_updated(M2MBase *base, M2MBase::BaseType type) {
    	printf("simpleClient - value_updated \r\n");
        printf("PUT Request Received! \r\n\n\n\n\n");
    }



    // (16)
    /*
    * update the registration period
    */
    void test_update_register() {
        if (_registered) {
            _interface->update_registration(_register_security, 0);
        }
    }





    // (17)
    /*
    * manually configure the security object private variable
    */
   void set_register_object(M2MSecurity *register_object) {
        if (_register_security == NULL) {
            _register_security = register_object;
        }
    }




private:

    /*
    *  Private variables used in class
    */
    M2MInterface    	     *_interface;
    M2MSecurity              *_register_security;
    M2MObject                *_object;
    volatile bool            _bootstrapped;
    volatile bool            _error;
    volatile bool            _registered;
    volatile bool            _unregistered;
    int                      _value;
    struct MbedClientDevice  _device;
    String                   _server_address;
};


#endif // __SIMPLECLIENT_H__
