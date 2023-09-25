# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.2.31]
### Added
- Add ability to allow automatic self-created guest logins

## [2.2.30]
### Added
- Skip print jobs that are less than 2kb in size ( they are likely still being written )

## [2.2.29]
### Added
- Skip print jobs that are not finished writing to disk

## [2.2.28]
### Added
- Add support for keyboard input when checking for inactivity on Windows

## [2.2.27]
### Added
- Transmit client ip address, mac address and hostname to the server. Requires server 4.7.7 or greater

## [2.2.26]
### Added
- Add ability to specify Libki client logs dir using an environment variable LIBKI_LOGS_DIR

## [2.2.25]
### Changed
- Send client version with every server request, not use client registration
- Add checks to ensure print jobs are renamed and readable before attempting to updload

## [2.2.24]
### Added
- Add addional guards to ClientTimeNotificationFrequency and ClientTimeWarningThreshold

## [2.2.23]
### Changed
- Reintroduce network error message for network connection issues

## [2.2.22]
### Added
- Hide password field if server setting EnableClientPasswordlessMode is enabled

## [2.2.21]
### Changed
- Centralized error message display logic in network client. Error message will now go away when network error is resolved

## [2.2.20]
### Added
- Add ability to set time remaining notification frequency and warning threshold

## [2.2.19]
### Added
- Log shutdown events received by the client

## [2.2.18]
### Changed
- Show server error code rather than message

## [2.2.17]
### Added
- Add client name to login screen #88

## [2.2.16] [r22.11]
### Fixed
- Fix possible memory leak caused by Internet connectivity checker

## [2.2.15]
### Changed
- Move internet connectivity sites list to a server side setting

## [2.2.14]
### Fixed
- Remove windows standard window bar from Libki login screen introduced in 2.2.12

## [2.2.13]
### Added
- Improve keeping guard screens at the forefront

## [2.2.12]
### Fixed
- Fix crashes related to session locking

## [2.2.11] [r22.08]
### Added
- Added checks and visual alerts on the login screen if the client has lost server and/or general Internet connectivity

## [2.2.10]
### Added
- Added more thorough debugging / logging statements to almost all methods

## [2.2.9]
### Added
- Add the ability to specify multiple usernames for onlyRunFor and OnlyStopFor. List should be comma delimited e.g. "Name1,Name2,Name3"

## [2.2.8]
### Fixed
- Fix bug where "lock session" button does not show up on the first run

## [2.2.7]
### Added
- Add logging to file in user roaming appdata directory
- Add ability to set custom text for Terms of Service on the message and in the "Show Details" section

## [2.2.6]
### Added
- Add ability to show time in system try icon and/or a "splash" in the corner of the screen

## [2.2.5]
### Added
- Update the installer to add the Windows 10 delayed loading registry edit fix: https://thegeekpage.com/startupdelayinmsec/

## [2.2.4]
### Fixed
- Handle SSL errors

## [2.2.3]
### Added
- Client now attempts to create printer directories if they do not already exist.

## [2.2.2] [r20.11]
### Added
- Add ability to remotely shutdown, restart, and wake-on-lan clients
### Changed
- Update Norvegian translation

## [2.2.1] - 2020-06-03
### Added
- Bug fix. If banners wasn't being used, an error message was displayed about it not being able to find the host.

## [2.2.0] - 2020-05-29
### Added
- Added the ability for users to temporarily lock an active session, in order to step away from the client. Feature is enabled via a setting on the Libki server. Requires Libki Server 4.2.4 or later.

## [2.1.0] - 2020-05-27
### Added
- This changelog
- Add ability to remotely log a Libki client in as a guest from the admin interface. Feature requires server version 4.2.0 or later.
