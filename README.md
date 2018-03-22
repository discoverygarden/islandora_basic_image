# Basic Image Solution Pack

## Introduction

Creates Image Collection to hold image objects. Adds support for Image objects
based on JPEG/PNG/GIF image files.

## Requirements

This module requires the following modules/libraries:

* [Islandora](https://github.com/discoverygarden/islandora)
* [Tuque](https://github.com/islandora/tuque)
* [ImageMagick](https://drupal.org/project/imagemagick)

*To successfully create derivative data streams ImageMagick (TN & JPG) needs to
be installed on the server.*

## Installation

Install as
[usual](https://www.drupal.org/docs/8/extending-drupal-8/installing-drupal-8-modules).

## Configuration

Configure the image-tool kit to use ImageMagick rather than GD in
Configuration > Media > Image Toolkit
(admin/config/media/image-toolkit). If GD is selected, TN and JPG datastreams
will not be generated.

![Configuration](https://camo.githubusercontent.com/6ae64673716ddf1f58d0e4856d7d7a5d79845506/687474703a2f2f692e696d6775722e636f6d2f4f33735150654f2e706e67)

+Some features are disabled by default. Enable ImageMagick Advanced module.
+![0000 basic image
001](https://cloud.githubusercontent.com/assets/2738244/24003797/d6aea9da-0a3a-11e7-9c30-d40a0e5240b3.png)
If you wish to prevent image upscaling for images under 500x700 select the
configuration option present at Configuration > Islandora > Solution pack
configuration > Basic Image Solution Pack
(admin/config/islandora/solution_pack_config/basic_image).

## Documentation

Further documentation for this module is available at [our
wiki](https://wiki.duraspace.org/display/ISLANDORA/Basic+Image+Solution+Pack).

## Troubleshooting/Issues

Having problems or solved one? Create an issue, check out the Islandora Google
groups.

* [Users](https://groups.google.com/forum/?hl=en&fromgroups#!forum/islandora)
* [Devs](https://groups.google.com/forum/?hl=en&fromgroups#!forum/islandora-dev)

or contact [discoverygarden](http://support.discoverygarden.ca).

## Maintainers/Sponsors

Current maintainers:

* [discoverygarden](http://www.discoverygarden.ca)

## Development

If you would like to contribute to this module, please check out the helpful
[Documentation](https://github.com/Islandora/islandora/wiki#wiki-documentation-for-developers),
[Developers](http://islandora.ca/developers) section on Islandora.ca and create
an issue, pull request and or contact
[discoverygarden](http://support.discoverygarden.ca).

## License

[GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)
