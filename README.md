# Diagnostic Client Library
Build status:
|GitHub Action|
|:-:|
[![CMake](https://github.com/Mandarein/diag-client-lib/actions/workflows/cmake.yml/badge.svg)](https://github.com/Mandarein/diag-client-lib/actions/workflows/cmake.yml)

## Diagnostic Client Library

Welcome to MANDAREIN Diagnostic Client library which is developed based on motivation taken from
[Adaptive AUTOSAR Diagnostic Specification 20-11](https://www.autosar.org/fileadmin/user_upload/standards/adaptive/20-11/AUTOSAR_SWS_Diagnostics.pdf)

MANDAREIN Diagnostic Client library is developed keeping in mind the requirements needed on the diagnostic tester side.
Some internal APIs is implemented based on API Reference mentioned in 
[Adaptive AUTOSAR Diagnostic Specification 20-11](https://www.autosar.org/fileadmin/user_upload/standards/adaptive/20-11/AUTOSAR_SWS_Diagnostics.pdf)

## Overview

Diagnostic Client library acts as diagnostic tester library which can be easily linked with any application which wants 
to send any diagnostic request to server and diagnose the ECU.

Diagnostic Client library currently supports below Diagnostic Protocols :-

- DoIP 
- UDS

The main advantage of Diagnostic Client library is that you can open multiple conversion(tester instance) for diagnosing multiple ECU at same time. 
You can check section [how to use diag-client-lib](#how-to-use-diag-client-lib) for more details

## Get Started

In this section, you can learn how to [build and install](#build-and-install)
Diagnostic Client Library and also learn [how to use diag-client-lib](#how-to-use-diag-client-lib)

### Build and Install

The following packages needs to be installed in order to build and install 
Diagnostic Client library:-

- cmake
- dlt-daemon
- boost >= 1.78.0

```bash
sh build.sh
```

### Dependencies

Diagnostic Client Library has dependencies with BOOST library and COVESA DLT logging.

BOOST Library is used for asio operations and COVESA DLT logging is used for sending DLT logs
to user.

In future use of BOOST library will be removed completely.

### How to use diag-client-lib

Diagnostic Client Library has to be linked first either statically or dynamically with executable before usage.

Diagnostic Client shared library can be built by setting the below CMake Flag to ON
```bash
"BUILD_SHARED_LIBS" : "ON",
```

Main instance of Diagnostic Client Libary should be created using `CreateDiagnosticClient` function call and config json file path should be passed as parameter 
```bash
std::unique_ptr<diag::client::DiagClient> diagclient =
            diag::client::CreateDiagnosticClient("etc/diag_client_config.json");
```

Multiple tester instance can be created using Diagnostic Client Libary main instance.

More information on the APIs to be used , check [doc_api_definition](doc/API_definitions.md) documentation

More information on the diag client config json file, refer [doc_config_json](doc/Config_Json_file_definitions.md) documentation

Check the example application [Examples](examples/) on how Diagnostic Client Libary should be linked and used.


## Known Defect
* No defect is identified yet.

You can add new issues with label `bug` for notifying us about any defect in library.

## Future Work
* Support of Vehicle Discovery Phase (Vehicle Identification/ Announcement) will be added
* DoIP with TLS
* Remove Boost Library dependency

For adding more features you can add new issues with label `enhancement` so that we can work on it.

## License

Full information of license is available in the [LICENSE](LICENSE) file of this project.

Boost License is available here [LICENSE](lib/libOsAbstraction/libBoost/LICENSE)

## Contact

Avijit Dey <iandiehard@outlook.com>

## Author

- > Avijit Dey