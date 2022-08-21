# Diagnostic Client Library


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

Diagnostic Client Library currently has dependencies with BOOST library and COVESA DLT logging.

BOOST Library is used for asio operations and COVESA DLT logging is used for sending DLT logs
to user.

In future use of BOOST library will be removed completely.

### How to use diag-client-lib

1. Open single tester conversion

2. Open multiple tester conversion

## Known Defect
* No defect is identified yet.

You can add new issues with label `bug` for notifying us about any defect in library.

## Future Work
* Support of Vehicle Discovery Phase (Vehicle Identification/ Announcement) will be added
* DoIP with TLS
* Remove use of Boost Library

For adding more features you can add new issues with label `enhancement` so that we can work on it.

## License

Full information of license is available in the LICENSE file of this project.

Boost License is available here [LICENSE](lib/libOsAbstraction/libBoost/LICENSE)

## Contact

Avijit Dey <iandiehard@outlook.com>

## Author

- > Avijit Dey