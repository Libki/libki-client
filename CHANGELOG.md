# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
- Add logging to file in user roaming appdata directory
- Add ability to set custom text for Terms of Service on the message and in the "Show Details" section

## [2.2.5]
- Update the installer to add the Windows 10 delayed loading registry edit fix: https://thegeekpage.com/startupdelayinmsec/

## [2.2.4]
- Handle SSL errors

## [2.2.3]
- Client now attempts to create printer directories if they do not already exist.

## [2.2.2] [r20.11]
- Add ability to remotely shutdown, restart, and wake-on-lan clients
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
