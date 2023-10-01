# Diagnostic Client Library

[![CMake](https://github.com/Iandiehard/diag-client-lib/actions/workflows/cmake.yml/badge.svg)](https://github.com/Iandiehard/diag-client-lib/actions/workflows/cmake.yml)

<!-- TOC -->
* [Diagnostic Client Library](#diagnostic-client-library)
  * [Diagnostic Client Library](#diagnostic-client-library-1)
  * [Overview](#overview)
  * [Get Started](#get-started)
    * [Build and Install](#build-and-install)
      * [In Linux :-](#in-linux--)
      * [In Windows :-](#in-windows--)
    * [How to use diag-client-lib](#how-to-use-diag-client-lib)
    * [Logging in diag-client-lib](#logging-in-diag-client-lib)
    * [Documentation in diag-client-lib](#documentation-in-diag-client-lib)
  * [Requirements](#requirements)
  * [Known Defect](#known-defect)
  * [Future Work](#future-work)
  * [License](#license)
  * [Contact](#contact)
  * [Author](#author)
<!-- TOC -->

## Diagnostic Client Library
Welcome to Diagnostic Client library that is developed based on motivation taken from
[Adaptive AUTOSAR Diagnostic Specification 21-11](https://www.autosar.org/fileadmin/standards/R21-11/AP/AUTOSAR_SWS_Diagnostics.pdf)

Diagnostic Client library is developed according to the requirements needed on the diagnostic tester side.
Some internal APIs is implemented based on API Reference mentioned in
[Adaptive AUTOSAR Diagnostic Specification 21-11](https://www.autosar.org/fileadmin/standards/R21-11/AP/AUTOSAR_SWS_Diagnostics.pdf)

## Overview
Diagnostic Client library acts as diagnostic tester library that is linked to a user application which wants to send diagnostic 
requests to multiple ECU over a network.

Diagnostic Client library supports below Diagnostic Protocols :-

- DoIP (Diagnostic over Internet Protocol)
- UDS  (Unified Diagnostic Services)

Diagnostic Client library supports opening of multiple conversation(tester instance) for sending diagnostic request to multiple ECU at the same time. 

Public Api's can be found in [include](diag-client-lib/appl/include) to understand the usage of Diagnostic Client library, 

## Get Started
In this section, you will learn how to [build and install](#build-and-install)
Diagnostic Client Library and also learn [how to use diag-client-lib](#how-to-use-diag-client-lib)

### Build and Install

#### In Linux :-
The following packages are needed to build and install Diagnostic Client library:-

| Package                                                       | Usage             | Recommended version |
|---------------------------------------------------------------|-------------------|---------------------|
| [CMake](https://cmake.org/cmake/help/latest/release/3.5.html) | build system      | \>=3.5              |
| [COVESA DLT](https://github.com/COVESA/dlt-daemon)            | logging & tracing | \>= 2.18.8          |
| [Boost](https://www.boost.org/)                               | asio operation    | \>= 1.79.0          |

You can also execute to install dependencies :-
```bash 
sh .github/setup.sh 
```
#### In Windows :-
The following packages are needed to build and install Diagnostic Client library:-

| Package                                                       | Usage             | Recommended version |
|---------------------------------------------------------------|-------------------|---------------------|
| [CMake](https://cmake.org/cmake/help/latest/release/3.5.html) | build system      | \>=3.5              |
| [Boost](https://www.boost.org/)                               | asio operation    | \>= 1.79.0          |
 
Note: Covesa DLT is not directly supported in Windows. For more information see [Dlt Issue](https://github.com/COVESA/dlt-daemon/issues/136)


### How to use diag-client-lib
Diagnostic Client Library has to be linked first either statically or dynamically with executable before usage.
Diagnostic Client library can be built as shared library by setting the below CMake Flag to ON
```cmake
BUILD_SHARED_LIBS : ON
```

Main instance of Diagnostic Client Library must be created using `CreateDiagnosticClient` method call by passing the path to 
[diag-client config json](diag-client-lib/appl/etc/diag_client_config.json) file as parameter.
```cpp
  // Create the Diagnostic client and pass the config for creating internal properties
  std::unique_ptr<diag::client::DiagClient> diag_client{
      diag::client::CreateDiagnosticClient("etc/diag_client_config.json")};
```
[diag-client config json](diag-client-lib/appl/etc/diag_client_config.json) file can be modified as per user requirements.

Once Diagnostic Client Library is instantiated and initialized, `GetDiagnosticClientConversation` can be used to get the tester/conversation instance
by passing the tester/conversation name.
```cpp
  // Get conversation for tester one by providing the conversation name configured
  // in diag_client_config file passed while creating the diag client
  diag::client::conversation::DiagClientConversation diag_client_conversation {
      diag_client->GetDiagnosticClientConversation("DiagTesterOne")};
```
Multiple tester instance can be created using these method as provided in the configuration json file.

Check the example application [Examples](examples) on how Diagnostic Client Library can be linked and used.
Example can be built too by enabling CMake Flag:-
```cmake
BUILD_EXAMPLES : ON
```

### Logging in diag-client-lib
Diagnostic Client Library supports logging and tracing by using the logging infrastructure from [COVESA DLT](https://github.com/COVESA/dlt-daemon).
Logging is switched OFF by default using the CMake Flag, can be switched ON by enabling the flag:-
```cmake
BUILD_WITH_DLT : ON
```
Note: DLT logging is not supported in Windows. So, CMake Flag must be switched OFF.

### Documentation in diag-client-lib
Diagnostic Client Library uses doxygen to generate the documentation of the public api's. 
Doxygen build is switched OFF by default using the CMake Flag, can be switched ON by enabling the flag:-
```cmake
BUILD_DOXYGEN : ON
```
In the future, we will have internal code documentation too.

## Requirements
Component requirements implemented are documented [REQ](docs/requirement/requirements.md)

## Known Defect
* No defect is identified yet.

You can add new issues with label `bug` for notifying us about any defect in library.

## Future Work
* Improve internal API documentation
* DoIP with TLS

For adding more features you can add new issues with label `enhancement` so that we can work on it.

## License
Full information on project license is available here [LICENSE](LICENSE).
Boost License is available here [LICENSE](diag-client-lib/lib/boost-support/LICENSE).

## Contact
For any other queries regarding `diag-client-lib`, please drop a mail to `iandiehard@outlook.com`.

## Author
Original idea, design and implementation done by Avijit Dey `iandiehard@outlook.com`.
