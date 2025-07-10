This project is intended for demonstration purposes only. It is not intended for
use in a production environment.

# smc-hello-world

This project is used as an example on how to use
[smc-common](https://github.com/google/smc-common) repository. This can be run
on AST1030 or AST1035 chips but you should be able to run this on other Zephyr
supported chips by adding board .overlay and .conf files. This code has been
tested on a ASt1035 chip.

smc-hello-world mocks the following redfish topology.

-   `/redfish/v1/Chassis`

    -   `/redfish/v1/Chassis/SATA_0`

        -   `/redfish/v1/Chassis/SATA_0/Drives`
            -   `/redfish/v1/Chassis/SATA_0/Drives/SATA_0`
                -   `/redfish/v1/Chassis/SATA_0/Drives/SATA_0/Actions/Drive.Reset`
        -   `/redfish/v1/Chassis/SATA_0/Sensors`
            -   `/redfish/v1/Chassis/SATA_0/Sensors/Temp`

    -   `/redfish/v1/Chassis/SATA_1`

        -   `/redfish/v1/Chassis/SATA_1/Drives`
            -   `/redfish/v1/Chassis/SATA_1/Drives/SATA_1`
                -   `/redfish/v1/Chassis/SATA_1/Drives/SATA_1/Actions/Drive.Reset`
        -   `/redfish/v1/Chassis/SATA_1/Sensors`
            -   `/redfish/v1/Chassis/SATA_1/Sensors/Temp`

    -   `/redfish/v1/Chassis/Tray`

        -   `/redfish/v1/Chassis/Tray/Controls`

            -   `/redfish/v1/Chassis/Tray/Controls/fans`
            -   `/redfish/v1/Chassis/Tray/Controls/hdd_pid`
            -   `/redfish/v1/Chassis/Tray/Controls/vr_pid`

        -   `/redfish/v1/Chassis/Tray/Sensors`

            -   `/redfish/v1/Chassis/Tray/Sensors/Fan_duty`
            -   `/redfish/v1/Chassis/Tray/Sensors/Fan_tach`
            -   `/redfish/v1/Chassis/Tray/Sensors/Sen_current`
            -   `/redfish/v1/Chassis/Tray/Sensors/Sen_power`
            -   `/redfish/v1/Chassis/Tray/Sensors/Sen_temperature`
            -   `/redfish/v1/Chassis/Tray/Sensors/Sen_voltage`

-   `/redfish/v1/Managers`

    -   `/redfish/v1/Managers/smc`
        -   `/redfish/v1/Managers/smc/ManagerDiagnosticData`

-   `/redfish/v1/Storage`

    -   `/redfish/v1/Storage/SATA`
        -   `/redfish/v1/Storage/SATA/Controllers`
            -   `/redfish/v1/Storage/SATA/Controllers/sata_controller`

-   `/redfish/v1/UpdateService`

    -   `/redfish/v1/UpdateService/FirmwareInventory`
        -   `/redfish/v1/UpdateService/FirmwareInventory/smc`

## Capabilities

-   Configured to respond to PLDM RDE commands with MCTP serial over CDC ACM
    over USB.
-   Support expand 1 (Currently only ChassisCollection and SensorCollection will
    expand)
-   Configured a ADC sensor for `ADC0` device at `channel 0` which will provide
    the `/redfish/v1/Chassis/Tray/Sensors/Sen_voltage` reading.
-   All the other sensors are set to dummy values.
-   Configured device `PWM` to drive a fan.
-   Configured the PID controller to run 2 closed loop PID loops.
    -   Each PID loop will generate a fan duty cycle based on corresponding
        input temperatures.
    -   The maximum fan duty cycle generated will be used to run the fan.
    -   The fan does not have a secondary software PID controller to drive fan
        duty cycle to the desired target.
-   Enabled Watchdog resets in case the chip get "stuck".
-   Using AST1035 UART5 for console output.
-   Configured shell support for debugging. `help` command will display all the
    available shell commands.
-   An OEM field (`BuildInfo`) added to SoftwareInventory schema to display any
    embedded build related information.
-   Mocks drive power state allowing us to turn on/off drives via RDE.
-   Allows manual fan control over RDE.

## Building smc-hello-world application

This application is built using Zephyr's standard development workflow based on
Git and West. Zephyr's
[Getting Started Guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html)
describes how to set up this workflow. You should start by installing system
dependencies, West and the SDK. The guide instructs you to create a virtual
environment but you can skip this for now.

Once the dependencies are installed, you are ready to build this application.

We can use `west` to download all the repositories needed. Let's say we want to
create a workspace folder `test-hello-world`. Use the following command for
that.

```
$ west init -m https://github.com/google/smc-hello-world --mr main test-hello-world
$ cd test-hello-world/
$ west update
```

Once all the repositories are synced, we need to patch the third party repos
used. For this we can run the `smc-common/patch_third_party_repos.sh` from the
workspace.

```
$ smc-common/patch_third_party_repos.sh
```

Now we are ready to build the application.

```
$ west build -p auto -b ast1030_evb  smc-hello-world
```

The binary will be inside `test-hello-world/build/zephyr/zephyr.bin`
