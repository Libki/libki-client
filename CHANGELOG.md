# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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
