[Click here](../README.md) to view the README.

## Design and implementation

The design of this application is minimalistic to get started with code examples on PSOC&trade; Edge MCU devices. All PSOC&trade; Edge E84 MCU applications have a dual-CPU three-project structure to develop code for the CM33 and CM55 cores. The CM33 core has two separate projects for the secure processing environment (SPE) and non-secure processing environment (NSPE). A project folder consists of various subfolders, each denoting a specific aspect of the project. The three project folders are as follows:

**Table 1. Application projects**

Project | Description
--------|------------------------
*proj_cm33_s* | Project for CM33 secure processing environment (SPE)
*proj_cm33_ns* | Project for CM33 non-secure processing environment (NSPE)
*proj_cm55* | CM55 project

<br>

In this code example, at device reset, the secure boot process starts from the ROM boot with the secure enclave (SE) as the root of trust (RoT). From the secure enclave, the boot flow is passed on to the system CPU subsystem where the secure CM33 application starts. After all necessary secure configurations, the flow is passed on to the non-secure CM33 application. 

This example implements two RTOS tasks to demonstrate Azure IoT Hub features: Cloud to Device (C2D), Telemetry, methods, Device Twin, and plug and play (PnP). The main function initializes the BSP and the retarget-io library and calls the Menu function which presents the list of Azure features to the user. Upon selection of a feature, a task is created for running the feature. Every feature task requires valid certificates or tokens that need to be passed either from the flash or secure hardware. The application uses the MQTT client library to connect to the Azure IoT Hub.
After the validation of IoT device credentials, the data exchange can then take place between the Hub and the device.

- **SAS Authentication**

   SAS tokens are generated using symmetric keys provided during device enrollment. The device then uses this key to generate SAS tokens. These SAS tokens have a hashed signature, which is used to verify the authenticity of these tokens. Once the device is authenticated, these SAS tokens are used to connect to Azure IoT Hub and send messages.

- **X.509 authentication**

   X.509 certificate PKI is used to authenticate devices to the IoT Hub and secure the IoT Hub endpoints. The process begins with registering and uploading the X.509 certificates to an IoT Hub which will be used for authentication of IoT devices to the IoT Hub whenever they connect. This authentication process saves you from generating private secure keys for every IoT device. With the X.509 CA feature, you need to register the certificate just once, and then use it to connect and authenticate as many devices as we want.

All the source files related to the application are placed under the _/source_ directory:

 File | Description
 ------ | ------
 _azure_common.h_ | Contains public interfaces for different Azure feature tasks.
 _main.c_ | Initializes the BSP and the retarget-io library and calls the menu task.
 _menu_task.c_ | Contains functions related to user input and Azure feature task creation.
 _mqtt_iot_azure_device_demo_app.c_ | Contains functions related to Azure features - Cloud To Device, Telemetry, methods, Device Twin.
 _mqtt_iot_hub_pnp.c_ | Contains the functions related to the Azure plug and play feature.
 _mqtt_iot_common.c_ | Contains functions common to Azure applications.
 _mqtt_iot_common.h_ | Contains public interfaces common to Azure applications.
 _mqtt_main.h_ | Contains public interfaces related to Azure features and MQTT broker details, Wi-Fi configuration macros such as SSID, password, certificates, and keys.

All the scripts needed for this example are placed under the _/scripts_ directory.

 File | Description
 ------ | ------
 format_X509_cert_key.py | Python script to convert certificate/key to string format
