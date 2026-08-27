A short list of the logging message IDs

## Message IDs

### AUTHRESULT

- **SData Format**: AuthResult
- **Description**: The Libki server responded to a login attempt

### BYPASS

- **SData Format**: BypassAttempt
- **Description**: Someone tried to bypass Libki

### CHECKINTERNET

- **SData Format**: CheckInternet
- **Description**: Libki is checking the internet against a particular URL

### CHECKINTERNETREPLY

- **SData Format**: ServerReply
- **Description**: The response from a particular internet check

### CLOSEEVENT

- **SData Format**: Nil
- **Description**: Libki received a close event and decides what to do with it.

### ENTERFUNC

- **SData Format**: FuncFrame
- **Description**: Logged at the debug level at the start of a function. Typically fired by the _ENTER_FUNC_ macro.

### LEAVEFUNC

- **SData Format**: FuncFrame
- **Description**: Logged at the debug level at the end of a function. Typically fired by the _LEAVE_FUNC_ macro.

### LOGIN

- **SData Format**: AuthRequest
- **Description**: Someone attempted to log in to Libki.

### LOGOUT

- **SData Format**: AuthRequest
- **Description**: Someone attempted to log out of Libki.

### NODELOCK

- **SData Format**: Nil
- **Description**: Libki is locking the computer

### NODEPOWER

- **SData Format**: NodePower
- **Description**: Node is changing the power state of the computer (e.g. shutdown, suspend)

### NODEUNLOCK

- **SData Format**: Nil
- **Description**: Libki is unlocking the computer

### ONLYRUNFOR

- **SData Format**: UserMatch
- **Description**: Libki evaluated whether it should actually be running for this user or not, based on the onlyRunFor
  setting.

### ONLYSTOPFOR

- **SData Format**: UserMatch
- **Description**: Libki evaluated whether is should actually be running for this user or not, based on the onlyStopFor
  setting.

### PRINTERFOUND

- **SData Format**: Printer
- **Description**: Libki has parsed a printer from it's main configuration.

### PRINTJOBRESULT

- **SData Format**: ServerReply
- **Description**: Libki has received a response after sending a print job.

### PRINTJOBSEND

- **SData Format**: PrintJob
- **Description**: Libki found a print job in the printer directories, and it's sending it.

### REGISTERNODE

- **SData Format**: Node
- **Description**: Libki is registering with the Libki server

### REGISTERNODERSULT

- **SData Format**: ServerReply
- **Description**: Libki has received a response after attempting to register

### RESERVATIONACK

- **SData Format**: Reservation
- **Description**: Libki is acknowledging a reservation it's received from the server

### SETTING

- **SData Format**: SettingValue
- **Description**: Reports a setting value, for debugging. Usually logged through LOG_SETTING without an unstructured
  message.

### STARTSHELL

- **SData Format**: Shell
- **Description**: Libki has started the user's shell after login

### QUERYUSER

- **SData Fomrat**: AuthRequest
- **Description**: Libki has requested this user's data

### QUERYPRINTER

- **SData Format**: PrinterQuery
- **Description**: Libki has requested a printer's data

### QUERYPRINTERRESULT

- **SData Format**: PrinterResult
- **Description**: Libki has received a response on a printer's data

## SData Formats

### AuthResult

- **SD-ID**: AuthResult@32473
- **Parameters**:
    - _authenticated_ - "yes" if the client authentication action succeeded, "no" if it didn't
    - _error_ - The error message of a failed auth, "-" if there was a successful auth.

### AuthRequest

- **SD-ID**: AuthRequest@32473
- **Paramters**:
    - _username_ - The username of the person attempting an authentication action
    - _action_ - The action the user is attempting to do (e.g. login)
    - _server_ - The URL that Libki sent the action request to

### BypassAttempt

- **SD-ID**: BypassAttempt@32473
- **Parameters**:
    - _attemptHash_ - The MD5 hash of the password typed in
    - _storedHash_ - The MD5 password hash stored in the configuration
    - _allowed_ - "yes" if Libki was disabled, "no" if it wasn't.

### CheckInternet

- **SD-ID**: CheckInternet@32473
- **Paramters**:
    - _server_ - The URL of the server Libki is checking against

### FuncFrame

- **SD-ID**: FuncFrame@32473
- **Parameters**:
    - _funcName_ - The name of the function that logged the event, usually made by the \_\_PRETTY_FUNCTION\_\_ macro.

### Node

- **SD-ID**: Node@32473
- **Parameters**:
    - _server_ - The server the node is registering to
    - _nodeName_ - The name of the node attempting to register
    - _version_ - The libki version of the node
    - _ageLimit_ - The age limit libki is setting for the node

### NodePower

- **SD-ID**: NodePower@32473
- **Parameters**:
    - _action_ - The power action being performed (e.g. shutdown, suspend)

### Printer

- **SD-ID**: Printer@32473
- **Parameters**:
    - _printerName_ - The name of the printer
    - _printerPath_ - The folder this printer's jobs are sent to

### PrinterQuery

- **SD-ID**: PrinterQuery@32473
- **Parameters**:
    - _printerName_ - The name of the printer

### PrinterResult

- **SD-ID**: PrinterResult@32473
- **Parameters**:
    - _printerName_ - The name of the printer
    - _currency_ - The currency the printer accepts
    - _costPerPage_ - How much it costs to print a page
    - _funds_ - How much money the user has for funds
    - _gratisBalance_ - How much the user can print for free
    - _gratisMethod_ - How the free pages is calculated
    - _error_ - If there were any errors getting the result

### PrintJob

- **SD-ID**: PrintJob@32473
- **Parameters**:
    - _printer_ - The name of the printer it was submitted to
    - _filename_ - The absolute file path the job was found at
    - _pageCount_ - The number of pages the print job has
    - _copies_ - The number of copies the print job has

### Reservation

- **SD-ID**: Reservation@32473
- **Parameters**:
    - _reservedFor_: The user holding the reservation

### ServerReply

- **SD-ID**: ServerReply@32473
- **Parameters**:
-
    - _networkError_: Any network errors that occurred (e.g. timeouts)
    - _statusCode_: Any HTTP errors that occurred, or "-" if there was no HTTP response due to a network error
    - _message_: The error message, or "-" if there was none

### SettingValue

- **SD-ID** SettingValue@32473
- **Parameters**:
    - _key_ - The name of the setting
    - _value_ - The value that the setting stored

### Shell

- **SD-ID**: Shell@32473
- **Parameters**:
    - _shell_ - The file path of the user shell

### UserMatch

- **SD-ID**: UserMatch@32473
- **Parameters**:
    - _user_ - The currently logged-in user
    - _storedUser_ - The user stored in the configuration
    - _allowed_ - "yes" if Libki ran for the user, "no" if it didn't.
