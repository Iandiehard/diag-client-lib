## Diagnostic Client Library Requirements
The following software requirements are derived from ISO 22900-2 2022 (Diagnostic protocol data unit(D-PDU API)) and
[Adaptive AUTOSAR Diagnostic Specification 21-11](https://www.autosar.org/fileadmin/standards/R21-11/AP/AUTOSAR_SWS_Diagnostics.pdf)

### REQ: DiagClientLib-Library-Support
Diagnostic client library shall be used as a shared or static library. Diagnostic client library shall  be easily 
linkable with any executable and shall be ready to use.

### REQ: DiagClientLib-DoIP-Support
Diagnostic client library only provide support for DoIP(Diagnostic Over IP) as a diagnostic communication protocol 
for sending diagnostic request to Diagnostic Servers.

### REQ: DiagClientLib-MultipleTester-Connection
Diagnostic client library shall support creation of multiple diagnostic logical connection towards a single ECU or 
towards multiple ECUs. Each of these connection is known as Diagnostic Client Conversation.

### REQ: DiagClientLib-ComParam-Settings
Diagnostic client library shall support setting of diagnostic communication protocol parameter value via JSON file. 
In this case ComParam of DoIP protocol shall be configured via JSON file provided. A set of ComParams can be used to 
individually define communication to a single ECU or a functional group of ECUs.

### REQ: DiagClientLib-Construction
Diagnostic client library shall provide an API to construct an instance of Diagnostic Client Library with specific 
settings(ComParam etc).

### REQ: DiagClientLib-Destruction
Diagnostic client library shall provide an API to destruct the already created instance of Diagnostic Client Library.

### REQ: DiagClientLib-Initialization
Diagnostic client library shall provide an API to initialize the library during start-up phase.

### REQ: DiagClientLib-DeInitialization
Diagnostic client library shall provide an API to de-initialize the library and stop all the internal processing during
shutdown phase.

### REQ: DiagClientLib-Conversation-Construction
Diagnostic client library shall provide an API to construct the Diagnostic Client Conversation instance required for 
logical connection towards single or multiple ECUs.

### REQ: DiagClientLib-Conversation-Destruction
Diagnostic client library shall provide an API to destruct the existing Diagnostic Client Conversation instance.

### REQ: DiagClientLib-Conversation-StartUp
Diagnostic client library shall provide an API to start the Diagnostic Client Conversation instance.

### REQ: DiagClientLib-Conversation-Shutdown
Diagnostic client library shall provide an API to shut down the Diagnostic Client Conversation instance.

### REQ: DiagClientLib-Conversation-Connect
Diagnostic client library shall provide an API for establishing diagnostic logical connection towards single 
or multiple ECUs.

### REQ: DiagClientLib-Conversation-Disconnect
Diagnostic client library shall provide an API to disconnect the diagnostic logical connection towards single
or multiple ECUs.

### REQ: DiagClientLib-Conversation-DiagRequestResponse
Diagnostic client library shall provide an API to send diagnostic request towards the connected ECU/ECUs and 
receive diagnostic response and provide back the diagnostic response to the user.
